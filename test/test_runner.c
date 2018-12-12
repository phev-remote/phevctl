#define LOGGING_OFF
#define LOGGING_OFF

#include "unity.h"
#include "test_phev_core.c"
#include "test_phev_register.c"
#include "test_phev_pipe.c"

int main()
{
    UNITY_BEGIN();
//  PHEV_CORE

    RUN_TEST(test_create_phev_message);
    RUN_TEST(test_destroy_phev_message);
    RUN_TEST(test_split_message_single_correct_size);
    RUN_TEST(test_split_message_single_correct_command);
    RUN_TEST(test_split_message_single_correct_length);
    RUN_TEST(test_split_message_single_correct_type);
    RUN_TEST(test_split_message_single_correct_reg);
    RUN_TEST(test_split_message_single_correct_data);
    RUN_TEST(test_split_message_double_correct);
    RUN_TEST(test_split_message_double_decode);
    RUN_TEST(test_encode_message_single);
    RUN_TEST(test_encode_message_single_checksum);
    RUN_TEST(test_simple_command_request_message);
    RUN_TEST(test_simple_command_response_message);
    RUN_TEST(test_command_message);
    RUN_TEST(test_ack_message);
    RUN_TEST(test_start_message);
    RUN_TEST(test_start_encoded_message);
    RUN_TEST(test_ping_message);
    RUN_TEST(test_response_handler_start);
    RUN_TEST(test_calc_checksum);
    RUN_TEST(test_phev_message_to_message);
    RUN_TEST(test_phev_ack_message); 
    RUN_TEST(test_phev_head_lights_on);
    RUN_TEST(test_phev_head_lights_on_message);
    RUN_TEST(test_phev_mac_response);
    RUN_TEST(test_phev_message_to_phev_message_and_back);

//  PHEV_REGISTER

    RUN_TEST(test_phev_register_bootstrap);
    RUN_TEST(test_phev_register_getVin);
    RUN_TEST(test_phev_register_should_send_mac_and_aa);
    RUN_TEST(test_phev_register_should_trigger_aa_ack_event);
    RUN_TEST(test_phev_register_should_call_complete_when_registered);
    RUN_TEST(test_phev_register_should_get_start_ack);
    RUN_TEST(test_phev_register_should_get_aa_ack);
    RUN_TEST(test_phev_register_should_get_registration);
    RUN_TEST(test_phev_register_should_get_ecu_version);
    RUN_TEST(test_phev_register_should_get_remote_security_present);
    RUN_TEST(test_phev_register_should_get_reg_disp);
    RUN_TEST(test_phev_register_end_to_end);

//  PHEV PIPE
    
    RUN_TEST(test_phev_pipe_createPipe);
    RUN_TEST(test_phev_pipe_outputChainInputTransformer);
    RUN_TEST(test_phev_pipe_splitter_one_message);
    RUN_TEST(test_phev_pipe_publish);
    RUN_TEST(test_phev_pipe_commandResponder);

    return UNITY_END();
}