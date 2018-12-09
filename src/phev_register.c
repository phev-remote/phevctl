#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "phev_core.h"
#include "phev_pipe.h"
#include "phev_register.h"

const char * TAG = "PHEV_REGISTER";

phevRegisterCtx_t * phev_register_init(phevRegisterSettings_t settings)
{
    phevRegisterCtx_t * ctx = malloc(sizeof(phevRegisterCtx_t));

    ctx->pipe = settings.pipe;
    ctx->complete = settings.complete;
    
    memcpy(ctx->mac,settings.mac,MAC_ADDR_SIZE);

    ctx->pipe->ctx = ctx;

    phev_pipe_registerEventHandler(settings.pipe, settings.eventHandler);

    return ctx;
}
void phev_register_sendRegister(phev_pipe_ctx_t * ctx)
{
    phevMessage_t * initMessage = phev_core_simpleRequestCommandMessage(KO_WF_INIT_RQ_SP,0);
    message_t * message = phev_core_convertToMessage(initMessage);

    msg_pipe_outboundPublish(ctx->pipe, message);
    free(message);
    free(initMessage);
}

void phev_register_sendMac(phev_pipe_ctx_t * ctx)
{
    LOG_V(TAG,"START - sendMac");
    
    message_t * message = phev_core_startMessageEncoded(((phevRegisterCtx_t *) ctx->ctx)->mac);
    msg_pipe_outboundPublish(ctx->pipe,  message);
    free(message);
    LOG_V(TAG,"END - sendMac");
    
}

int phev_register_eventHandler(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event)
{
    switch(event->event) 
    {
        case PHEV_PIPE_GOT_VIN: {
            
            phev_register_sendMac(ctx);
            break;
        }
        case PHEV_PIPE_AA_ACK: {
             LOG_I(TAG,"Start acknowledged");
            //phev_register_sendRegister(ctx);
            break;
        }
        case PHEV_PIPE_REGISTRATION: {
            LOG_I(TAG,"Registration complete");
            if(((phevRegisterCtx_t *) ctx->ctx)->complete != NULL)
            {
                LOG_D(TAG,"Calling callback");
            
                ((phevRegisterCtx_t *) ctx->ctx)->complete();
            }
            
            break;
        }
        default : {
            LOG_W(TAG, "Unknown event %d\n",event->event);
        }
    }
}
