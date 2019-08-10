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
static char args_doc[] = "COMMAND [VALUES]...";
static struct argp_option options[] = { 
    { "mac", 'm', "<MAC ADDRESS>",0, "MAC address."},
    { "init", 'i', 0,0, "Initialise and register with the car - car must be in registration mode."},
    { "host", 'h', "<HOST NAME>",0, "IP address of car - defaults to 192.168.8.46."},
    { "port", 'p', "<PORT NUMBER>",0, "Port to use - defaults to 8080"},
    { "uri", 'u',"<URI>",0,"URI for MQTT server"},
    { "topic",'t',"<TOPIC NAME>",0,"MQTT output topic"},
    { "cmdtopic",'c',"<COMMAND TOPIC NAME>",0,"MQTT command topic"},
    { "verbose",'v',0,0,"Verbose"},
    { 0 } 
};

struct arguments {
    bool init;
    char * host;
    uint8_t * mac;
    int port;
    char * uri;
    char * topic;
    char * command_topic;
    bool verbose;
};

uint8_t DEFAULT_MAC[] = {0,0,0,0,0,0};
char * remaining_args = NULL, num_remaining_args= 0;

#define HEADLIGHTS "headlights"
#define BATTERY "battery"
#define ON "on"
#define OFF "off"

enum commands { CMD_UNSET, CMD_HEADLIGHTS, CMD_BATTERY };

enum commands command = CMD_UNSET;
bool bool_value;

int process_command(char * arg, int arg_num)
{
    if(strcmp(arg,HEADLIGHTS) == 0 && arg_num == 0)
    {
        command = CMD_HEADLIGHTS;
    }
    if(strcmp(arg,BATTERY) == 0 && arg_num == 0)
    {
        command = CMD_BATTERY;
    }
    if(strcmp(arg,ON) == 0 && arg_num == 1)
    {
        bool_value = true;
    }
    if(strcmp(arg,OFF) == 0 && arg_num == 1)
    {
        bool_value = false;
    } 
    return 0;
}
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
    case 't': {
        if(arg !=NULL)
        {
            arguments->topic = strdup(arg);
        }
        break;
    }
    case 'c': {
        if(arg !=NULL)
        {
            arguments->command_topic = strdup(arg);
        }
        break;
    }
    case 'v': {
        arguments->verbose = true;
        break;
    }
    case ARGP_KEY_END:
        if(state->arg_num <2)
        {
            argp_usage(state);
        } 
        break;
    case ARGP_KEY_ARG: 
        if(state->arg_num >=2)
        {
            argp_usage(state);
        } else {
            if(process_command(strdup(arg), state->arg_num))
            {
                argp_usage(state);
            }
            
        }
        break;
    return 0;
    default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

static void headLightCallback(phevCtx_t * ctx, void * value)
{
    printf("Head light operation successful\n");
    phev_exit(ctx);
    exit(0);

}
static void batteryLevelCallback(phevCtx_t * ctx, void * level)
{
    printf("Battery %d\n",*((int *) level));
    phev_exit(ctx);
    exit(0);
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
            switch(command)
            {
                case CMD_BATTERY: 
                {
                    if(event->reg == KO_WF_BATT_LEVEL_INFO_REP_EVR)
                    {
                        int batt = phev_batteryLevel(event->ctx);
                        if(batt < 0)
                        {
                            return 0;
                        }
                        printf("Battery level %d\n",batt);
                        exit(0);
                    }
                    break;
                }
            }
            return 0;
        }
    
        case PHEV_REGISTRATION_COMPLETE: 
        {
            printf("Registration Complete\n");
            return 0;
        }
        case PHEV_CONNECTED:
        {
            return 0;
        }
        case PHEV_START:
        {
            printf("Started\n");
            return 0;
        }
        case PHEV_VIN:
        {
            //printf("VIN %s\n",event->data);
            return 0;
        }
        case PHEV_ECU_VERSION:
        {
            //printf("ECU Version\n");

            if(command != CMD_UNSET)
            {
                switch(command)
                {
                    case CMD_HEADLIGHTS: {
                        printf("Turning head lights %s\n",(bool_value?"ON":"OFF"));
                        phev_headLights(event->ctx, bool_value, headLightCallback);        
                        break;
                    }
                }
                //printf("Process command %d %s\n",command, (bool_value?"ON":"OFF"));
                //phev_exit(event->ctx);
                //exit(0);
            }
            return 0;
        }

    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void * main_thread(void * ctx)
{
    phev_start((phevCtx_t *) ctx);
}
void print_intro()
{
    printf("Mitsubishi Outlander PHEV Remote CLI\n");
    printf("Designed and coded by Jamie Nuttall 2019\n");

}
int main(int argc, char *argv[])
{
    print_intro();
    struct arguments arguments;
    phevCtx_t * ctx; 
    
    
    arguments.host = "192.168.8.46";
    arguments.uri = "tcp://localhost:1883";
    arguments.mac = DEFAULT_MAC;
    arguments.port = 8080;
    arguments.init = false;
    arguments.command_topic = "defaultin";
    arguments.topic = "defaultout";
    arguments.verbose = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);


#ifdef MQTT_PAHO
    printf("MQTT config\n");
    printf("URI %s\nIncoming Topic %s\nOutgoing Topic %s\n",arguments.uri,arguments.command_topic,arguments.topic);
    
    mqttPahoSettings_t mqtt_settings = {
        .uri = arguments.uri,
        .clientId = "client",
        .username = "user",
        .password = "password",
        .incomingTopic = arguments.command_topic,
        .outgoingTopic = arguments.topic,
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

    if(arguments.init) 
    {
	    printf("Registering device\n");
	    printf("Host : %s\nPort : %d\nMAC : %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",arguments.host,arguments.port,arguments.mac[0],arguments.mac[1],arguments.mac[2],arguments.mac[3],arguments.mac[4],arguments.mac[5]);
	    ctx = phev_registerDevice(settings);
    } else {
        if(arguments.verbose)
        {
            printf("Host : %s\nPort : %d\nMAC : %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",arguments.host,arguments.port,arguments.mac[0],arguments.mac[1],arguments.mac[2],arguments.mac[3],arguments.mac[4],arguments.mac[5]);
        }
    	ctx = phev_init(settings);
    }

    pthread_t main;

    int ret = pthread_create( &main, NULL, main_thread, (void*) ctx);
    
    char ch;
    do {
        ch = getchar();
    } while (ch !='x' && phev_running(ctx));
    
    printf("Exiting\n");
    exit(0);
    
}
