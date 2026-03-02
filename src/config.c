#include "config.h"
#include "main.h"

#include <assert.h>
#include <chlsdl/common/print.h>
#include <chlsdl/common/util/util.h>
#include <chlsdl/macros.h>
#include <json-c/json.h>

static void
generate_new_config(const char * config_file)
{
    print_warn("generating new config at: '%s'\n", config_file);
    const char *  config_str = "{\"user_agent\": null}";
    json_object * config     = json_tokener_parse(config_str);
    assert(config);

    assert(json_object_to_file_ext(config_file, config, JSON_C_TO_STRING_PRETTY)
        != -1);
    json_object_put(config);
}

void
config_destroy(struct chlsdl_config * config)
{
    free((char *)config->user_agent);
    free(config);
}

struct chlsdl_config *
parse_config(const char * config_dir)
{
    char * config_file = svconcat("%s/config.json", config_dir);
    assert(config_file);

    if (!file_exists(config_file))
        generate_new_config(config_file);

    print_debug_warn("parsing config at: '%s'\n", config_file);

    json_object * json_config = json_object_from_file(config_file);
    assert(json_config);

    struct chlsdl_config * config = malloc(sizeof(*config));
    assert(config);

    /* get user agent */
    config->user_agent = json_object_get_string(
        json_object_object_get(json_config, "user_agent"));
    if (config->user_agent) {
        config->user_agent = strdup(config->user_agent);
        assert(config->user_agent);
    }

    print_debug_warn("user_agent: '%s'\n", config->user_agent);

    json_object_put(json_config);
    free(config_file);

    return config;
}
