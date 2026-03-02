#ifndef CONFIG_H_
#define CONFIG_H_

struct chlsdl_config {
    const char * user_agent;
};

extern struct chlsdl_config *
parse_config(const char * config_dir);
extern void
config_destroy(struct chlsdl_config * config);

#endif // CONFIG_H_
