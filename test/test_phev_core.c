#include "unity.h"
#include "phev_core.h"
#include "msg_utils.h"

const uint8_t singleMessage[] = {0x6f, 0x0a, 0x00, 0x12, 0x10, 0x06, 0x06, 0x13, 0x05, 0x13, 0x01, 0xd3};
const uint8_t doubleMessage[] = {0x6f, 0x0a, 0x00, 0x12, 0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xff, 0x6f, 0x0a, 0x00, 0x13, 0x11, 0x05, 0x16, 0x15, 0x03, 0x0d, 0x01, 0xff};

void test_create_phev_message(void)
{
    uint8_t data[] = {0,1,2,3,4,5};
    phevMessage_t * message = phev_core_createMessage(0x6f,REQUEST_TYPE,0x12,data, sizeof(data));

    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL(0x6f, message->command);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, message->type);
    TEST_ASSERT_EQUAL(0x12, message->reg);
    TEST_ASSERT_EQUAL(sizeof(data), message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, message->data, sizeof(data));
} 

void test_destroy_phev_message(void)
{
    uint8_t data[] = {0,1,2,3,4,5};
    phevMessage_t * message = phev_core_createMessage(0x6f,REQUEST_TYPE,0x12,data, sizeof(data));

    TEST_ASSERT_NOT_NULL(message);
    phev_core_destroyMessage(message);
    
} 

void test_split_message_single_correct_size(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(12, ret);
}

