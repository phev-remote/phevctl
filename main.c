#include <stdio.h>
#include <argp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define LOG_LEVEL LOG_NONE

#include "phev.h"
#ifdef MQTT_PAHO
#include "msg_mqtt_paho.h"
#endif
const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "jamie@wattu.com";
static char doc[] = "Programe to .";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = { 
    { "mac", 'm', 0,1, "MAC address.",0},
    { "init", 'i', 0,0, "Initialise and register with the car - car must be in registration mode.",0},
    { "host", 'h', 0,1, "IP address of car - defaults to 192.168.8.46.",0},
    { "port", 'p', 0,1, "Port to use - defaults to 8080",0},
    { "uri", 'u',0,1,"URI for MQTT server",0},
    { 0 } 
};

struct arguments {
    bool init;
    char * host;
    uint8_t * mac;
    int port;
    char * uri;
};

uint8_t DEFAULT_MAC[] = {0,0,0,0,0,0};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'p': {
        uint16_t port = 8080;
        if(arg != NULL) {
           port = atoi(arg);
        }
        arguments->port = port;
        break;
    }
    case 'm': {
        if(arg != NULL)
        {
            if (strlen(arg) == 17)
            {
                uint8_t *mac = malloc(6);
                sscanf(arg, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
                arguments->mac = mac;
                break;
            }
        }
        arguments->mac = DEFAULT_MAC;
        break;
    } 
    case 'h': {
        if(arg !=NULL) 
        {
            arguments->host = strdup(arg);
        }
        break;
    }
    case 'i': 
        arguments->init = true;
        break;
    case 'u': {
        if(arg !=NULL)
        {
            arguments->uri = strdup(arg);
        }
        break;
    }
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static int main_eventHandler(phevEvent_t * event)
{
    
    switch (event->type)
    {
        case PHEV_REGISTER_UPDATE: 
        {
#ifdef DISPLAY_REGS
            printf("Register : %d Data :",event->reg);
            for(int i=0;i<event->length;i++)
            {
                printf("%d ",event->data[i]);
            }
            printf("\n");
#endif
            return 0;
        }
    
        case PHEV_REGISTRATION_COMPLETE: 
        {
            printf("Registration Complete\n");
            return 0;
        }
        case PHEV_CONNECTED:
        {
            printf("Connected to car\n");
            return 0;
        }
        case PHEV_START:
        {
            printf("Started\n");
            return 0;
        }
        case PHEV_VIN:
        {
            printf("VIN %s\n",event->data);
            return 0;
        }
        case PHEV_ECU_VERSION:
        {
            printf("ECU Version\n");
            return 0;
        }
        /*
            PHEV_PIPE_GOT_VIN,
    PHEV_PIPE_CONNECTED,
    PHEV_PIPE_START_ACK,
    PHEV_PIPE_REGISTRATION,
    PHEV_PIPE_ECU_VERSION2,
    PHEV_PIPE_REMOTE_SECURTY_PRSNT_INFO,
    PHEV_PIPE_REG_DISP,
    PHEV_PIPE_MAX_REGISTRATIONS,
    PHEV_PIPE_REG_UPDATE,
    PHEV_PIPE_REG_UPDATE_ACK,
    */
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void * main_thread(void * ctx)
{
    phev_start((phevCtx_t *) ctx);
}
int main(int argc, char *argv[])
{
    struct arguments arguments;
    phevCtx_t * ctx; 
    
    
    arguments.host = "192.168.8.46";
    arguments.uri = "tcp://localhost:1883";
    arguments.mac = DEFAULT_MAC;
    arguments.port = 8080;
    arguments.init = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

#ifdef MQTT_PAHO
    mqttPahoSettings_t mqtt_settings = {
        .uri = arguments.uri,
        .clientId = "client",
        .username = "user",
        .password = "password",
        .incomingTopic = "in",
        .outgoingTopic = "out",
    };
    
    messagingClient_t * client = msg_mqtt_paho_createMqttPahoClient(mqtt_settings);

    phevSettings_t settings = {
        .host = arguments.host,
        .mac = arguments.mac,
        .port = arguments.port,
        .registerDevice = arguments.init,
        .handler = main_eventHandler,
        .in = client,
    };
#else 
    phevSettings_t settings = {
        .host = arguments.host,
        .mac = arguments.mac,
        .port = arguments.port,
        .registerDevice = arguments.init,
        .handler = main_eventHandler,
    };
#endif
    printf("PHEV\n");

    if(arguments.init) 
    {
	    printf("Registering device\n");
	    printf("Host : %s\nPort : %d\nMAC : %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",arguments.host,arguments.port,arguments.mac[0],arguments.mac[1],arguments.mac[2],arguments.mac[3],arguments.mac[4],arguments.mac[5]);
	    ctx = phev_registerDevice(settings);
    } else {
	
    	printf("Host : %s\nPort : %d\nMAC : %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",arguments.host,arguments.port,arguments.mac[0],arguments.mac[1],arguments.mac[2],arguments.mac[3],arguments.mac[4],arguments.mac[5]);
    	ctx = phev_init(settings);
    }

    pthread_t main;

    int ret = pthread_create( &main, NULL, main_thread, (void*) ctx);
    
    char ch;
    do {
        ch = getchar();
    } while (ch !='x');
    
    printf("Exiting\n");
    exit(0);
    
}
