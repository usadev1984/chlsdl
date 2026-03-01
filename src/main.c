#include "main.h"
#include "clipboard.h"
#include "common/util/util.h"
#include "print.h"

#include <assert.h>
#include <chlsdl/common/common.h>
#include <chlsdl/module.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

static int nlibs;

static struct module_lib {
    const char * path;
    const char * name;
    void *       dl_lib;
} libs[128];

const char * g_cache_dir     = NULL;
const char * g_config_dir    = NULL;
const char * g_downloads_dir = NULL;

static void
cleanup(int sig)
{
    clipboard_deinit();
    for (int i = nlibs; i-- > 0;)
        dlclose(libs[i].dl_lib);
    free((char *)g_downloads_dir);
    free((char *)g_config_dir);
    free((char *)g_cache_dir);
    exit(sig);
}

static void
set_cache_dir()
{
    char * cache = getenv("XDG_CACHE_HOME");
    if (!cache) {
        print_warn("XDG_CACHE_HOME is unset. using '$HOME/.cache/" PROGRAM_NAME
                   "' instead\n");

        char * home = getenv("HOME");
        cache       = svconcat("%s/.cache/" PROGRAM_NAME, home);
        assert(cache);
    } else {
        cache = svconcat("%s/" PROGRAM_NAME, cache);
        assert(cache);
    }

    g_cache_dir = cache;

    if (mkdir(g_cache_dir, S_IRWXU | S_IRGRP) == -1 && errno != EEXIST) {
        print_error("failed to create directory: '%s'\n", g_cache_dir);
        cleanup(1);
    }
}

static void
set_config_dir()
{
    char * config = getenv("XDG_CONFIG_HOME");
    if (!config) {
        print_warn(
            "XDG_CONFIG_HOME is unset. using '$HOME/.config/" PROGRAM_NAME
            "' instead\n");

        char * home = getenv("HOME");
        config      = svconcat("%s/.config/" PROGRAM_NAME, home);
        assert(config);
    } else {
        config = svconcat("%s/" PROGRAM_NAME, config);
        assert(config);
    }

    g_config_dir = config;

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
     * FIXME: search `modules` placed in appropriate system directories like
     * /lib and /usr/lib and wherever they are in nixos
     */
    static const char * dir     = "modules";
    DIR *               modules = opendir(dir);
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

    for (int i = 0; i < nlibs; ++i) {
        struct module_lib * module = &libs[i];
        print_debug_warn("loading '%s'\n", module->name);
        module->dl_lib = dlopen(module->path, RTLD_NOW | RTLD_GLOBAL);

        if (!module->dl_lib) {
            print_debug_error("'%s'\n", dlerror());
            assert(0);
        }
    }
}

int
main()
{
    print_debug_success("libchlsdl-common version: '%s'\n",
        get_libchlsdl_common_version()->version);

    set_cache_dir();
    set_config_dir();
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
    };

    init_modules(&cdata);
    clipboard_init();

    while (1) {
        char * clip_content = (char *)clipboard_get();
        assert(clip_content);

        print_debug_success("read: '%s'\n", clip_content);

        clipboard_string_free(clip_content);
        nanosleep(&(const struct timespec) { .tv_nsec = MS_TO_NS(300) }, NULL);
    }

    cleanup(0);

    return 0;
}
