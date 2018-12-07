#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "phev_controller.h"
#include "phev_response_handler.h"
//#include "phev_store.h"
#include "msg_pipe.h"
#include "msg_utils.h"
#include "msg_tcpip.h"
#include "msg_gcp_mqtt.h"
#include "msg_mqtt.h"
#include "phev_ota.h"
#include "logger.h"

const static char *APP_TAG = "PHEV_CONTROLLER";

void phev_controller_preOutConnectHook(msg_pipe_ctx_t * pipe)
{
    LOG_V(APP_TAG,"START - preOutConnectHook");
    phevCtx_t * ctx = (phevCtx_t *) pipe->user_context;
    LOG_D(APP_TAG,"Setting wifi ssid %s password %s",ctx->config->connectionConfig.carConnectionWifi.ssid 
                                                    ,ctx->config->connectionConfig.carConnectionWifi.password);
    if(ctx->startWifi) 
    {
        ctx->startWifi(ctx->config->connectionConfig.carConnectionWifi.ssid,
                    ctx->config->connectionConfig.carConnectionWifi.password,true);
    }
    LOG_D(APP_TAG,"Setting tcpip ctx host %s port %d",ctx->config->connectionConfig.host,
                                                        ctx->config->connectionConfig.port);
    ((tcpip_ctx_t *) ctx->pipe->out->ctx)->host = ctx->config->connectionConfig.host;
    ((tcpip_ctx_t *) ctx->pipe->out->ctx)->port = ctx->config->connectionConfig.port;
    LOG_V(APP_TAG,"END - preOutConnectHook");
    
    //ctx->pipe->in->connect(ctx->pipe->in);
    
}
message_t * phev_controller_input_responder(void * ctx, message_t * message) {
    
    return msg_utils_createMsgTopic(DEVICE_STATE_TOPIC,(uint8_t *) "Hello",5);
}
message_t * phev_controller_responder(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - responder");
    phevCtx_t * phevCtx = (phevCtx_t *) ctx;
    if(message != NULL) {

        phevMessage_t phevMsg;

        phev_core_decodeMessage(message->data, message->length, &phevMsg);

        if((phevMsg.type == REQUEST_TYPE)) // && (phevMsg.command == 0x6f))
        {
            phevMessage_t * msg = phev_core_responseHandler(&phevMsg);
            message_t * out = phev_core_convertToMessage(msg);
            phev_core_destroyMessage(msg);
            LOG_V(APP_TAG,"END - responder - 1");
    
            return out;
        } else {
            if(phevMsg.command == 0x9f) {
                phevCtx->currentPing = (message->data[3] + 1) % 100;
                phevCtx->successfulPing = true;
            }
        }
        free(phevMsg.data);
        
    }
    LOG_V(APP_TAG,"END - responder 2");
    return NULL;

}

#define DEVICE_STATE_TOPIC "my-device-state"

void phev_controller_sendState(phevCtx_t * ctx)
{
    LOG_V(APP_TAG,"START - sendState");

    //gcp_ctx_t * gcp = (gcp_ctx_t *) ctx->pipe->in->ctx;
    message_t * message = msg_utils_createMsg((unsigned char *) "Hello",5);
    //msg_mqtt_publish(((gcp_ctx_t *) ctx->pipe->in->ctx)->mqtt,DEVICE_STATE_TOPIC,message);
    LOG_V(APP_TAG,"END - sendState");
}
messageBundle_t * phev_controller_splitter(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - splitter");
    
    LOG_BUFFER_HEXDUMP(APP_TAG, message->data,message->length,LOG_DEBUG);
    message_t * out = phev_core_extractMessage(message->data, message->length);

    if(out == NULL) return NULL;
    messageBundle_t * messages = malloc(sizeof(messageBundle_t));

    messages->numMessages = 0;
    messages->messages[messages->numMessages++] = out;
    
    int total = out->length;

    while(message->length > total)
    {
        out = phev_core_extractMessage(message->data + total, message->length - total);
        if(out!= NULL)
        {
            total += out->length;
            messages->messages[messages->numMessages++] = out;
        } else {
            break;
        }
        
    }
    LOG_D(APP_TAG,"Split messages into %d",messages->numMessages);
    LOG_MSG_BUNDLE(APP_TAG,messages);
    LOG_V(APP_TAG,"END - splitter");
    return messages;
}

