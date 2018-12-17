#ifndef _PHEV_PIPE_H_
#define _PHEV_PIPE_H_
#include <time.h>
#include "msg_core.h"
#include "msg_pipe.h"
#include "phev_core.h"


enum {
    PHEV_PIPE_GOT_VIN,
    PHEV_PIPE_AA_ACK,
    PHEV_PIPE_START_ACK,
    PHEV_PIPE_REGISTRATION,
    PHEV_PIPE_ECU_VERSION2,
    PHEV_PIPE_REMOTE_SECURTY_PRSNT_INFO,
    PHEV_PIPE_REG_DISP,
    PHEV_PIPE_MAX_REGISTRATIONS,
};

typedef struct phevPipeEvent_t 
{
    int event;
    size_t length;
    uint8_t * data;
} phevPipeEvent_t;

typedef struct phevVinEvent_t
{
    char vin[18];
    uint8_t data;
    uint8_t registrations;
} phevVinEvent_t;

typedef struct phev_pipe_ctx_t phev_pipe_ctx_t;
typedef void phevError_t;
typedef int (* phevPipeEventHandler_t)(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event);
typedef void (* phevErrorHandler_t)(phevError_t * error);

typedef struct phev_pipe_ctx_t {
    msg_pipe_ctx_t * pipe;
    phevPipeEventHandler_t eventHandler;
    phevErrorHandler_t errorHandler;
    time_t lastPingTime;
    uint8_t currentPing;
    void * ctx;
} phev_pipe_ctx_t;

typedef struct phev_pipe_settings_t {
    messagingClient_t * in;
    messagingClient_t * out;
    msg_pipe_splitter_t inputSplitter;
    msg_pipe_splitter_t outputSplitter;
    msg_pipe_responder_t inputResponder;
    msg_pipe_responder_t outputResponder;
    msg_pipe_transformer_t outputInputTransformer;
    msg_pipe_transformer_t outputOutputTransformer;
    msg_pipe_connectHook_t preConnectHook;
    phevErrorHandler_t errorHandler;
    void * ctx;
} phev_pipe_settings_t;

void phev_pipe_loop(phev_pipe_ctx_t *);
phev_pipe_ctx_t * phev_pipe_createPipe(phev_pipe_settings_t);
message_t * phev_pipe_outputChainInputTransformer(void *, message_t *);
message_t * phev_pipe_outputEventTransformer(void *, message_t *);
void phev_pipe_registerEventHandler(phev_pipe_ctx_t *, phevPipeEventHandler_t);
void phev_pipe_deregisterEventHandler(phev_pipe_ctx_t *, phevPipeEventHandler_t);
message_t * phev_pipe_commandResponder(void *, message_t *);
messageBundle_t * phev_pipe_outputSplitter(void *, message_t *);
void phev_pipe_ping(phev_pipe_ctx_t *);
void phev_pipe_resetPing(phev_pipe_ctx_t *);
//void phev_pipe_sendCommand(phev_core_command_t);

#endif
