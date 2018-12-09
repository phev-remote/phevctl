#include "phev_pipe.h"
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
        .lazyConnect = 1,
        .user_context = ctx,
        .in_chain = inputChain,
        .out_chain = outputChain,
        .preOutConnectHook = settings.preConnectHook,
    };
    
    ctx->pipe = msg_pipe(pipe_settings);

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
    message_t * ret = phev_core_convertToMessage(phevMessage);

    phev_core_destroyMessage(phevMessage);
    
    LOG_V(APP_TAG,"END - outputChainInputTransformer");
    
    return ret;
}

void phev_pipe_sendEvent(void * ctx, phevPipeEvent_t * event)
{
    LOG_V(APP_TAG,"START - sendEvent");
    phev_pipe_ctx_t * phevCtx = (phev_pipe_ctx_t *) ctx;

    if(phevCtx->eventHandler != NULL)
    {
        phevPipeEvent_t * evt = malloc(sizeof(phevPipeEvent_t));
        
        evt->event = event->event;
        evt->data =  malloc(VIN_LEN + 1);
        evt->length = VIN_LEN + 1;

        memcpy(evt->data, event->data + 1, VIN_LEN);
        
        evt->data[VIN_LEN]  = 0;

        phevCtx->eventHandler(phevCtx, evt);
    }
    
    LOG_V(APP_TAG,"END - sendEvent");
    
    //printf()
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
    
    uint8_t * data = malloc(phevMessage->length);
    memcpy(data, phevMessage->data,phevMessage->length); 
    phevPipeEvent_t event = {
        .event = PHEV_PIPE_GOT_VIN,
        .data = data,
        .length = phevMessage->length,
    };
    
    phev_pipe_sendEvent(ctx, &event);
    
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
