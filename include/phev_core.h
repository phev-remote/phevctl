#ifndef _PHEV_CORE_H_
#define _PHEV_CORE_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "msg_core.h"
#define PHEV_OK 0

#define REQUEST_TYPE 0
#define RESPONSE_TYPE 1

#define DEFAULT_CMD_LENGTH 4

#define PING_SEND_CMD 0xf9
#define PING_RESP_CMD 0x9f

#define START_SEND 0xf2
#define START_RESP 0x2f
#define SEND_CMD 0xf6
#define RESP_CMD 0x6f

#define VIN_LEN 17
#define MAC_ADDR_SIZE 6

/*
enum commands_t  {
    PING_SEND_CMD = 0xf9, 
    PING_RESP_CMD = 0x9f, 
    START_SEND = 0xf2, 
    START_RESP = 0x2f, 
    SEND_CMD = 0xf6,
    RESP_CMD = 0x6f 
};
*/

typedef struct phevMessage_t
{
    uint8_t command;
    uint8_t length;
    uint8_t type;
    uint8_t reg;
    uint8_t *data;
    uint8_t checksum;
} phevMessage_t;

phevMessage_t * phev_core_createMessage(uint8_t command, uint8_t type, uint8_t reg, uint8_t * data, size_t length);

void phev_core_destroyMessage(phevMessage_t * message);

int phev_core_decodeMessage(const uint8_t *data, size_t len, phevMessage_t *message);

int phev_core_encodeMessage(phevMessage_t *message,uint8_t **data);

message_t * phev_core_extractMessage(const uint8_t *data, size_t len);

phevMessage_t *phev_core_requestMessage(uint8_t command, uint8_t reg, uint8_t *data, size_t length);

phevMessage_t *phev_core_responseMessage(uint8_t command, uint8_t reg, uint8_t *data, size_t length);

phevMessage_t *phev_core_simpleResponseCommandMessage(uint8_t reg, uint8_t value);

phevMessage_t *phev_core_simpleRequestCommandMessage(uint8_t reg, uint8_t value);

phevMessage_t *phev_core_simpleResponseCommandMessage(uint8_t reg, uint8_t value);

phevMessage_t *phev_core_commandMessage(uint8_t reg, uint8_t *data, size_t length);

phevMessage_t *phev_core_ackMessage(uint8_t command, uint8_t reg);

phevMessage_t *phev_core_startMessage(uint8_t *mac);

message_t *phev_core_startMessageEncoded(uint8_t *mac);

phevMessage_t *phev_core_pingMessage(uint8_t number);

phevMessage_t *phev_core_responseHandler(phevMessage_t * message);

uint8_t phev_core_checksum(const uint8_t * data); 

message_t * phev_core_convertToMessage(phevMessage_t * message);

static char * phev_core_strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    //if(s == NULL) return NULL;
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
}

#endif