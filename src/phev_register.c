#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "phev_core.h"
#include "phev_pipe.h"
#include "phev_register.h"

const char * TAG = "PHEV_REGISTER";

void phev_register_sendMac(phev_pipe_ctx_t * ctx)
{
    LOG_V(TAG,"START - sendMac");
    
    message_t * message = phev_core_startMessageEncoded(((phevRegisterCtx_t *) ctx->ctx)->mac);
    msg_pipe_outboundPublish(ctx->pipe,  message);
    if(ctx->started)
    {
        ctx->started(ctx);
    }
    //free(message);
    LOG_V(TAG,"END - sendMac");
    
}
phevRegisterCtx_t * phev_register_init(phevRegisterSettings_t settings)
{
    phevRegisterCtx_t * ctx = malloc(sizeof(phevRegisterCtx_t));

    ctx->pipe = settings.pipe;
    ctx->complete = settings.complete;

    memcpy(ctx->mac,settings.mac,MAC_ADDR_SIZE);

    ctx->pipe->ctx = ctx;
    ctx->startAck = false;
    ctx->aaAck = false;
    ctx->registrationRequest = false;
    ctx->ecu = false;
    ctx->remoteSecurity = false;
    ctx->registrationAck = false;
    ctx->registrationComplete = false;
        

    phev_pipe_registerEventHandler(settings.pipe, settings.eventHandler);

    return ctx;
}
void phev_register_sendRegister(phev_pipe_ctx_t * ctx)
{
    LOG_V(TAG,"START - sendRegister");
    phevMessage_t * reg = phev_core_simpleRequestCommandMessage(KO_WF_REG_DISP_SP,1);
    message_t * message = phev_core_convertToMessage(reg);

    msg_pipe_outboundPublish(ctx->pipe,  message);
//    free(message);
    LOG_V(TAG,"END - sendRegister");
    
}
int phev_register_eventHandler(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event)
{
    phevRegisterCtx_t * regCtx = (phevRegisterCtx_t *) ctx->ctx;

    if(regCtx->registrationComplete) 
    {
        return 0;
    }

    switch(event->event) 
    {
        case PHEV_PIPE_GOT_VIN: {
            LOG_I(TAG,"Got VIN");
            phev_register_sendMac(ctx);
            break;
        }
        case PHEV_PIPE_START_ACK: {
            LOG_I(TAG,"Start acknowledged");
            regCtx->startAck = true;
            //phev_register_sendRegister(ctx);
            break;
        }
        case PHEV_PIPE_AA_ACK: {
            LOG_I(TAG,"AA acknowledged");
            regCtx->aaAck = true;
            break;
        }
        case PHEV_PIPE_REGISTRATION: {
            LOG_I(TAG,"Registration");
            regCtx->registrationRequest = true;
            
            break;
        }
        case PHEV_PIPE_ECU_VERSION2: {
            LOG_I(TAG,"ECU version");
            regCtx->ecu = true;
            
            break;
        };
        case PHEV_PIPE_REMOTE_SECURTY_PRSNT_INFO: {
            LOG_I(TAG,"Remote security present info");
            regCtx->remoteSecurity = true;
            
            phev_register_sendRegister(ctx);
            break;
        }
        case PHEV_PIPE_REG_DISP: {
            LOG_I(TAG,"Registration Acknowledged");

            regCtx->registrationAck = true;
            
            break;
        }
        default : {
            LOG_W(TAG, "Unknown event %d\n",event->event);
        }
    }
    if(regCtx->startAck &&
        regCtx->aaAck &&
        regCtx->registrationRequest &&
        regCtx->ecu &&
        regCtx->remoteSecurity &&
        regCtx->registrationAck)
    {
        LOG_I(TAG,"Registration Complete");
        regCtx->registrationComplete = true;
        if(((phevRegisterCtx_t *) ctx->ctx)->complete != NULL)
        {
                LOG_D(TAG,"Calling callback");

                ((phevRegisterCtx_t *) ctx->ctx)->complete();
        }
    }
}
