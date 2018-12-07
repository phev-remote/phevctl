#include "unity.h"
#include "msg_core.h"
#include "msg_tcpip.h"
#include "mock_msg_pipe.h"
#include "phev_controller.h"
#include "mock_phev_core.h"
#include "mock_msg_utils.h"
#include "mock_phev_config.h"
#include "mock_phev_response_handler.h"
#include "mock_ota.h"
#include "mock_logger.h"
#include "mock_msg_mqtt.h"
#include "mock_phev_store.h"

#include <cjson/cJSON.h>

void setUp(void)
{
    hexdump_Ignore();
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(false);
    phev_config_checkForParkLightsOn_IgnoreAndReturn(false);
    phev_config_checkForAirConOn_IgnoreAndReturn(false);
    phev_config_checkForHeadLightsOff_IgnoreAndReturn(true);
    phev_config_checkForParkLightsOff_IgnoreAndReturn(true);
    phev_config_checkForAirConOff_IgnoreAndReturn(true);
    msg_mqtt_publish_IgnoreAndReturn(0);
    phev_store_compare_IgnoreAndReturn(0);
    phev_store_add_IgnoreAndReturn(0);
    phev_store_create_IgnoreAndReturn(NULL);
    
    //msg_utils_createMsg_IgnoreAndReturn(NULL);
    
}

void test_handle_event(void)
{
    phevEvent_t event = {
        .type = CONNECT_REQUEST,
    };

    TEST_ASSERT_EQUAL(PHEV_OK, phev_controller_handleEvent(&event));
}

void test_phev_controller_init(void)
{
    messagingClient_t inClient;
    messagingClient_t outClient;

    msg_pipe_ctx_t pipe;

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .in     = &inClient,
        .out    = &outClient,
    };

    phevCtx_t * ctx = phev_controller_init(&settings);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL(&pipe, ctx->pipe);
    TEST_ASSERT_EQUAL(0, ctx->queueSize);
} 

