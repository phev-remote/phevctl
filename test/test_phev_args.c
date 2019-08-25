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

// CMD_GET_REG_VAL