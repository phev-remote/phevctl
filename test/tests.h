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

void test_phev_args_verbose_flag_on(void);
void test_phev_args_verbose_off_by_default(void);

void test_phev_args_host_default(void);
void test_phev_args_host_as_arg(void);

void test_phev_args_port_default(void);
void test_phev_args_port_as_arg(void);
void test_phev_args_port_as_arg_invalid(void);

void test_phev_args_get(void);
#endif