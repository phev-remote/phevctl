#include "phev_pipe.h"
#include "phev_core.h"
#include "logger.h"

const static char * APP_TAG = "PHEV_PIPE";

phev_pipe_ctx_t * phev_pipe_createPipe(phev_pipe_settings_t settings)
{
    LOG_V(APP_TAG,"START - createPipe");

    phev_pipe_ctx_t * ctx = malloc(sizeof(phev_pipe_ctx_t));

    msg_pipe_chain_t * inputChain = malloc(sizeof(msg_pipe_chain_t));
    msg_pipe_chain_t * outputChain = malloc(sizeof(msg_pipe_chain_t));

    inputChain->inputTransformer = NULL;
    inputChain->splitter = settings.inputSplitter;
    inputChain->filter = NULL;
    inputChain->outputTransformer = NULL;
    inputChain->responder = settings.inputResponder;
    inputChain->aggregator = NULL;
    inputChain->respondOnce = true;
    
    outputChain->inputTransformer = settings.outputInputTransformer;
    outputChain->splitter = settings.outputSplitter;
    outputChain->filter = NULL; 
    outputChain->outputTransformer = settings.outputOutputTransformer;
    outputChain->responder = settings.outputResponder;
    outputChain->aggregator = NULL;
    outputChain->respondOnce = false;

    msg_pipe_settings_t pipe_settings = {
        .in = settings.in,
        .out = settings.out,
        .lazyConnect = 0,
        .user_context = ctx,
        .in_chain = inputChain,
        .out_chain = outputChain,
        .preOutConnectHook = settings.preConnectHook,
    };
    
    ctx->pipe = msg_pipe(pipe_settings);

    ctx->started = settings.started;
    ctx->ctx = settings.ctx;
    
    LOG_V(APP_TAG,"END - createPipe");
    
    return ctx;
}

message_t * phev_pipe_outputChainInputTransformer(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - outputChainInputTransformer");
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));

    int length = phev_core_decodeMessage(message->data, message->length, phevMessage);
            
    if(length == 0) {
        LOG_E(APP_TAG,"Invalid message received");
        LOG_BUFFER_HEXDUMP(APP_TAG,message->data,message->length,LOG_DEBUG);
        
        return NULL;
    }

    LOG_D(APP_TAG,"Register %d Length %d Type %d",phevMessage->reg,phevMessage->length,phevMessage->type);
    LOG_BUFFER_HEXDUMP(APP_TAG,phevMessage->data,phevMessage->length,LOG_DEBUG);
    message_t * ret = phev_core_convertToMessage(phevMessage);

    phev_core_destroyMessage(phevMessage);
    
    LOG_V(APP_TAG,"END - outputChainInputTransformer");
    
    return ret;
}
message_t * phev_pipe_commandResponder(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - commandResponder");
    
    message_t * out = NULL;

    if(message != NULL) {

        phevMessage_t phevMsg;

        phev_core_decodeMessage(message->data, message->length, &phevMsg);

        if(phevMsg.command == PING_RESP_CMD) 
        {
            LOG_D(APP_TAG,"Ignoring ping");
            return NULL;
        }
        if(phevMsg.type == REQUEST_TYPE) 
        {
            phevMessage_t * msg = phev_core_responseHandler(&phevMsg);
            out = phev_core_convertToMessage(msg);
//            phev_core_destroyMessage(msg);
        }
//        free(phevMsg.data);
    }
    if(out)
    {
        LOG_D(APP_TAG,"Responding with");
        LOG_BUFFER_HEXDUMP(APP_TAG,out->data,out->length,LOG_DEBUG);
    } else {
        LOG_D(APP_TAG,"No response");
    }
    LOG_V(APP_TAG,"END - commandResponder");
    return out;
}

phevPipeEvent_t * phev_pipe_createVINEvent(uint8_t * data)
{
    LOG_V(APP_TAG,"START - createVINEvent");
    phevPipeEvent_t * event = malloc(sizeof(phevPipeEvent_t));

    if(data[19] < 3) {
        phevVinEvent_t * vinEvent = malloc(sizeof(phevVinEvent_t));
        event->event = PHEV_PIPE_GOT_VIN,
        event->data =  (uint8_t *) vinEvent;
        event->length = sizeof(phevVinEvent_t);
        memcpy(vinEvent->vin, data + 1, VIN_LEN);
        vinEvent->vin[VIN_LEN]  = 0;
        vinEvent->registrations = data[19];
    } else {
        event->event = PHEV_PIPE_MAX_REGISTRATIONS,
        event->data =  NULL;
        event->length = 0;
    }
    
    LOG_D(APP_TAG,"Created Event ID %d",event->event);
    LOG_BUFFER_HEXDUMP(APP_TAG,event->data,event->length,LOG_DEBUG);
    LOG_V(APP_TAG,"END - createVINEvent");
    
    return event;
}

