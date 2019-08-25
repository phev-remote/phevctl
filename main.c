#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define VERSION "0.0.1"
#define LOG_LEVEL LOG_NONE

#include "phev.h"
#ifdef MQTT_PAHO
#include "msg_mqtt_paho.h"
#endif

char * remaining_args = NULL, num_remaining_args= 0;

bool bool_value;
uint8_t uint_value;

int wait_for_regs = 0;

static void operationCallback(phevCtx_t * ctx, void * value)
{
    printf("Operation successful\n");
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
/*    
    switch (event->type)
    {
        case PHEV_REGISTER_UPDATE: 
        {
//#ifdef DISPLAY_REGS

//#endif
            if(arguments.verbose)
            {
                if(event->reg ==  KO_WF_DATE_INFO_SYNC_EVR)
                {
                    printf("Date sync 20%d-%d-%d %d:%0d:%0d\n",event->data[0],event->data[1],event->data[2],event->data[3],event->data[4],event->data[5]);
                }
            }
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
                case CMD_DISPLAY_REG:
                {
                    printf("Register : %d Data :",event->reg);
                    for(int i=0;i<event->length;i++)
                    {
                        printf("%02X ",event->data[i]);
                    }
                    printf("\n");
                    break;
                }
                case CMD_GET_REG_VAL: {
                    phevData_t * reg = phev_getRegister(event->ctx, uint_value);
                    if(reg == NULL)
                    {
                        if(wait_for_regs > WAIT_FOR_REG_MAX)
                        {
                            printf("REGISTER TIMEOUT\n");
                            exit(0);
                        }
                        wait_for_regs ++;
                        return 0;
                    }
                    printf("Get register %d : ",uint_value);
                    for(int i=0;i<reg->length;i++)
                    {
                        printf("%02X ",reg->data[i]);
                    }
                    printf("\n");
                    exit(0);
                    break;
                }
            }
            return 0;
        }
    
        case PHEV_REGISTRATION_COMPLETE: 
        {
            printf("Registration Complete\n");
            exit(0);
            return 0;
        }
        case PHEV_CONNECTED:
        {
            return 0;
        }
        case PHEV_STARTED:
        {
            printf("Started\n");
            return 0;
        }
        case PHEV_VIN:
        {
            if(arguments.verbose)
            {
                printf("VIN number : %s\n",event->data);
            }
            
            return 0;
        }
        case PHEV_ECU_VERSION:
        {
            if(arguments.verbose)
            {
                printf("ECU Version : %s\n",event->data);
            }
            if(command != CMD_UNSET)
            {
                switch(command)
                {
                    case CMD_HEADLIGHTS: {
                        printf("Turning head lights %s : ",(bool_value?"ON":"OFF"));
                        phev_headLights(event->ctx, bool_value, operationCallback);        
                        break;
                    }
                    case CMD_AIRCON: {
                        printf("Turning air conditioning %s : ",(bool_value?"ON":"OFF"));
                        phev_airCon(event->ctx, bool_value, operationCallback);        
                        break;
                    }
                }
            }
            return 0;
        }

    }
    */
    return 0;
}

void * main_thread(void * ctx)
{
    phev_start((phevCtx_t *) ctx);
}
void print_intro()
{
    printf("Mitsubishi Outlander PHEV Remote CLI - ");
    printf("Designed and coded by Jamie Nuttall 2019\nMIT License\n\n");
    printf("Type 'x' then enter to quit.\n");
    
}
int main(int argc, char *argv[])
{
    
    phevCtx_t * ctx; 
    
    printf("argc %d\n",argc);
    for(int i =0;i< argc;i++)
    {
        printf("%s\n",argv[i]);
    }
    exit(0);
/*
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
//        .incomingTopic = arguments.command_topic,
//        .outgoingTopic = arguments.topic,
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

    print_intro();

    if(command == CMD_REGISTER) 
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
    */
    exit(0);
    
}
