#ifndef _PHEV_RESPONSE_HANDLER_H_
#define _PHEV_RESPONSE_HANDLER_H_
#include "msg_core.h"
#include "msg_utils.h"
#include "phev_core.h"

#define DEVICE_STATE_TOPIC "my-device-state"
#define DEVICE_EVENT_TOPIC "/devices/my-device2/events"

message_t * phev_response_handler(void * ctx, phevMessage_t *message);
message_t * phev_response_incomingHandler(void * ctx, message_t *message);
#endif