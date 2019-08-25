#ifndef _TESTS_H_
#define _TESTS_H_

void test_phev_args_no_args(void);
void test_phev_args_register(void);
void test_phev_args_register_with_operand_should_fail(void);

void test_phev_args_headlights_on(void);
void test_phev_args_headlights_off(void);
void test_phev_args_headlights_no_operand(void);

void test_phev_args_aircon_on(void);
void test_phev_args_aircon_off(void);
void test_phev_args_aircon_no_operand(void);
#endif