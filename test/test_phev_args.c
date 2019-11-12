#include "unity.h"
#include "phevargs.h"

void test_phev_args_no_args(void)
{
    char * args[] = {"./phevctl"};
    
    TEST_ASSERT_NOT_NULL(phev_args_parse(1,args));
    
}
void test_phev_args_register(void)
{
    char * args[] = {"./phevctl","register"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_REGISTER,opts->command);
    
}
void test_phev_args_register_with_operand_should_fail(void)
{
    char * args[] = {"./phevctl","register","on"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_INVALID,opts->command);
    
}

void test_phev_args_headlights_on(void)
{
    char * args[] = {"./phevctl","headlights","on"};
    
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_HEADLIGHTS,opts->command);
    TEST_ASSERT_TRUE(opts->operand_on);
    
}
void test_phev_args_headlights_off(void)
{
    char * args[] = {"./phevctl","headlights","off"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);

    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_HEADLIGHTS,opts->command);
    TEST_ASSERT_FALSE(opts->operand_on);
    
}
void test_phev_args_headlights_no_operand(void)
{
    char * args[] = {"./phevctl","headlights"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_INVALID,opts->command);
    
}
void test_phev_args_headlights_invalid_operand(void)
{
    char * args[] = {"./phevctl","headlights","bob"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_INVALID,opts->command);
    
}
void test_phev_args_aircon_on(void)
{
    char * args[] = {"./phevctl","aircon","on"};
    
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_AIRCON,opts->command);
    TEST_ASSERT_TRUE(opts->operand_on);
    
}
void test_phev_args_aircon_off(void)
{
    char * args[] = {"./phevctl","aircon","off"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);

    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_AIRCON,opts->command);
    TEST_ASSERT_FALSE(opts->operand_on);
    
}
void test_phev_args_aircon_no_operand(void)
{
    char * args[] = {"./phevctl","aircon"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_INVALID,opts->command);
    
}
void test_phev_args_aircon_invalid_operand(void)
{
    char * args[] = {"./phevctl","aircon","bob"};
    int len = sizeof(args) / sizeof(char *);

    phev_args_opts_t * opts = phev_args_parse(len,args);
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(CMD_INVALID,opts->command);
    
}
void test_phev_args_verbose_flag_on(void)
{
    char * args[] = {"./phevctl","-v"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_TRUE(opts->verbose);
    
}
void test_phev_args_verbose_off_by_default(void)
{
    char * args[] = {"./phevctl"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_FALSE(opts->verbose);
    
}

void test_phev_args_host_default(void)
{
    char * args[] = {"./phevctl"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL_STRING("192.168.8.46",opts->host);
    
}
void test_phev_args_host_as_arg(void)
{
    char * args[] = {"./phevctl","-h","1.2.3.4"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL_STRING("1.2.3.4",opts->host);
    
}

void test_phev_args_port_default(void)
{
    char * args[] = {"./phevctl"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(8080,opts->port);
    
}
void test_phev_args_port_as_arg(void)
{
    char * args[] = {"./phevctl","-p","1234"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_EQUAL(1234,opts->port);
    
}
void test_phev_args_port_as_arg_invalid(void)
{
    char * args[] = {"./phevctl","-p","port"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_TRUE(opts->error);
    
}
void test_phev_args_get(void)
{
    char * args[] = {"./phevctl","get","20"};
    int len = sizeof(args) / sizeof(char *);
    
    phev_args_opts_t * opts = phev_args_parse(len,args);
    
    TEST_ASSERT_NOT_NULL(opts);
    TEST_ASSERT_FALSE(opts->error);
    TEST_ASSERT_EQUAL(CMD_GET_REG_VAL,opts->command);
    TEST_ASSERT_EQUAL(20,opts->reg_operand);
}
// CMD_GET_REG_VAL