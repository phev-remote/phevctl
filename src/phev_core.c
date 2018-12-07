#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "phev_core.h"
#include "msg_core.h"
#include "msg_utils.h"
#include "logger.h"

const static char *APP_TAG = "PHEV_CORE";

const uint8_t allowedCommands[] = {0xf2, 0x2f, 0xf6, 0x6f, 0xf9, 0x9f};

phevMessage_t * phev_core_createMessage(uint8_t command, uint8_t type, uint8_t reg, uint8_t * data, size_t length)
{
    LOG_V(APP_TAG,"START - createMessage");
    LOG_D(APP_TAG,"Data %d Length %d",data[0],length);
    phevMessage_t * message = malloc(sizeof(phevMessage_t));

    message->command = command;
    message->type = type;
    message->reg = reg;
    message->length = length;
    message->data = malloc(message->length);
    memcpy(message->data, data, length);
    
    LOG_D(APP_TAG,"Message Data %d",message->data[0]);
    
    LOG_V(APP_TAG,"END - createMessage");
    
    return message;
}

void phev_core_destroyMessage(phevMessage_t * message) 
{
    LOG_V(APP_TAG,"START - destroyMessage");
    
    if(message == NULL) return;

    if(message->data != NULL)
    {
        free(message->data);
    }
    free(message);
    LOG_V(APP_TAG,"END - destroyMessage");
    
}
int phev_core_validate_buffer(uint8_t * msg, size_t len)
{
    LOG_V(APP_TAG,"START - validateBuffer");
    
    for(int i = 0;i < sizeof(allowedCommands); i++)
    {
        if(msg[0] == allowedCommands[i])
        {
            if((msg[1] + 2) > len)
            {
                return 0;  // length goes past end of message
            }
            return 1; //valid message
        }
    }
    LOG_V(APP_TAG,"END - validateBuffer");
    
    return 0;  // invalid command
}
int phev_core_decodeMessage(const uint8_t *data, const size_t len, phevMessage_t *msg)
{
    LOG_V(APP_TAG,"START - decodeMessage");
    
    if(phev_core_validate_buffer(data, len) != 0)
    {

        msg->command = data[0];
        msg->length = data[1];
        msg->type = data[2];
        msg->reg = data[3];
        msg->data = malloc(msg->length - 3);
        if(msg->length > 3) 
        {
            memcpy(msg->data, data + 4, msg->length - 3);
        } else {
            msg->data = NULL;
        }
        msg->checksum = data[5 + msg->length];
        
        LOG_D(APP_TAG,"Command %d Length %d type %d reg %d",msg->command,msg->length ,msg->type,msg->reg);
        if(msg->data != NULL)
        {
            LOG_BUFFER_HEXDUMP(APP_TAG,msg->data,msg->length - 3,LOG_DEBUG);
        }
        
        LOG_V(APP_TAG,"END - decodeMessage");
        
        return msg->length + 2;
    } else {
        LOG_E(APP_TAG,"INVALID MESSAGE");
        LOG_BUFFER_HEXDUMP(APP_TAG,data,len,LOG_DEBUG);
        
        LOG_V(APP_TAG,"END - decodeMessage");
        return 0;
    }
}
message_t * phev_core_extractMessage(const uint8_t *data, const size_t len)
{
    LOG_V(APP_TAG,"START - extractMessage");
    
    if(phev_core_validate_buffer(data, len) != 0)
    {
        
        message_t * message = msg_utils_createMsg(data,len);

        LOG_V(APP_TAG,"END - extractMessage");
    
        return message;
    } else {
        LOG_E(APP_TAG,"Invalid Message");
        
        LOG_V(APP_TAG,"END - extractMessage");
        return NULL;    
    }
}

int phev_core_encodeMessage(phevMessage_t *message,uint8_t ** data)
{
    LOG_V(APP_TAG,"START - encodeMessage");
        
    uint8_t * d = malloc(message->length + 5);

    d[0] = message->command;
    d[1] = message->length +3;
    d[2] = message->type;
    d[3] = message->reg;
    if(message->data != NULL) 
    {
        memcpy(d + 4, message->data, message->length );
    }
    d[message->length + 4] = phev_core_checksum(d);

    *data = d;
    LOG_D(APP_TAG,"Created message");
    LOG_BUFFER_HEXDUMP(APP_TAG,d,d[1] +2,LOG_DEBUG);
    LOG_V(APP_TAG,"END - encodeMessage");
        
    return d[1] + 2;
}

phevMessage_t *phev_core_message(uint8_t command, uint8_t type, uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_createMessage(command, type, reg, data, length);
}
phevMessage_t *phev_core_responseMessage(uint8_t command, uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_message(command, RESPONSE_TYPE, reg, data, length);
}
phevMessage_t *phev_core_requestMessage(uint8_t command, uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_message(command, REQUEST_TYPE, reg, data, length);
}
phevMessage_t *phev_core_commandMessage(uint8_t reg, uint8_t *data, size_t length)
{
    return phev_core_requestMessage(SEND_CMD, reg, data, length);
}
phevMessage_t *phev_core_simpleRequestCommandMessage(uint8_t reg, uint8_t value)
{
    const uint8_t data = value;
    return phev_core_requestMessage(SEND_CMD, reg, &data, 1);
}
phevMessage_t *phev_core_simpleResponseCommandMessage(uint8_t reg, uint8_t value)
{
    const uint8_t data = value;
    return phev_core_responseMessage(SEND_CMD, reg, &data, 1);
}
phevMessage_t *phev_core_ackMessage(uint8_t command, uint8_t reg)
{
    const uint8_t data = 0;
    return phev_core_responseMessage(command, reg, &data, 1);
}
phevMessage_t *phev_core_startMessage(uint8_t pos, uint8_t *mac)
{
    uint8_t * data = malloc(7);
    data[0] = pos;
    memcpy(data + 1,mac, 6);
    return phev_core_requestMessage(START_SEND, 0x01, data, 7);
}
message_t *phev_core_startMessageEncoded(uint8_t pos, uint8_t *mac)
{
    phevMessage_t * start = phev_core_startMessage(pos, mac);
    phevMessage_t * startaa = phev_core_simpleRequestCommandMessage(0xaa,0);
    message_t * message = msg_utils_concatMessages(
                                    phev_core_convertToMessage(start),
                                    phev_core_convertToMessage(startaa)
                                );
    phev_core_destroyMessage(start);
    phev_core_destroyMessage(startaa);
    return message;
}
phevMessage_t *phev_core_pingMessage(uint8_t number)
{
    const uint8_t data = 0;
    return phev_core_requestMessage(PING_SEND_CMD, number, &data, 1);
}
phevMessage_t *phev_core_responseHandler(phevMessage_t * message)
{
    uint8_t command = ((message->command & 0xf) << 4) | ((message->command & 0xf0) >> 4);
    return phev_core_ackMessage(command, message->reg); 
}

uint8_t phev_core_checksum(const uint8_t * data) 
{
    uint8_t b = 0;
    int j = data[1] + 2;
    for (int i = 0;; i++)
    {
      if (i >= j - 1) {
        return b;
      }
      b = (uint8_t)(data[i] + b);
    }
}
message_t * phev_core_convertToMessage(phevMessage_t *message)
{
    LOG_V(APP_TAG,"START - convertToMessage");
        
    uint8_t * data = NULL;   
    
    size_t length = phev_core_encodeMessage(message, &data);

    message_t * out = msg_utils_createMsg(data,length);

    free(data);

    LOG_V(APP_TAG,"END - convertToMessage");
        
    return out;
}