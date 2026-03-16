#include "greatest.h"
#include "phevargs.h"

TEST phev_args_no_args(void)
{
    char *args[] = {"./phevctl"};

    ASSERT(phev_args_parse(1, args) != NULL);
    PASS();
}

TEST phev_args_register(void)
{
    char *args[] = {"./phevctl", "register"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_REGISTER, opts->command);
    PASS();
}

TEST phev_args_register_with_operand_should_fail(void)
{
    char *args[] = {"./phevctl", "register", "on"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_INVALID, opts->command);
    PASS();
}

TEST phev_args_headlights_on(void)
{
    char *args[] = {"./phevctl", "headlights", "on"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_HEADLIGHTS, opts->command);
    ASSERT(opts->operand_on);
    PASS();
}

TEST phev_args_headlights_off(void)
{
    char *args[] = {"./phevctl", "headlights", "off"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_HEADLIGHTS, opts->command);
    ASSERT_FALSE(opts->operand_on);
    PASS();
}

TEST phev_args_headlights_no_operand(void)
{
    char *args[] = {"./phevctl", "headlights"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_INVALID, opts->command);
    PASS();
}

TEST phev_args_headlights_invalid_operand(void)
{
    char *args[] = {"./phevctl", "headlights", "bob"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_INVALID, opts->command);
    PASS();
}

TEST phev_args_aircon_on(void)
{
    char *args[] = {"./phevctl", "aircon", "on"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_AIRCON, opts->command);
    ASSERT(opts->operand_on);
    PASS();
}

TEST phev_args_aircon_off(void)
{
    char *args[] = {"./phevctl", "aircon", "off"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_AIRCON, opts->command);
    ASSERT_FALSE(opts->operand_on);
    PASS();
}

TEST phev_args_aircon_no_operand(void)
{
    char *args[] = {"./phevctl", "aircon"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_INVALID, opts->command);
    PASS();
}

TEST phev_args_aircon_invalid_operand(void)
{
    char *args[] = {"./phevctl", "aircon", "bob"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(CMD_INVALID, opts->command);
    PASS();
}

TEST phev_args_verbose_flag_on(void)
{
    char *args[] = {"./phevctl", "-v"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT(opts->verbose);
    PASS();
}

TEST phev_args_verbose_off_by_default(void)
{
    char *args[] = {"./phevctl"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_FALSE(opts->verbose);
    PASS();
}

TEST phev_args_host_default(void)
{
    char *args[] = {"./phevctl"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_STR_EQ("192.168.8.46", opts->host);
    PASS();
}

TEST phev_args_host_as_arg(void)
{
    char *args[] = {"./phevctl", "-h", "1.2.3.4"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_STR_EQ("1.2.3.4", opts->host);
    PASS();
}

TEST phev_args_port_default(void)
{
    char *args[] = {"./phevctl"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(8080, opts->port);
    PASS();
}

TEST phev_args_port_as_arg(void)
{
    char *args[] = {"./phevctl", "-p", "1234"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_EQ(1234, opts->port);
    PASS();
}

TEST phev_args_port_as_arg_invalid(void)
{
    char *args[] = {"./phevctl", "-p", "port"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT(opts->error);
    PASS();
}

TEST phev_args_get(void)
{
    char *args[] = {"./phevctl", "get", "20"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t *opts = phev_args_parse(len, args);
    ASSERT(opts != NULL);
    ASSERT_FALSE(opts->error);
    ASSERT_EQ(CMD_GET_REG_VAL, opts->command);
    ASSERT_EQ(20, opts->reg_operand);
    PASS();
}

SUITE(phev_args)
{
    RUN_TEST(phev_args_no_args);

    RUN_TEST(phev_args_register);
    RUN_TEST(phev_args_register_with_operand_should_fail);

    RUN_TEST(phev_args_headlights_on);
    RUN_TEST(phev_args_headlights_off);
    RUN_TEST(phev_args_headlights_no_operand);
    RUN_TEST(phev_args_headlights_invalid_operand);

    RUN_TEST(phev_args_aircon_on);
    RUN_TEST(phev_args_aircon_off);
    RUN_TEST(phev_args_aircon_no_operand);
    RUN_TEST(phev_args_aircon_invalid_operand);

    RUN_TEST(phev_args_verbose_flag_on);
    RUN_TEST(phev_args_verbose_off_by_default);

    RUN_TEST(phev_args_host_default);
    RUN_TEST(phev_args_host_as_arg);

    RUN_TEST(phev_args_port_default);
    RUN_TEST(phev_args_port_as_arg);
    RUN_TEST(phev_args_port_as_arg_invalid);

    RUN_TEST(phev_args_get);
}

GREATEST_MAIN_DEFS();

int main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(phev_args);
    GREATEST_MAIN_END();
}