void test_split_message_single_correct_command(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(0x6f, msg.command);
} 
void test_split_message_single_correct_length(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(0x0a, msg.length);
} 
void test_split_message_single_correct_type(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg.type);
} 
void test_split_message_single_correct_reg(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL(0x12, msg.reg);
} 
void test_split_message_single_correct_data(void)
{
    phevMessage_t msg;
    uint8_t data[] = {0x10, 0x06, 0x06, 0x13, 0x05, 0x13};

    int ret = phev_core_decodeMessage(singleMessage, sizeof(singleMessage), &msg);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, msg.data, 6);
} 
void test_split_message_double_correct(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(doubleMessage, sizeof(doubleMessage), &msg);

    ret = phev_core_decodeMessage(doubleMessage + ret, sizeof(singleMessage) - ret, &msg);

    TEST_ASSERT_EQUAL(0x12, msg.reg);
} 
void test_split_message_double_decode(void)
{
    phevMessage_t msg;

    int ret = phev_core_decodeMessage(doubleMessage, sizeof(doubleMessage), &msg);

    ret = phev_core_decodeMessage(doubleMessage + ret, sizeof(doubleMessage) - ret, &msg);

    TEST_ASSERT_EQUAL(0x13, msg.reg);
} 
void test_encode_message_single(void)
{
    uint8_t data[] = {0x10, 0x06, 0x06, 0x13, 0x05, 0x13,0x01};
    phevMessage_t * msg = phev_core_createMessage(0x6f,REQUEST_TYPE,0x12,data, sizeof(data));

    uint8_t * out;
    int num = phev_core_encodeMessage(msg, &out);
    TEST_ASSERT_EQUAL(12, num);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(singleMessage, out, num);
} 
void test_encode_message_single_checksum(void)
{
    uint8_t data[] = {0x00};

    phevMessage_t  * msg = phev_core_createMessage(0x6f, RESPONSE_TYPE, 0xaa, data, 1);
    
    uint8_t * out;
    int num = phev_core_encodeMessage(msg, &out);
    TEST_ASSERT_EQUAL(0x1e, out[5]);
} 
void test_simple_command_request_message(void)
{
    phevMessage_t *msg = phev_core_simpleRequestCommandMessage(0x01, 0xff);

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x1, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x1, msg->reg);
    TEST_ASSERT_EQUAL(0xff, msg->data[0]);
} 
void test_simple_command_response_message(void)
{
    phevMessage_t *msg = phev_core_simpleResponseCommandMessage(0x01, 0xff);

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x1, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x1, msg->reg);
    TEST_ASSERT_EQUAL(0xff, msg->data[0]);
} 
void test_command_message(void)
{
    uint8_t data[] = {0, 1, 2, 3, 4, 5};

    phevMessage_t *msg = phev_core_commandMessage(0x10, data, sizeof(data));

    TEST_ASSERT_EQUAL(0xf6, msg->command);
    TEST_ASSERT_EQUAL(0x6, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x10, msg->reg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data, msg->data, sizeof(data));
} 
void test_ack_message(void)
{
    const uint8_t reg = 0x10;
    const phevMessage_t *msg = phev_core_ackMessage(0x6f, reg);

    TEST_ASSERT_EQUAL(0x6f, msg->command);
    TEST_ASSERT_EQUAL(0x1, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x10, msg->reg);
    TEST_ASSERT_EQUAL(0x00, *msg->data);
} 
void test_start_message(void)
{
    uint8_t mac[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    const uint8_t expected[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,0x00};
    
    phevMessage_t *msg = phev_core_startMessage(mac);

    TEST_ASSERT_NOT_NULL(msg);
    TEST_ASSERT_EQUAL(START_SEND, msg->command);
    TEST_ASSERT_EQUAL(0x07, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x01, msg->reg);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, msg->data, sizeof(expected));
} 
void test_start_encoded_message(void)
{
    uint8_t mac[] = {0,0,0,0,0,0};
    uint8_t expected[] = {0xf2, 0x0a, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfd, 
        0xf6, 0x04, 0x00, 0xaa, 0x00, 0xa4};

    message_t *message = phev_core_startMessageEncoded(mac);

    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL(18, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, message->data, message->length);
} 
void test_ping_message(void)
{
    const uint8_t num = 1;

    phevMessage_t * msg = phev_core_pingMessage(num);

    TEST_ASSERT_EQUAL(PING_SEND_CMD, msg->command);
    TEST_ASSERT_EQUAL(0x01, msg->length);
    TEST_ASSERT_EQUAL(REQUEST_TYPE, msg->type);
    TEST_ASSERT_EQUAL(num, msg->reg);
    TEST_ASSERT_EQUAL(0, msg->data[0]);
}
void test_response_handler_start(void)
{
    uint8_t value = 0;
    phevMessage_t request = {
        .command = RESP_CMD,
        .length = 4,
        .type = REQUEST_TYPE,
        .reg = 0x29,
        .data = &value,  
    };
    phevMessage_t * msg = phev_core_responseHandler(&request);

    TEST_ASSERT_EQUAL(SEND_CMD, msg->command);
    TEST_ASSERT_EQUAL(0x01, msg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, msg->type);
    TEST_ASSERT_EQUAL(0x29, msg->reg);
    TEST_ASSERT_EQUAL(0, *msg->data);   
} 
void test_calc_checksum(void)
{
    const uint8_t data[] = {0x2f,0x04,0x00,0x01,0x01,0x00};
    uint8_t checksum = phev_core_checksum(data);
    TEST_ASSERT_EQUAL(0x35,checksum);
} 
void test_phev_message_to_message(void)
{
    phevMessage_t * phevMsg = phev_core_simpleRequestCommandMessage(0xaa, 0x00);
    int8_t expected[] = {0xf6,0x04,0x00,0xaa,0x00,0xa4};
    
    message_t * message = phev_core_convertToMessage(phevMsg);

    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL(6, message->length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, message->data,6); 
}

void test_phev_ack_message(void)
{
    const uint8_t expected[] = {0xf6, 0x04, 0x01, 0xc0, 0x00, 0xbb};
    const phevMessage_t * phevMsg = phev_core_ackMessage(0xf6,0xc0);
    
    TEST_ASSERT_NOT_NULL(phevMsg);
    TEST_ASSERT_EQUAL(0xf6, phevMsg->command);
    TEST_ASSERT_EQUAL(0x01, phevMsg->length);
    TEST_ASSERT_EQUAL(RESPONSE_TYPE, phevMsg->type);
    TEST_ASSERT_EQUAL(0, phevMsg->data[0]);
} 
void test_phev_head_lights_on(void)
{
    const uint8_t expected[] = {0xf6, 0x04, 0x00, 0x0a, 0x01, 0x05};
    
    phevMessage_t * headLightsOn = phev_core_simpleRequestCommandMessage(0x0a, 1);
    message_t * message = phev_core_convertToMessage(headLightsOn);
    
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, message->data,6); 
    
}
void test_phev_head_lights_on_message(void)
{
    const uint8_t expected[] = {0xf6, 0x04, 0x00, 0x0a, 0x01, 0xbb};
    
    phevMessage_t * headLightsOn = phev_core_simpleRequestCommandMessage(0x0a, 1);
    
    TEST_ASSERT_EQUAL(1, headLightsOn->data[0]);
}