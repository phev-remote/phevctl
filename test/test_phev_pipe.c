#include <string.h>
#include "unity.h"
#include "msg_core.h"
#include "msg_pipe.h"
#include "msg_utils.h"
#include "phev_pipe.h"

void test_phev_pipe_outHandlerIn(messagingClient_t *client, message_t *message) 
{
    return;
}

message_t * test_phev_pipe_inHandlerIn(messagingClient_t *client) 
{
    return NULL;
}
message_t * global_message = NULL;
message_t * global_in_message = NULL;

void test_phev_pipe_outHandlerOut(messagingClient_t *client, message_t *message) 
{
    global_message = msg_utils_copyMsg(message);
    return;
}

message_t * test_phev_pipe_inHandlerOut(messagingClient_t *client) 
{
    message_t * message = msg_utils_copyMsg(global_in_message);
    
    return message;
}
void test_phev_pipe_createPipe(void)
{
    global_message = NULL;
    messagingSettings_t inSettings = {
        .incomingHandler = test_phev_register_inHandlerIn,
        .outgoingHandler = test_phev_register_outHandlerIn,
    };
    messagingSettings_t outSettings = {
        .incomingHandler = test_phev_register_inHandlerOut,
        .outgoingHandler = test_phev_register_outHandlerOut,
    };
    
    messagingClient_t * in = msg_core_createMessagingClient(inSettings);
    messagingClient_t * out = msg_core_createMessagingClient(outSettings);

    phev_pipe_settings_t settings = {
        .in = in,
        .out = out,
        .inputSplitter = NULL,
        .outputSplitter = NULL,
        .inputResponder = NULL,
        .outputResponder = (msg_pipe_responder_t) phev_pipe_commandResponder,
        .outputOutputTransformer = (msg_pipe_transformer_t) phev_pipe_outputEventTransformer,
        .preConnectHook = NULL,
        .outputInputTransformer = (msg_pipe_transformer_t) phev_pipe_outputChainInputTransformer,
    };

    phev_pipe_ctx_t * ctx =  phev_pipe_createPipe(settings);
    
    TEST_ASSERT_NOT_NULL(ctx);

}

