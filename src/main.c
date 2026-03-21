#include "main.h"
#include "config.h"
#include "socket.h"

#include <assert.h>
#include <chlsdl-modules/chlsdl-common/common.h>
#include <chlsdl-modules/chlsdl-common/print.h>
#include <chlsdl-modules/chlsdl-common/util/util.h>
#include <chlsdl/macros.h>
#include <chlsdl/module.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#ifdef USE_LIBNOTIFY
#    include <chlsdl-modules/chlsdl-common/util/notify.h>
#endif

static int nlibs;

static struct module_lib {
    const char * path;
    const char * name;
    void *       dl_lib;
} libs[128];

static int                    nmodules;
static const struct module ** modules;

const char * g_cache_dir     = NULL;
const char * g_config_dir    = NULL;
const char * g_downloads_dir = NULL;

static struct chlsdl_config * config;

static int                  local_socket;
static struct curl_buffer * js_data;

static void
cleanup(int sig)
{
#ifdef USE_LIBNOTIFY
    chlsdl_notify_uninit();
#endif

    unset_curl_user_agent();
    unset_curl_logfile_path();
    curl_buffer_dealloc(js_data);
    socket_close(local_socket);
    for (int i = nlibs; i-- > 0;) {
        modules[i]->deinit();
        dlclose(libs[i].dl_lib);
    }
    free(modules);
    free((char *)g_downloads_dir);
    config_destroy(config);
    free((char *)g_config_dir);
    free((char *)g_cache_dir);
    exit(sig);
}

