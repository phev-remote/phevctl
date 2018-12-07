#ifndef _PHEV_REGISTER_H_
#define _PHEV_REGISTER_H_

#include "msg_pipe.h"
#include "phev_pipe.h"
enum {
    PHEV_REGISTER_GOT_VIN,
};

typedef struct phevRegisterSettings_t {
    phev_pipe_ctx_t * pipe;
    phevPipeEventHandler_t eventHandler;
} phevRegisterSettings_t;

typedef struct phevRegisterCtx_t {
    phev_pipe_ctx_t * pipe;
} phevRegisterCtx_t;

phevRegisterCtx_t * phev_register_init(phevRegisterSettings_t);
void phev_register_start(phevRegisterCtx_t *);
int phev_register_eventHandler(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event);

#endif