#include "main.h"
#include "clipboard.h"
#include "common/util/util.h"
#include "print.h"

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>

const char * g_cache_dir     = NULL;
const char * g_config_dir    = NULL;
const char * g_downloads_dir = NULL;

static void
cleanup(int sig)
{
    clipboard_deinit();
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

int
main()
{
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