static void
set_cache_dir()
{
    char * cache_dir = getenv("XDG_CACHE_HOME");
    if (!cache_dir) {
        print_warn("XDG_CACHE_HOME is unset. using '$HOME/.cache/" PROGRAM_NAME
                   "' instead\n");

        char * home = getenv("HOME");
        cache_dir   = svconcat("%s/.cache/" PROGRAM_NAME, home);
        assert(cache_dir);
    } else {
        cache_dir = svconcat("%s/" PROGRAM_NAME, cache_dir);
        assert(cache_dir);
    }

    g_cache_dir = cache_dir;

    if (mkdir(g_cache_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST) {
        print_error("failed to create directory: '%s'\n", g_cache_dir);
        cleanup(1);
    }
}

static void
set_config_dir()
{
    char * config_dir = getenv("XDG_CONFIG_HOME");
    if (!config_dir) {
        print_warn(
            "XDG_CONFIG_HOME is unset. using '$HOME/.config/" PROGRAM_NAME
            "' instead\n");

        char * home = getenv("HOME");
        config_dir  = svconcat("%s/.config/" PROGRAM_NAME, home);
        assert(config_dir);
    } else {
        config_dir = svconcat("%s/" PROGRAM_NAME, config_dir);
        assert(config_dir);
    }

    g_config_dir = config_dir;

    if (mkdir(g_config_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST) {
        print_error("failed to create directory: '%s'\n", g_config_dir);
        cleanup(1);
    }
}

static void
set_downloads_dir()
{
    assert(g_config_dir);
    g_downloads_dir = svconcat("%s/downloads", g_config_dir);

    if (mkdir(g_downloads_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST) {
        print_error("failed to create directory: '%s'\n", g_downloads_dir);
        cleanup(1);
    }
}

static void
find_modules(int * nout, struct module_lib * out)
{
    /*
     * FIXME: find modules placed in appropriate system directory on nixos
     */
    static const char * dir = MODULES_PATH;

    print_debug_warn("looking for modules in: '%s'\n", dir);
    DIR * modules = opendir(dir);
    assert(modules);

    struct dirent * ent;
    int             i = 0;
    while ((ent = readdir(modules))) {
        const char * ext = strchr(ent->d_name, '.');
        if (ext && strcmp(ext, ".so") == 0) {
            const char * start = ent->d_name + 3;
            out[i].name = strndup(start, strchr(ent->d_name, '.') - start);
            assert(out[i].name);

            out[i].path = svconcat("%s/%s", dir, ent->d_name);
            assert(out[i++].path);
            print_debug_warn("found module: '%s'\n", out[i - 1].path);
        }
    }
    closedir(modules);
    *nout = i;
}

static void
init_modules(const struct chlsdl_data * chlsdl_data)
{
    find_modules(&nlibs, libs);
    if (nlibs == 0)
        return (void)print_debug_warn("no modules found\n");

    print_debug_warn("loading %d modules\n", nlibs);

    modules = malloc(sizeof(*modules) * nlibs);
    assert(modules);

    for (int i = 0; i < nlibs; ++i) {
        struct module_lib * module = &libs[i];
        print_debug_warn("loading '%s'\n", module->name);
        module->dl_lib = dlopen(module->path, RTLD_NOW | RTLD_GLOBAL);

        if (!module->dl_lib) {
            print_debug_error("'%s'\n", dlerror());
            assert(0);
        }

        /* get *_init() address */
        chlsdl_defer char * init_symbol_name
            = svconcat("%s_init", module->name);
        assert(init_symbol_name);

        print_debug_warn("getting '%s()' address\n", init_symbol_name);
        module_init init = dlsym(module->dl_lib, init_symbol_name);

        if (!init) {
            print_debug_error("'%s'\n", dlerror());
            assert(0);
        }

        print_debug_success("'%s()' address %p\n", init_symbol_name, init);

        /* call *_init() to get address of module struct */
        modules[nmodules] = init(chlsdl_data);
        assert(modules[nmodules]);
        ++nmodules;
    }
}

static char *
get_line_from_string(const char * s)
{
    const char * nl = strchr(s, '\n');
    if (!nl)
        return NULL;

    return strndup(s, nl - s);
}

int
main()
{
    print_debug_success("libchlsdl-common version: '%s'\n",
        get_libchlsdl_common_version()->version);

    set_cache_dir();
    set_config_dir();

    config = parse_config(g_config_dir);

    set_downloads_dir();
    print_info("using cache dir: '%s'\n", g_cache_dir);
    print_info("using config dir: '%s'\n", g_config_dir);
    print_info("using downloads dir: '%s'\n", g_downloads_dir);

    if (sigaction(SIGINT, &(struct sigaction) { .sa_handler = cleanup }, NULL)
        == -1) {
        print_error("sighandler\n");
        cleanup(1);
    }

    /* initalize modules */
    const struct chlsdl_data cdata = {
        get_libchlsdl_common_version(),
        g_cache_dir,
        g_downloads_dir,
        3000,
    };

    init_modules(&cdata);
    local_socket = socket_open(config->port);
    assert(local_socket != -1);
    print_info("created socket\n");

    js_data = curl_buffer_alloc(KILOBYTES_TO_BYTES(1));
    assert(js_data);

    {
        char * curllog = svconcat("%s/curl_log.txt", g_cache_dir);
        assert(curllog);
        set_curl_logfile_path(curllog);
        free(curllog);
    }

    set_curl_user_agent(config->user_agent);

#ifdef USE_LIBNOTIFY
    assert(chlsdl_notify_init("chlsdl"));

    assert(chlsdl_notify_notification_show_new(
        "chlsdl", "listening...", .timeout = 3000));
#endif

    while (1) {
        print_info("listening...\n");
        if (socket_recv_no_http_header(local_socket, js_data) <= 0) {
            print_warn("read nothing\n");
            goto skip;
        }

        print_debug_success("read: '%s'\n", js_data->data);

        char * url = get_line_from_string(js_data->data);
        if (!url)
            goto skip;

        for (int i = 0; i < nmodules; ++i) {
            const struct module * mod = modules[i];

            if (pcre2_match(mod->regex.pattern, (PCRE2_SPTR8)url,
                    PCRE2_ZERO_TERMINATED, 0, 0, mod->regex.md, NULL)
                >= 0) {
                print_info("downloading: '%s'\n", url);
                if (!mod->func)
                    assert(0);

                mod->func(js_data->data);
                break;
            }
        }

    skip:
        free(url);
        nanosleep(&(const struct timespec) { .tv_nsec = MS_TO_NS(300) }, NULL);
    }

    cleanup(0);

    return 0;
}