message_t * phev_controller_outputChainInputTransformer(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - outputChainInputTransformer");
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));

    int length = phev_core_decodeMessage(message->data, message->length, phevMessage);
            
    if(length == 0) {
        LOG_E(APP_TAG,"Invalid message received");
        LOG_BUFFER_HEXDUMP(APP_TAG,message->data,message->length,LOG_DEBUG);
        //msg_utils_destroyMsg(message);
    
        return NULL;
    }
    message_t * ret = phev_core_convertToMessage(phevMessage);

    phev_core_destroyMessage(phevMessage);
    
    LOG_V(APP_TAG,"END - outputChainInputTransformer");
    
    return ret;
}

message_t * phev_controller_outputChainOutputTransformer(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - outputChainOutputTransformer");
    
    phevCtx_t * phevCtx = (phevCtx_t *) ctx;
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));
    phev_core_decodeMessage(message->data,message->length, phevMessage);
    
    message_t * ret = phev_response_handler(ctx, phevMessage);
    
    if(ret == NULL)
    {
        LOG_D(APP_TAG,"No Response required");
    }

    phev_core_destroyMessage(phevMessage);
    
    LOG_V(APP_TAG,"END - outputChainOutputTransformer");
    
    return ret;
}

void phev_controller_initState(phevState_t * state)
{
    LOG_V(APP_TAG,"START - initState");
    
    state->connectedClients = 0;
    LOG_V(APP_TAG,"END - initState");
    
}
void phev_controller_initConfig(phevConfig_t * config)
{
    unsigned long long int buildNumber = (unsigned long long int) BUILD_NUMBER;
    
    LOG_V(APP_TAG,"START - initConfig");
    
    LOG_D(APP_TAG,"Setting build number in config to %llu", buildNumber);
    
    config->updateConfig.currentBuild = BUILD_NUMBER;
    
    phev_controller_initState(&config->state);
    LOG_V(APP_TAG,"END - initConfig");
    
}
messageBundle_t * phev_controller_configToMessageBundle(phevConfig_t * config)
{
    LOG_V(APP_TAG,"START - configToMessageBundle");
    
    messageBundle_t * messages = malloc(sizeof(messageBundle_t));
    messages->numMessages = 0;
    
    if(phev_config_checkForConnection(&config->state)) 
    {
        LOG_D(APP_TAG,"Connection change, sending start message");
    
        // replace with real MAC

        uint8_t mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        messages->messages[messages->numMessages++] = phev_core_startMessageEncoded(2,mac);
    }

    if(phev_config_checkForHeadLightsOn(&config->state)) 
    {
        LOG_D(APP_TAG,"Head lights change, sending head lights on message");
    
        messages->messages[messages->numMessages++] = phev_controller_turnHeadLightsOn();
    } 
    if(phev_config_checkForHeadLightsOff(&config->state)) 
    {
        LOG_D(APP_TAG,"Head lights change, sending head lights off message");
    
        messages->messages[messages->numMessages++] = phev_controller_turnHeadLightsOff();    
    }
    if(phev_config_checkForParkLightsOn(&config->state)) 
    {
        LOG_D(APP_TAG,"Parking lights change, sending parking lights on message");
    
        messages->messages[messages->numMessages++] = phev_controller_turnParkLightsOn();
    } 
    if(phev_config_checkForParkLightsOff(&config->state)) 
    {
        LOG_D(APP_TAG,"Parking lights change, sending parking lights off message");
    
        messages->messages[messages->numMessages++] = phev_controller_turnParkLightsOff();
        
    }
    if(phev_config_checkForAirConOn(&config->state)) 
    {
        LOG_D(APP_TAG,"Air con change, sending air con on message");
    
        messages->messages[messages->numMessages++] = phev_controller_turnAirConOn();
    } 
    if(phev_config_checkForAirConOff(&config->state)) 
    {
        LOG_D(APP_TAG,"Air con change, sending air con off message");
    
        messages->messages[messages->numMessages++] = phev_controller_turnAirConOff();
    }
    LOG_V(APP_TAG,"END - configToMessageBundle");
    
    return messages;

}

