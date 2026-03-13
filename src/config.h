#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

struct chlsdl_config {
    const char * user_agent;
    uint16_t     port;
};

extern struct chlsdl_config *
parse_config(const char * config_dir);
extern void
config_destroy(struct chlsdl_config * config);

#endif // CONFIG_H_