void test_phev_controller_init_set_phev_ctx(void)
{
    messagingClient_t inClient;
    messagingClient_t outClient;

    void * dummyCtx;

    msg_pipe_ctx_t pipe = {
        .user_context = dummyCtx,
    };

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .in     = &inClient,
        .out    = &outClient,
    };

    phevCtx_t * ctx = phev_controller_init(&settings);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL(dummyCtx, ctx->pipe->user_context);
} 
static int fake_publish_called = 0;
void fake_publish(void * ctx, message_t * message)
{
    
    uint8_t data[] = {1,2,3};

    TEST_ASSERT_EQUAL(3, message->length);
    TEST_ASSERT_EQUAL_MEMORY(data,message->data,3);
    fake_publish_called ++;
}
void test_phev_controller_initConfig(void)
{
    phevConfig_t config = {
        .updateConfig = {
            .currentBuild = 0,
        },
    };
    phev_controller_initConfig(&config);
    TEST_ASSERT_EQUAL(1,config.updateConfig.currentBuild);
}
void test_phev_controller_initState(void)
{
    phevState_t state;

    phev_controller_initState(&state);

    TEST_ASSERT_EQUAL(0, state.connectedClients);
} /*
void test_phev_controller_config_splitter_connected(void)
{
    const char * msg_data = "{ \"state\": { \"connectedClients\": 1 } }";
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    message_t * start = malloc(sizeof(message_t));
    
    start->data = "START";
    start->length = sizeof("START");
    
    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        
    };

    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));
    
    phev_config_checkForFirmwareUpdate_IgnoreAndReturn(false);
    phev_config_parseConfig_IgnoreAndReturn(&config);
    phev_config_checkForConnection_IgnoreAndReturn(true);
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(false);
    phev_core_startMessageEncoded_IgnoreAndReturn(start);
    phev_core_simpleRequestCommandMessage_IgnoreAndReturn(NULL);
    phev_core_convertToMessage_IgnoreAndReturn(NULL);
    phev_core_destroyMessage_Ignore();
    messageBundle_t * out = phev_controller_configSplitter(ctx, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(4,out->numMessages);
    
    TEST_ASSERT_EQUAL_STRING(start->data,out->messages[0]->data);
}   
void test_phev_controller_config_splitter_not_connected(void)
{
    const char * msg_data = "{ \"state\": { \"connectedClients\": 0 } }";
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    message_t * start = malloc(sizeof(message_t));
    
    start->data = "START";
    start->length = sizeof("START");
    
    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        
    };
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));
    
    phev_config_checkForFirmwareUpdate_IgnoreAndReturn(false);
    phev_config_parseConfig_IgnoreAndReturn(&config);
    phev_config_checkForConnection_IgnoreAndReturn(false);
    phev_core_startMessageEncoded_IgnoreAndReturn(start);
    
    phev_core_simpleRequestCommandMessage_IgnoreAndReturn(NULL);
    phev_core_convertToMessage_IgnoreAndReturn(NULL);
    phev_core_destroyMessage_Ignore();
    
    message_t * phev_controller_turnHeadLightsOn(void);
    messageBundle_t * out = phev_controller_configSplitter(ctx, message);
    
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(3,out->numMessages);
} 
void test_phev_controller_config_splitter_headLightsOn(void)
{
    const char * msg_data = "";
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    const uint8_t lightsOn[] = {0xf6,0x04,0x00,0x0a,0x02,0xff};
    
    message_t * outMsg = msg_utils_createMsg(&lightsOn,6);
    
    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        
    };
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));
    //msg_pipe_ctx_t * pipe = malloc(sizeof(msg_pipe_ctx_t));
    //pipe->in = 
    //gcp_ctx_t * gcp = malloc(sizeof(gcp_ctx_t));

    //gcp->mqtt = NULL;
    //pipe->ctx
    //ctx->pipe = pipe;

    phev_config_checkForFirmwareUpdate_IgnoreAndReturn(false);
    phev_config_parseConfig_IgnoreAndReturn(&config);
    phev_config_checkForConnection_IgnoreAndReturn(false);
    phev_core_startMessageEncoded_IgnoreAndReturn(NULL);
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(true);
    phev_core_simpleRequestCommandMessage_IgnoreAndReturn(NULL);
    phev_core_convertToMessage_IgnoreAndReturn(&outMsg);
    phev_core_destroyMessage_Ignore();
    //msg_utils_createMsg_IgnoreAndReturn(NULL);
    
    //messageBundle_t * out = phev_controller_configSplitter(ctx, message);
    
    //TEST_ASSERT_NOT_NULL(out);
    //TEST_ASSERT_EQUAL(1,out->numMessages);
    //TEST_ASSERT_NOT_NULL(out->messages[0]);
    //TEST_ASSERT_EQUAL(6,out->messages[0]->length);
    
    //TEST_ASSERT_NOT_NULL(out->messages[0]->data);
    
    //TEST_ASSERT_EQUAL_MEMORY(lightsOn, out->messages[0]->data,6);
} 
void test_phev_controller_splitter_one_message(void)
{
    const uint8_t msg_data[] = {0x6f,0x04,0x01,0x02,0x00,0xff};
    
    message_t * message = malloc(sizeof(message_t));
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));

    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    phev_core_extractMessage_IgnoreAndReturn(message);

    messageBundle_t * messages = phev_controller_splitter(ctx, message);

    TEST_ASSERT_EQUAL(1, messages->numMessages);
} 
void test_phev_controller_splitter_two_messages(void)
{
    const uint8_t msg_data[] = {0x6f,0x04,0x01,0x01,0x00,0xff,0x6f,0x04,0x01,0x02,0x00,0xff};
    const uint8_t msg1_data[] = {0x6f,0x04,0x01,0x01,0x00,0xff};
    const uint8_t msg2_data[] = {0x6f,0x04,0x01,0x02,0x00,0xff};
    
    message_t * message = malloc(sizeof(message_t));
    
    message->data = msg_data;
    message->length = sizeof(msg_data);
    
    message_t * message2 = malloc(sizeof(message_t));
    
    message2->data = msg1_data;
    message2->length = sizeof(msg1_data);
    
    message_t * message3 = malloc(sizeof(message_t));
    
    message3->data = msg2_data;
    message3->length = sizeof(msg2_data);

    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));
    
    phev_core_extractMessage_IgnoreAndReturn(message2);
    phev_core_extractMessage_IgnoreAndReturn(message3);

    messageBundle_t * messages = phev_controller_splitter(ctx, message);

    TEST_ASSERT_EQUAL(2, messages->numMessages);
    TEST_ASSERT_EQUAL(6, messages->messages[0]->length);
    TEST_ASSERT_EQUAL(6, messages->messages[1]->length);
    
    TEST_ASSERT_EQUAL_MEMORY(msg1_data, messages->messages[0]->data, 6);
    TEST_ASSERT_EQUAL_MEMORY(msg2_data, messages->messages[1]->data, 6);

}
static int startWifiCalled = 0;

void startWifiStub(void)
{
    startWifiCalled ++;
} 
void test_phev_controller_performUpdate(void)
{
    
    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = false,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        
    };
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));
    
    ctx->config = &config;
    ctx->startWifi = startWifiStub;
    
    ota_Ignore();

    phev_controller_performUpdate(ctx);

    TEST_ASSERT_EQUAL(1,startWifiCalled);
    TEST_ASSERT_TRUE(ctx->otaUpdating);
    
} 
void test_phev_controller_configIsNotSet(void)
{
        
    phevSettings_t phev_settings = {
        
    };

    msg_pipe_IgnoreAndReturn(NULL);
    phevCtx_t * ctx = phev_controller_init(&phev_settings);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_NULL(ctx->config);
}
void test_phev_controller_connectionConfigIsSet(void)
{
        
    phevSettings_t phev_settings = {
        
    };
    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        
    };

    msg_pipe_IgnoreAndReturn(NULL);
    phevCtx_t * ctx = phev_controller_init(&phev_settings);

    phev_controller_setConfig(ctx,&config);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_NOT_NULL(ctx->config);
    TEST_ASSERT_EQUAL_STRING(config.connectionConfig.host, ctx->config->connectionConfig.host);
    TEST_ASSERT_EQUAL(config.connectionConfig.port, ctx->config->connectionConfig.port);
    TEST_ASSERT_EQUAL_STRING(config.connectionConfig.carConnectionWifi.ssid, ctx->config->connectionConfig.carConnectionWifi.ssid);
    TEST_ASSERT_EQUAL_STRING(config.connectionConfig.carConnectionWifi.password, ctx->config->connectionConfig.carConnectionWifi.password);
    
}
void test_phev_controller_updateConfigIsSet(void)
{
        
    phevSettings_t phev_settings = {
        
    };

    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
    };

    msg_pipe_IgnoreAndReturn(NULL);
    phevCtx_t * ctx = phev_controller_init(&phev_settings);

    phev_controller_setConfig(ctx,&config);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_NOT_NULL(ctx->config);
    TEST_ASSERT_EQUAL(config.updateConfig.latestBuild, ctx->config->updateConfig.latestBuild);
    TEST_ASSERT_EQUAL(config.updateConfig.currentBuild, ctx->config->updateConfig.currentBuild);
    TEST_ASSERT_TRUE(ctx->config->updateConfig.updateOverPPP);
    TEST_ASSERT_EQUAL_STRING(config.updateConfig.updateWifi.ssid, ctx->config->updateConfig.updateWifi.ssid);
    TEST_ASSERT_EQUAL_STRING(config.updateConfig.updateWifi.password, ctx->config->updateConfig.updateWifi.password);
    TEST_ASSERT_EQUAL_STRING(config.updateConfig.updatePath, ctx->config->updateConfig.updatePath);
    TEST_ASSERT_EQUAL_STRING(config.updateConfig.updateHost, ctx->config->updateConfig.updateHost);
    TEST_ASSERT_EQUAL(config.updateConfig.updatePort, ctx->config->updateConfig.updatePort);
    TEST_ASSERT_EQUAL_STRING(config.updateConfig.updateImageFullPath, ctx->config->updateConfig.updateImageFullPath);
    TEST_ASSERT_TRUE(ctx->config->updateConfig.forceUpdate);

}
void test_phev_controller_configStateIsSet(void)
{
        
    phevSettings_t phev_settings = {
        
    };

    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        .state.connectedClients = 1,
        .state.headLightsOn = true,
        .state.parkLightsOn = true,
        .state.airConOn = true,
    };

    msg_pipe_IgnoreAndReturn(NULL);
    phevCtx_t * ctx = phev_controller_init(&phev_settings);

    phev_controller_setConfig(ctx,&config);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_NOT_NULL(ctx->config);
    TEST_ASSERT_EQUAL(config.state.connectedClients, ctx->config->state.connectedClients);
    TEST_ASSERT_TRUE(ctx->config->state.headLightsOn);
    TEST_ASSERT_TRUE(ctx->config->state.parkLightsOn);
    TEST_ASSERT_TRUE(ctx->config->state.airConOn);
    
}
void test_phev_controller_configToMessageBundle_no_updates(void)
{

    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        .state.connectedClients = 0,
        .state.headLightsOn = false,
        .state.parkLightsOn = false,
        .state.airConOn = false,
    };

    phev_config_checkForConnection_IgnoreAndReturn(false);
    //phev_core_startMessageEncoded_IgnoreAndReturn(NULL);
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(false);

    messageBundle_t * messages = phev_controller_configToMessageBundle(&config);

    TEST_ASSERT_NOT_NULL(messages);
    TEST_ASSERT_EQUAL(0,messages->numMessages);
}
void test_phev_controller_configToMessageBundle_new_connection(void)
{

    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        .state.connectedClients = 1,
        .state.headLightsOn = false,
        .state.parkLightsOn = false,
        .state.airConOn = false,
    };

    uint8_t data[] = {1,2,3,4};
    message_t * start = msg_utils_createMsg(&data ,4);
    phev_config_checkForConnection_IgnoreAndReturn(true);
    phev_core_startMessageEncoded_IgnoreAndReturn(start);
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(false);

    messageBundle_t * messages = phev_controller_configToMessageBundle(&config);

    TEST_ASSERT_NOT_NULL(messages);
    TEST_ASSERT_EQUAL(1,messages->numMessages);
    TEST_ASSERT_NOT_NULL(messages->messages[0]);
    TEST_ASSERT_EQUAL(4,messages->messages[0]->length);
    TEST_ASSERT_EQUAL_MEMORY(&data, messages->messages[0]->data,4);

}
void test_phev_controller_configToMessageBundle_head_lights_on(void)
{

    phevConfig_t config = {
        .connectionConfig.host = "127.0.0.1",
        .connectionConfig.port = 8080,
        .connectionConfig.carConnectionWifi.ssid = "SSID",
        .connectionConfig.carConnectionWifi.password = "password",
        .updateConfig.latestBuild = 1234,
        .updateConfig.currentBuild = 1234567,  
        .updateConfig.updateOverPPP = true,  
        .updateConfig.updateWifi.ssid = "SSID123",
        .updateConfig.updateWifi.password = "password123",
        .updateConfig.updatePath = "/path12345678",
        .updateConfig.updateHost = "update.host.com",
        .updateConfig.updatePort = 80,
        .updateConfig.updateImageFullPath = "http://update.host.com/path",
        .updateConfig.forceUpdate = true,
        .state.connectedClients = 1,
        .state.headLightsOn = false,
        .state.parkLightsOn = false,
        .state.airConOn = false,
    };

    uint8_t data[] = {1,2,3,4};
    message_t * cmd = msg_utils_createMsg(&data ,4);
    phev_config_checkForConnection_IgnoreAndReturn(false);
    phev_core_simpleRequestCommandMessage_IgnoreAndReturn(cmd);
    phev_core_convertToMessage_IgnoreAndReturn(cmd);
    phev_core_destroyMessage_Ignore();
    phev_config_checkForHeadLightsOn_IgnoreAndReturn(true);

    messageBundle_t * messages = phev_controller_configToMessageBundle(&config);

    TEST_ASSERT_NOT_NULL(messages);
    TEST_ASSERT_EQUAL(1,messages->numMessages);
    TEST_ASSERT_NOT_NULL(messages->messages[0]);
    TEST_ASSERT_EQUAL(4,messages->messages[0]->length);
    TEST_ASSERT_EQUAL_MEMORY(&data, messages->messages[0]->data,4);

} */