void phev_controller_updateConfig(phevCtx_t * ctx, phevConfig_t * config)
{
    LOG_V(APP_TAG,"START - updateConfig");
    //if(ctx->config != NULL)
    //{
    //    LOG_D(APP_TAG,"Freeing old config");
    
    //    free(ctx->config);
   // }
    //ctx->config = config;
    
    messageBundle_t * messages = phev_controller_configToMessageBundle(ctx->config);   

    phev_controller_sendMessageBundle(ctx, messages);
    LOG_V(APP_TAG,"END - updateConfig");
    
}
int phev_controller_handleEvent(phevEvent_t * event)
{
    return PHEV_OK;
}
void phev_controller_sendCommand(phevCtx_t * ctx, phevMessage_t * message) 
{
    int index = ctx->queueSize;
    ctx->queuedCommands[index] = malloc(sizeof(phevMessage_t));
    ctx->queuedCommands[index]->data = malloc(message->length - 2);
    memcpy(ctx->queuedCommands[index], message, sizeof(phevMessage_t));
    memcpy(ctx->queuedCommands[index]->data, message->data, message->length - 2); 

    ctx->queueSize ++;
}
void phev_controller_connect(phevCtx_t * ctx)
{ 
    uint8_t data[] = {0,0,0,0,0,0};

    phevMessage_t start = {
        .command    = START_SEND,
        .length     = 0x0a,
        .type       = REQUEST_TYPE,
        .reg        = 0x01,  
        .data       = &data,
    };
    phev_controller_sendCommand(ctx, &start);

    //phev_controller_waitForResponse(ctx);

} 
void phev_controller_sendMessage(phevCtx_t * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - phev sendMessage");
    if(message != NULL)
    {
        msg_pipe_outboundPublish(ctx->pipe, message);
    }
    
    LOG_V(APP_TAG,"END - phev sendMessage");
    
}
void phev_controller_ping(phevCtx_t * ctx)
{
    LOG_V(APP_TAG,"START - ping");
    if(((ctx->currentPing + 1) % 30) == 0) 
    {
        LOG_D(APP_TAG,"Send time sync");
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);
        
        const uint8_t pingTime[] = {
            timeinfo.tm_year - 100,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec,
            1
        };
        phevMessage_t * dateCmd = phev_core_commandMessage(KO_WF_DATE_INFO_SYNC_SP,pingTime, sizeof(pingTime));
        message_t * message = phev_core_convertToMessage(dateCmd);
        phev_controller_sendMessage(ctx, message);
        //msg_utils_destroyMsg(message);
        //phev_core_destroyMessage(dateCmd);
    
    }
    ctx->successfulPing = false;
    phevMessage_t * ping = phev_core_pingMessage(ctx->currentPing);
    message_t * message = phev_core_convertToMessage(ping);
    phev_controller_sendMessage(ctx, message);
    //msg_utils_destroyMsg(message);
    //phev_core_destroyMessage(ping);
    LOG_V(APP_TAG,"END - ping");
    
}
void phev_controller_resetPing(phevCtx_t * ctx)
{
    ctx->currentPing = 0;
    ctx->lastPingTime = 0;
}
void phev_controller_performUpdate(phevCtx_t * ctx)
{
    LOG_V(APP_TAG,"START - performUpdate");
    
    ctx->otaUpdating = true;
    
    if(!ctx->config->updateConfig.updateOverPPP)
    {
        LOG_D(APP_TAG,"Perform update over WIFI");
        if(ctx->startWifi) {
            ctx->startWifi(ctx->config->updateConfig.updateWifi.ssid,ctx->config->updateConfig.updateWifi.password,false);
            LOG_D(APP_TAG,"Started UPDATE WIFI");
        } else {
            LOG_D(APP_TAG,"WIFI not configured");
        }
        
        
    } else {
        LOG_D(APP_TAG,"Perform update over GSM");
    }
    ota(ctx->config->updateConfig.updateHost,ctx->config->updateConfig.updatePort, ctx->config->updateConfig.updateImageFullPath);
    LOG_V(APP_TAG,"END - performUpdate");
    
}
message_t * phev_controller_turnHeadLightsOn(void)
{
    phevMessage_t * headLightsOn = phev_core_simpleRequestCommandMessage(KO_WF_H_LAMP_CONT_SP, 1);
    message_t * message = phev_core_convertToMessage(headLightsOn);
    phev_core_destroyMessage(headLightsOn);
    
    return message;
}
message_t * phev_controller_turnHeadLightsOff(void)
{
    phevMessage_t * headLightsOff = phev_core_simpleRequestCommandMessage(KO_WF_H_LAMP_CONT_SP, 2);
    message_t * message = phev_core_convertToMessage(headLightsOff);
    phev_core_destroyMessage(headLightsOff);
    
    return message;
}
message_t * phev_controller_turnParkLightsOn(void)
{
    phevMessage_t * parkLights = phev_core_simpleRequestCommandMessage(KO_WF_P_LAMP_CONT_SP, 1);
    message_t * message = phev_core_convertToMessage(parkLights);
    phev_core_destroyMessage(parkLights);
    
    return message;
}
message_t * phev_controller_turnParkLightsOff(void)
{
    phevMessage_t * airCon = phev_core_simpleRequestCommandMessage(KO_WF_P_LAMP_CONT_SP, 2);
    message_t * message = phev_core_convertToMessage(airCon);
    phev_core_destroyMessage(airCon);
    
    return message;
}
message_t * phev_controller_turnAirConOn(void)
{
    phevMessage_t * airCon = phev_core_simpleRequestCommandMessage(KO_WF_MANUAL_AC_ON_RQ_SP, 2);
    message_t * message = phev_core_convertToMessage(airCon);
    phev_core_destroyMessage(airCon);
    
    return message;
}
message_t * phev_controller_turnAirConOff(void)
{
    phevMessage_t * airCon = phev_core_simpleRequestCommandMessage(KO_WF_MANUAL_AC_ON_RQ_SP, 1);
    message_t * message = phev_core_convertToMessage(airCon);
    phev_core_destroyMessage(airCon);
    
    return message;
}
void phev_controller_sendMessageBundle(phevCtx_t * ctx, messageBundle_t * messages)
{
    LOG_V(APP_TAG,"START - sendMessageBundle");
    
    for(int i=0;i<messages->numMessages;i++)
    {
        phev_controller_sendMessage(ctx,messages->messages[i]);
    }
    LOG_V(APP_TAG,"END - sendMessageBundle");
    
}
void phev_controller_setConfig(phevCtx_t * ctx, phevConfig_t * config)
{
    LOG_V(APP_TAG,"START - setConfig");
    
    if(ctx->config == NULL)
    {
        LOG_D(APP_TAG,"Allocated new config");
        ctx->config = malloc(sizeof(phevConfig_t));
        ctx->config->state.headLightsOn = FALSE;
        ctx->config->state.parkLightsOn = FALSE;
        ctx->config->state.airConOn = FALSE;

    }

    ctx->config->connectionConfig.host = phev_core_strdup(config->connectionConfig.host);
    ctx->config->connectionConfig.port = config->connectionConfig.port;
    strncpy(ctx->config->connectionConfig.carConnectionWifi.ssid,config->connectionConfig.carConnectionWifi.ssid,MAX_WIFI_SSID_LEN);
    strncpy(ctx->config->connectionConfig.carConnectionWifi.password,config->connectionConfig.carConnectionWifi.password,MAX_WIFI_PASSWORD_LEN);
    ctx->config->updateConfig.latestBuild = config->updateConfig.latestBuild;
    ctx->config->updateConfig.currentBuild = config->updateConfig.currentBuild;
    ctx->config->updateConfig.updateOverPPP = config->updateConfig.updateOverPPP;
    strncpy(ctx->config->updateConfig.updateWifi.ssid,config->updateConfig.updateWifi.ssid,MAX_WIFI_SSID_LEN);
    strncpy(ctx->config->updateConfig.updateWifi.password,config->updateConfig.updateWifi.password,MAX_WIFI_PASSWORD_LEN);
    ctx->config->updateConfig.updatePath = malloc(strlen(config->updateConfig.updatePath));
    ctx->config->updateConfig.updatePath = phev_core_strdup(config->updateConfig.updatePath);
    ctx->config->updateConfig.updateHost = phev_core_strdup(config->updateConfig.updateHost);
    ctx->config->updateConfig.updatePort = config->updateConfig.updatePort;
    ctx->config->updateConfig.updateImageFullPath = phev_core_strdup(config->updateConfig.updateImageFullPath);
    ctx->config->updateConfig.forceUpdate = config->updateConfig.forceUpdate;
    ctx->config->state.connectedClients = config->state.connectedClients;

    if(config->state.headLightsOn != NOTSET)
    {
        ctx->config->state.headLightsOn = config->state.headLightsOn;
    }
    if(config->state.parkLightsOn != NOTSET)
    {
        ctx->config->state.parkLightsOn = config->state.parkLightsOn;
    }
    if(config->state.airConOn != NOTSET)
    {
        ctx->config->state.airConOn = config->state.airConOn;
    }
    
    //phev_controller_sendState(ctx);
    //LOG_I(APP_TAG,"%s",phev_config_displayConfig(config));
    LOG_V(APP_TAG,"END - setConfig");
}


