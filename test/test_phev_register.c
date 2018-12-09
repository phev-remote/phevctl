#include <string.h>
#include "unity.h"
#include "phev_register.h"
#include "msg_core.h"
#include "msg_pipe.h"
#include "msg_utils.h"
#include "phev_pipe.h"

static message_t * test_phev_register_messages[10];
static message_t * test_phev_register_inHandlerSend = NULL;
static int test_phev_register_index = 0;
static int test_register_aa_ack = 0;

uint8_t startMsg[] = { 0x6f,0x17,0x00,0x15,0x00,0x4a,0x4d,0x41,0x58,0x44,0x47,0x47,0x32,0x57,0x47,0x5a,0x30,0x30,0x32,0x30,0x33,0x35,0x01,0x01,0xf3 };

void test_phev_register_outHandlerIn(messagingClient_t *client, message_t *message) 
{
    //printf("Out in handler got message %s\n",message->data);
    hexdump("TEST-out-in",message->data,message->length,0);
    test_phev_register_messages[test_phev_register_index++] = msg_utils_copyMsg(message);
    return;
}

message_t * test_phev_register_inHandlerIn(messagingClient_t *client) 
{
    //message_t * message = malloc(sizeof(message_t));
    //inTimes ++;
    //printf("In handler got message %s\n",mess);
    
    return NULL;
}
void test_phev_register_outHandlerOut(messagingClient_t *client, message_t *message) 
{
    //printf("Out out handler got message %s\n",message->data);
    hexdump("TEST-out-out",message->data,message->length,0);
    
    test_phev_register_messages[test_phev_register_index++] = msg_utils_copyMsg(message);
    return;
}

message_t * test_phev_register_inHandlerOut(messagingClient_t *client) 
{
    message_t * message = NULL;
    if(test_phev_register_inHandlerSend != NULL)
    {
        message = test_phev_register_inHandlerSend;    
    }
    
    return message;
}
message_t * mock_outputInputTransformer(void *ctx, message_t *message)
{
    printf("Got message %s",message->data);
    TEST_FAIL_MESSAGE("FAILED");
    return NULL;
}

phev_pipe_ctx_t * test_phev_register_create_pipe_helper(void)
{
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
        .ctx = NULL,
        .in = in,
        .out = out,
        .inputSplitter = NULL,
        .outputSplitter = NULL,
        .inputResponder = NULL,
        .outputResponder = (msg_pipe_responder_t) phev_pipe_command_responder,
        .outputOutputTransformer = (msg_pipe_transformer_t) phev_pipe_outputEventTransformer,
        .preConnectHook = NULL,
        .outputInputTransformer = (msg_pipe_transformer_t) phev_pipe_outputChainInputTransformer,
    };

    return phev_pipe_createPipe(settings);
}

void test_phev_register_bootstrap(void)
{
    phev_pipe_ctx_t * pipe = test_phev_register_create_pipe_helper();
    
    phevRegisterSettings_t settings = {
        .pipe = pipe,
    };
    
    phevRegisterCtx_t * ctx = phev_register_init(settings);

    TEST_ASSERT_NOT_NULL(ctx);
}

static char * vin = NULL;

int test_phev_register_event_handler(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event)
{
    switch(event->event) 
    {
        case PHEV_PIPE_GOT_VIN: {
            vin = strdup(event->data);
            //printf("Got vin %s\n",vin);
            break;
        }
        case PHEV_PIPE_AA_ACK: {
            test_register_aa_ack ++;
            break;
        }
        default : {
            printf("Unknown event %d\n",event->event);
        }
    }
}

void test_phev_register_getVin(void)
{
    phev_pipe_ctx_t * pipe = test_phev_register_create_pipe_helper();
    
    phevRegisterSettings_t settings = {
        .pipe = pipe,
        .eventHandler = (phevPipeEventHandler_t) test_phev_register_event_handler,
    };
    
    phevRegisterCtx_t * ctx = phev_register_init(settings);

    test_phev_register_inHandlerSend = msg_utils_createMsg(startMsg,sizeof(startMsg));

    msg_pipe_loop(ctx->pipe->pipe);

    TEST_ASSERT_EQUAL_STRING("JMAXDGG2WGZ002035",vin);
    test_phev_register_inHandlerSend = NULL;
}
void test_phev_register_should_send_mac_and_aa(void)
{
    test_phev_register_index = 0;
    const uint8_t expected[] = {0xf2,0x0a,0x00,0x01,0x2f,0x0d,0xc2,0xc2,0x91,0x85,0x00,0xd3,0xf6,0x04,0x00,0xaa,0x00,0xa4};
    const uint8_t mac[] = {0x2f,0x0d,0xc2,0xc2,0x91,0x85};
    phev_pipe_ctx_t * pipe = test_phev_register_create_pipe_helper();
    
    phevRegisterSettings_t settings = {
        .pipe = pipe,
        .eventHandler = (phevPipeEventHandler_t) phev_register_eventHandler,
    };
    
    memcpy(settings.mac,mac,MAC_ADDR_SIZE);

    phevRegisterCtx_t * ctx = phev_register_init(settings);

    phevPipeEvent_t event = {
        .event = PHEV_PIPE_GOT_VIN,
        .data = "JMAXDGG2WGZ002035",
        .length = 17,
    };
    
    phev_register_eventHandler(pipe ,&event);

    TEST_ASSERT_NOT_NULL(test_phev_register_messages[0]);
    TEST_ASSERT_EQUAL_MEMORY(expected,test_phev_register_messages[0]->data,sizeof(expected));
    
} 
void test_phev_register_should_trigger_aa_ack_event(void)
{
    uint8_t aaAck[] = {0x6f,0x04,0x01,0xaa,0x00,0x1e};

    test_phev_register_inHandlerSend = msg_utils_createMsg(aaAck,sizeof(aaAck));

    phev_pipe_ctx_t * pipe = test_phev_register_create_pipe_helper();
    
    phevRegisterSettings_t settings = {
        .pipe = pipe,
        .eventHandler = (phevPipeEventHandler_t) test_phev_register_event_handler,
    };

    phevRegisterCtx_t * ctx = phev_register_init(settings);

    msg_pipe_loop(pipe->pipe);

    TEST_ASSERT_EQUAL(1,test_register_aa_ack);
    
} 
