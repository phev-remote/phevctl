#ifndef _PHEV_CONTROLLER_H_
#define _PHEV_CONTROLLER_H_
#include <stdbool.h>
#include <time.h>
#include "phev_core.h"
//#include "phev_store.h"
#include "phev_config.h"

#include "msg_pipe.h"

#define MAX_QUEUED_COMMANDS 10
#define PHEV_OK 0
#define IMAGE_PREFIX "firmware-"

#define KO_WF_MANUAL_AC_ON_RQ_SP 4
#define KO_WF_DATE_INFO_SYNC_SP 5
#define KO_WF_H_LAMP_CONT_SP 10
#define KO_WF_P_LAMP_CONT_SP 11


#ifdef __XTENSA__ 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PHEV_SEC_DELAY(n) vTaskDelay((n*1000) / portTICK_PERIOD_MS) 
#else
#define PHEV_SEC_DELAY(n) sleep(n) 
#endif

typedef struct phevCtx_t phevCtx_t;

typedef void (* phevStartWifi_t)(const char * ssid, const char * password, bool setPPPDefault);
typedef struct phevSettings_t
{
//    phevStore_t * store;
    messagingClient_t * in;
    messagingClient_t * out;
    msg_pipe_transformer_t inputTransformer;
    msg_pipe_transformer_t outputTransformer;
    phevStartWifi_t startWifi;
} phevSettings_t;

typedef enum phevEventType { CONNECT_REQUEST } phevEventType_t; 

struct phevCtx_t {    
    phevStore_t * store;
    phevConfig_t * config;
    phevMessage_t * queuedCommands[MAX_QUEUED_COMMANDS];
    size_t queueSize;
    msg_pipe_ctx_t * pipe;
    msg_pipe_transformer_t outputTransformer;
    phevStartWifi_t startWifi;
    int (* connect)(const char*, uint16_t);
    uint8_t currentPing;
    bool successfulPing;
    time_t lastPingTime;
    bool otaUpdating;
}; 

typedef struct phevEvent_t
{
    phevEventType_t type;
} phevEvent_t;

int phev_controller_handleEvent(phevEvent_t * event);

phevCtx_t * phev_controller_init(phevSettings_t * settings);
void phev_controller_setCarConnectionConfig(phevCtx_t *ctx, const char * ssid, const char * password, const char * host, const uint16_t port);
//void phev_controller_connect(phevCtx_t * ctx);
void phev_controller_ping(phevCtx_t * ctx);
void phev_controller_resetPing(phevCtx_t * ctx);

void phev_controller_setUpdateConfig(phevCtx_t * ctx, const char * ssid, const char * password, const char * host, const char * path, uint16_t port, int build);
void phev_controller_updateConfig(phevCtx_t * ctx, phevConfig_t * config);
    
message_t * phev_controller_input_responder(void * ctx, message_t * message);
void phev_controller_sendMessage(phevCtx_t * ctx, message_t * message);
void phev_controller_sendMessageBundle(phevCtx_t * ctx, messageBundle_t * messages);
message_t * phev_controller_turnHeadLightsOn(void);
message_t * phev_controller_turnHeadLightsOff(void);
message_t * phev_controller_turnParkLightsOn(void);
message_t * phev_controller_turnParkLightsOff(void);
message_t * phev_controller_turnAirConOn(void);
message_t * phev_controller_turnAirConOff(void);

messageBundle_t * phev_controller_splitter(void * ctx, message_t * message);    
messageBundle_t * phev_controller_configSplitter(void * ctx, message_t * message);

void phev_controller_initConfig(phevConfig_t * config);
void phev_controller_initState(phevState_t * state);
void phev_controller_setConfig(phevCtx_t * ctx, phevConfig_t * config);
void phev_controller_setConfigJson(phevCtx_t * ctx, char * config);

void phev_controller_eventLoop(phevCtx_t * ctx);
void phev_controller_performUpdate(phevCtx_t * ctx);
messageBundle_t * phev_controller_configToMessageBundle(phevConfig_t * config);
void phev_controller_sendState(phevCtx_t * ctx);
msg_pipe_ctx_t * phev_controller_createPipe(void * ctx, messagingClient_t * in, messagingClient_t * out);

#endif