void phev_controller_setConfigJson(phevCtx_t * ctx, char * jsonConf)
{
    LOG_V(APP_TAG,"START - setConfigJson");
    phevConfig_t * config = phev_config_parseConfig(jsonConf);
    
    phev_controller_setConfig(ctx,config);

    //LOG_I(APP_TAG,"%s",phev_config_displayConfig(config));
    LOG_V(APP_TAG,"END - setConfigJson");
}

messageBundle_t * phev_controller_configSplitter(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - configSplitter");
    phevCtx_t * phevCtx = (phevCtx_t *) ctx;
    phev_controller_setConfigJson(phevCtx, (char *) message->data);
    
    LOG_D(APP_TAG,"Checking for update");
    
    if(phev_config_checkForFirmwareUpdate(&phevCtx->config->updateConfig)) 
    {
        LOG_D(APP_TAG,"Update found");
        phev_controller_performUpdate(phevCtx);
        return NULL; // shouldn't get here under normal conditions
    }
    LOG_D(APP_TAG,"No new update");
    
    
    messageBundle_t * messages = phev_controller_configToMessageBundle(phevCtx->config);
    
    LOG_V(APP_TAG,"END - configSplitter");
    
    return messages; 

} 
void phev_controller_eventLoop(phevCtx_t * ctx)
{
    //LOG_V(APP_TAG,"START - eventLoop");
    
    time_t now;

    if(!ctx->pipe->in->connected)
    {
        LOG_D(APP_TAG,"Inbound Client connect");
    
        ctx->pipe->in->connect(ctx->pipe->in);
    } else {
        if(!ctx->pipe->out->connected && ctx->currentPing > 0)
        {
            LOG_D(APP_TAG,"Resetting ping");
    
            phev_controller_resetPing(ctx);

        /*    if(ctx->config != NULL)
            {
                while(ctx->pipe->out->connect(ctx->pipe->out) != 0) 
                {
                    LOG_E(APP_TAG,"Retrying after 10 seconds");
                    PHEV_SEC_DELAY(10);
                }
            } */    
        } 
        msg_pipe_loop(ctx->pipe);
        time(&now);
        if(now > ctx->lastPingTime && ctx->pipe->out->connected) {
            phev_controller_ping(ctx);
            time(&ctx->lastPingTime);
        }
    }
    //LOG_V(APP_TAG,"END - eventLoop");
       
}
bool phev_controller_filter(void * ctx, message_t * message)
{
    phevCtx_t * phevCtx = (phevCtx_t *) ctx;
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));
    
    bool ret = true;

    int len = phev_core_decodeMessage(message->data, message->length, phevMessage);

    if(phevMessage->command == 0x6f && len > 0) 
    {
   //     if(phev_store_compare(phevCtx->store,phevMessage->reg,phevMessage->data) == 0)
   //     {
   //         ret = false;
   //     } 
   //     else 
   //     {
   //         phev_store_add(phevCtx->store,phevMessage->reg,phevMessage->data,phevMessage->length - 3);
   //     }
    } 
    if(phevMessage != NULL)
    {
        if(phevMessage->data != NULL)
        {
            free(phevMessage->data);
        }
        free(phevMessage);
    }
    
    
    return ret;
}
msg_pipe_ctx_t * phev_controller_createPipe(void * ctx, messagingClient_t * in, messagingClient_t * out)
{
    msg_pipe_chain_t * inputChain = malloc(sizeof(msg_pipe_chain_t));
    msg_pipe_chain_t * outputChain = malloc(sizeof(msg_pipe_chain_t));

    inputChain->inputTransformer = NULL;
    inputChain->splitter = phev_controller_configSplitter;
    inputChain->filter = NULL;
    inputChain->outputTransformer = NULL;
    inputChain->responder = phev_response_incomingHandler;
    inputChain->aggregator = NULL;
    inputChain->respondOnce = true;
    
    outputChain->inputTransformer = phev_controller_outputChainInputTransformer;
    outputChain->splitter = phev_controller_splitter;
    outputChain->filter = NULL; //phev_controller_filter;
    outputChain->outputTransformer = phev_controller_outputChainOutputTransformer;
    outputChain->responder = phev_controller_responder;
    outputChain->aggregator = NULL;
    outputChain->respondOnce = false;

    msg_pipe_settings_t pipe_settings = {
        .in = in,
        .out = out,
        .lazyConnect = 1,
        .user_context = ctx,
        .in_chain = inputChain,
        .out_chain = outputChain,
        .preOutConnectHook = phev_controller_preOutConnectHook,
    };

    return msg_pipe(pipe_settings);

}
phevCtx_t * phev_controller_init(phevSettings_t * settings)
{
    LOG_V(APP_TAG,"START - init");
    phevCtx_t * ctx = malloc(sizeof(phevCtx_t));

    
    if(settings->startWifi)
    {
        ctx->startWifi = settings->startWifi;
    } else {
        ctx->startWifi = NULL;
    }
    ctx->outputTransformer = settings->outputTransformer;

    ctx->config = NULL;

    ctx->pipe = phev_controller_createPipe(ctx,settings->in,settings->out);
    
    ctx->queueSize = 0;
    ctx->currentPing = 0;
    ctx->successfulPing = false;
    ctx->lastPingTime = 0;
    ctx->otaUpdating = false;
    ctx->store = settings->store;

    LOG_V(APP_TAG,"END - init");
    
    return ctx;
}


