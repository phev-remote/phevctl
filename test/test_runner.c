#define LOGGING_OFF

#include "unity.h"
#include "tests.h"


void setUp(void)
{

}

void tearDown(void)
{

}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_phev_args_no_args);
    
    RUN_TEST(test_phev_args_register);
    RUN_TEST(test_phev_args_register_with_operand_should_fail);

    RUN_TEST(test_phev_args_headlights_on);
    RUN_TEST(test_phev_args_headlights_off);
    RUN_TEST(test_phev_args_headlights_no_operand);

    RUN_TEST(test_phev_args_aircon_on);
    RUN_TEST(test_phev_args_aircon_off);
    RUN_TEST(test_phev_args_aircon_no_operand);

    return UNITY_END();
}