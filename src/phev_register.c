#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "phev_pipe.h"
#include "phev_register.h"

const char * TAG = "PHEV_REGISTER";

phevRegisterCtx_t * phev_register_init(phevRegisterSettings_t settings)
{
    phevRegisterCtx_t * ctx = malloc(sizeof(phevRegisterCtx_t));

    ctx->pipe = settings.pipe;

    phev_pipe_registerEventHandler(settings.pipe, settings.eventHandler);
    
    return ctx;
}
void phev_register_start(phevRegisterCtx_t * ctx)
{
    //phev_core_createMessage(SEND_CMD, REQUEST_TYPE, , uint8_t * data, size_t length);

}

int phev_register_eventHandler(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event)
{
    switch(event->event) 
    {
        case PHEV_PIPE_GOT_VIN: {
            

            //phev_pipe_sendRegister(event->mac);
            break;
        }
        default : {
            LOG_W(TAG, "Unknown event %d\n",event->event);
        }
    }
}