void test_phev_pipe_publish(void)
{
    messagingSettings_t inSettings = {
        .incomingHandler = test_phev_pipe_inHandlerIn,
        .outgoingHandler = test_phev_pipe_outHandlerIn,
    };
    messagingSettings_t outSettings = {
        .incomingHandler = test_phev_pipe_inHandlerOut,
        .outgoingHandler = test_phev_pipe_outHandlerOut,
    };
    
    messagingClient_t * in = msg_core_createMessagingClient(inSettings);
    messagingClient_t * out = msg_core_createMessagingClient(outSettings);

    phev_pipe_settings_t settings = {
        .in = in,
        .out = out,
        .inputSplitter = NULL,
        .outputSplitter = NULL,
        .inputResponder = NULL,
        .outputResponder = (msg_pipe_responder_t) phev_pipe_commandResponder,
        .outputOutputTransformer = (msg_pipe_transformer_t) phev_pipe_outputEventTransformer,
    
        .preConnectHook = NULL,
        .outputInputTransformer = (msg_pipe_transformer_t) phev_pipe_outputChainInputTransformer,
    
    };

    phev_pipe_ctx_t * ctx =  phev_pipe_createPipe(settings);

    message_t * message = msg_utils_createMsg(startMsg,sizeof(startMsg));

    msg_pipe_outboundPublish(ctx->pipe,  message);

    TEST_ASSERT_NOT_NULL(global_message);
    TEST_ASSERT_EQUAL_MEMORY(startMsg,global_message->data,sizeof(startMsg));
}
void test_phev_pipe_commandResponder(void)
{
    const uint8_t reg[] = {0x6f,0x17,0x00,0x15,0x00,0x4a,0x4d,0x41,0x58,0x44,0x47,0x47,0x32,0x57,0x47,0x5a,0x30,0x30,0x32,0x30,0x33,0x35,0x01,0x01,0xf3};
    const uint8_t expected[] = {0xf6,0x04,0x01,0x15,0x00,0x10};
    global_in_message = msg_utils_createMsg(reg,sizeof(reg));
    messagingSettings_t inSettings = {
        .incomingHandler = test_phev_pipe_inHandlerIn,
        .outgoingHandler = test_phev_pipe_outHandlerIn,
    };
    messagingSettings_t outSettings = {
        .incomingHandler = test_phev_pipe_inHandlerOut,
        .outgoingHandler = test_phev_pipe_outHandlerOut,
    };
    
    messagingClient_t * in = msg_core_createMessagingClient(inSettings);
    messagingClient_t * out = msg_core_createMessagingClient(outSettings);

    phev_pipe_settings_t settings = {
        .in = in,
        .out = out,
        .inputSplitter = NULL,
        .outputSplitter = NULL,
        .inputResponder = NULL,
        .outputResponder = (msg_pipe_responder_t) phev_pipe_commandResponder,
        .outputOutputTransformer = (msg_pipe_transformer_t) phev_pipe_outputEventTransformer,
    
        .preConnectHook = NULL,
        .outputInputTransformer = (msg_pipe_transformer_t) phev_pipe_outputChainInputTransformer,
    
    };

    phev_pipe_ctx_t * ctx =  phev_pipe_createPipe(settings);

    msg_pipe_loop(ctx->pipe);
    TEST_ASSERT_NOT_NULL(global_message);
    TEST_ASSERT_EQUAL_MEMORY(expected,global_message->data,sizeof(expected));
}
void test_phev_pipe_commandResponder_should_only_respond_to_commands(void)
{
    const uint8_t reg[] = {0x9f,0x04,0x01,0x10,0x06,0xba};
    global_message = NULL;
    global_in_message = msg_utils_createMsg(reg,sizeof(reg));
    messagingSettings_t inSettings = {
        .incomingHandler = test_phev_pipe_inHandlerIn,
        .outgoingHandler = test_phev_pipe_outHandlerIn,
    };
    messagingSettings_t outSettings = {
        .incomingHandler = test_phev_pipe_inHandlerOut,
        .outgoingHandler = test_phev_pipe_outHandlerOut,
    };
    
    messagingClient_t * in = msg_core_createMessagingClient(inSettings);
    messagingClient_t * out = msg_core_createMessagingClient(outSettings);

    phev_pipe_settings_t settings = {
        .in = in,
        .out = out,
        .inputSplitter = NULL,
        .outputSplitter = NULL,
        .inputResponder = NULL,
        .outputResponder = (msg_pipe_responder_t) phev_pipe_commandResponder,
        .outputOutputTransformer = (msg_pipe_transformer_t) phev_pipe_outputEventTransformer,
    
        .preConnectHook = NULL,
        .outputInputTransformer = (msg_pipe_transformer_t) phev_pipe_outputChainInputTransformer,
    
    };

    phev_pipe_ctx_t * ctx =  phev_pipe_createPipe(settings);

    msg_pipe_loop(ctx->pipe);
    TEST_ASSERT_NULL(global_message);
}

void test_phev_pipe_outputChainInputTransformer(void)
{
    // TODO
}
void test_phev_pipe_splitter_one_message(void)
{
    uint8_t msg_data[] = {0x6f,0x04,0x01,0x02,0x00,0xff};
    
    message_t * message = malloc(sizeof(message_t));

    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    messageBundle_t * messages = phev_pipe_outputSplitter(NULL, message);

    TEST_ASSERT_EQUAL(1, messages->numMessages);
} 
void test_phev_pipe_splitter_two_messages(void)
{
    uint8_t msg_data[] = {0x6f,0x04,0x01,0x01,0x00,0xff,0x6f,0x04,0x01,0x02,0x00,0xff};
    const uint8_t msg1_data[] = {0x6f,0x04,0x01,0x01,0x00,0xff};
    const uint8_t msg2_data[] = {0x6f,0x04,0x01,0x02,0x00,0xff};
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
     
    messageBundle_t * messages = phev_pipe_outputSplitter(NULL, message);

    TEST_ASSERT_EQUAL(2, messages->numMessages);
    TEST_ASSERT_EQUAL(6, messages->messages[0]->length);
    TEST_ASSERT_EQUAL(6, messages->messages[1]->length);
    
    TEST_ASSERT_EQUAL_MEMORY(msg1_data, messages->messages[0]->data, 6);
    TEST_ASSERT_EQUAL_MEMORY(msg2_data, messages->messages[1]->data, 6);

}