phevPipeEvent_t * phev_pipe_AAResponseEvent(void)
{
    LOG_V(APP_TAG,"START - AAResponseEvent");
    phevPipeEvent_t * event = malloc(sizeof(phevPipeEvent_t));

    event->event = PHEV_PIPE_AA_ACK,
    event->data =  NULL;
    event->length = 0;
    LOG_D(APP_TAG,"Created Event ID %d",event->event);
    
    LOG_V(APP_TAG,"END - AAResponseEvent");
    
    return event;

}
phevPipeEvent_t * phev_pipe_startResponseEvent(void)
{
    LOG_V(APP_TAG,"START - startResponseEvent");
    phevPipeEvent_t * event = malloc(sizeof(phevPipeEvent_t));

    event->event = PHEV_PIPE_START_ACK,
    event->data =  NULL;
    event->length = 0;
    LOG_D(APP_TAG,"Created Event ID %d",event->event);
    
    LOG_V(APP_TAG,"END - startResponseEvent");
    
    return event;

}

phevPipeEvent_t * phev_pipe_registrationEvent(void)
{
    LOG_V(APP_TAG,"START - registrationEvent");
    phevPipeEvent_t * event = malloc(sizeof(phevPipeEvent_t));

    event->event = PHEV_PIPE_REGISTRATION,
    event->data =  NULL;
    event->length = 0;
    LOG_D(APP_TAG,"Created Event ID %d",event->event);
    
    LOG_V(APP_TAG,"END - registrationEvent");
    
    return event;

}
phevPipeEvent_t * phev_pipe_ecuVersion2Event(void)
{
    LOG_V(APP_TAG,"START - ecuVersion2Event");
    phevPipeEvent_t * event = malloc(sizeof(phevPipeEvent_t));

    event->event = PHEV_PIPE_ECU_VERSION2,
    event->data =  NULL;
    event->length = 0;
    LOG_D(APP_TAG,"Created Event ID %d",event->event);
    
    LOG_V(APP_TAG,"END - ecuVersion2Event");
    
    return event;
}
phevPipeEvent_t * phev_pipe_remoteSecurityPresentInfoEvent(void)
{
    LOG_V(APP_TAG,"START - remoteSecurityPresentInfoEvent");
    phevPipeEvent_t * event = malloc(sizeof(phevPipeEvent_t));

    event->event = PHEV_PIPE_REMOTE_SECURTY_PRSNT_INFO,
    event->data =  NULL;
    event->length = 0;
    LOG_D(APP_TAG,"Created Event ID %d",event->event);
    
    LOG_V(APP_TAG,"END - remoteSecurityPresentInfoEvent");
    
    return event;
}
phevPipeEvent_t * phev_pipe_regDispEvent(void)
{
    LOG_V(APP_TAG,"START - regDispEvent");
    phevPipeEvent_t * event = malloc(sizeof(phevPipeEvent_t));

    event->event = PHEV_PIPE_REG_DISP,
    event->data =  NULL;
    event->length = 0;
    LOG_D(APP_TAG,"Created Event ID %d",event->event);
    
    LOG_V(APP_TAG,"END - regDispEvent");
    
    return event;
}
phevPipeEvent_t * phev_pipe_messageToEvent(phev_pipe_ctx_t * ctx, phevMessage_t * phevMessage)
{
    LOG_V(APP_TAG,"START - messageToEvent");
    LOG_D(APP_TAG,"Reg %d Len %d Type %d",phevMessage->reg,phevMessage->length,phevMessage->type);
    phevPipeEvent_t * event = NULL;

    if(phevMessage->command == PING_RESP_CMD)
    {
        LOG_D(APP_TAG,"Ignoring ping");
        return NULL;
    } 
    switch(phevMessage->reg)
    {
        case KO_WF_VIN_INFO_EVR: {
            LOG_D(APP_TAG,"KO_WF_VIN_INFO_EVR");
            if(phevMessage->type == REQUEST_TYPE)
            {
                event = phev_pipe_createVINEvent(phevMessage->data);
            }
            break;
        }
        case KO_WF_CONNECT_INFO_GS_SP: {
            
            if(phevMessage->type == RESPONSE_TYPE && phevMessage->command == START_RESP)
            {
                LOG_D(APP_TAG,"KO_WF_CONNECT_INFO_GS_SP");
                event = phev_pipe_startResponseEvent();
            }
            break;
        }
        case KO_WF_START_AA_EVR: {
            
            if(phevMessage->type == RESPONSE_TYPE && phevMessage->command == RESP_CMD)
            {
                LOG_D(APP_TAG,"KO_WF_START_AA_EVR");
                event = phev_pipe_AAResponseEvent();
            }
            break;
        }
        case KO_WF_REGISTRATION_EVR: {
            if(phevMessage->type == REQUEST_TYPE && phevMessage->command == RESP_CMD)
            {
                event = phev_pipe_registrationEvent();
            }
            break;
        }
        case KO_WF_ECU_VERSION2_EVR: {
            if(phevMessage->type == REQUEST_TYPE && phevMessage->command == RESP_CMD)
            {
                event = phev_pipe_ecuVersion2Event();
            }
            break;
        }
        case KO_WF_REMOTE_SECURTY_PRSNT_INFO: {
            if(phevMessage->type == REQUEST_TYPE && phevMessage->command == RESP_CMD)
            {
                event = phev_pipe_remoteSecurityPresentInfoEvent();
            }
            break;
        }
        case KO_WF_REG_DISP_SP: {
            if(phevMessage->type == RESPONSE_TYPE && phevMessage->command == RESP_CMD)
            {
                event = phev_pipe_regDispEvent();
            }
            break;
        }
        default: {
            LOG_E(APP_TAG,"Register not handled");
        }
    }
    
    LOG_V(APP_TAG,"END - messageToEvent");
    return event;
}
void phev_pipe_sendEvent(void * ctx, phevMessage_t * phevMessage)
{
    LOG_V(APP_TAG,"START - sendEvent");
    
    phev_pipe_ctx_t * phevCtx = (phev_pipe_ctx_t *) ctx;

    if(phevCtx->eventHandler != NULL)
    {
        phevPipeEvent_t * evt = phev_pipe_messageToEvent(phevCtx,phevMessage);
        
        if(evt != NULL)
        {
            LOG_D(APP_TAG,"Sending event ID %d",evt->event);
            phevCtx->eventHandler(phevCtx, evt);    
        } else {
            LOG_D(APP_TAG,"Not sending event");
        }
    }
    
    LOG_V(APP_TAG,"END - sendEvent");
}
message_t * phev_pipe_outputEventTransformer(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - outputEventTransformer");
    
    phevMessage_t * phevMessage = malloc(sizeof(phevMessage_t));

    int length = phev_core_decodeMessage(message->data, message->length, phevMessage);
            
    if(length == 0) {
        LOG_E(APP_TAG,"Invalid message received - something serious happened here as we should only have a valid message at this point");
        LOG_BUFFER_HEXDUMP(APP_TAG,message->data,message->length,LOG_DEBUG);
        
        return NULL;
    }
    
    phev_pipe_sendEvent(ctx, phevMessage);
        
    message_t * ret = phev_core_convertToMessage(phevMessage);

    phev_core_destroyMessage(phevMessage);
    
    LOG_V(APP_TAG,"END - outputEventTransformer");

    return ret;
}

void phev_pipe_registerEventHandler(phev_pipe_ctx_t * ctx, phevPipeEventHandler_t eventHandler) 
{
    ctx->eventHandler = eventHandler;
}
void phev_pipe_deregisterEventHandler(phev_pipe_ctx_t * ctx, phevPipeEventHandler_t eventHandler)
{
    ctx->eventHandler = NULL;
}

messageBundle_t * phev_pipe_outputSplitter(void * ctx, message_t * message)
{
    LOG_V(APP_TAG,"START - outputSplitter");
    
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
    LOG_V(APP_TAG,"END - outputSplitter");
    return messages;
}
