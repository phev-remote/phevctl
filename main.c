#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define VERSION "0.0.1"
#define LOG_LEVEL LOG_NONE

#include "phev.h"
#include "phevargs.h"
#include "msg_utils.h"
#ifdef MQTT_PAHO
#include "msg_mqtt_paho.h"
#endif

char *remaining_args = NULL, num_remaining_args = 0;

bool bool_value;
uint8_t uint_value;

int wait_for_regs = 0;

static void operationCallback(phevCtx_t *ctx, void *value)
{
    printf("Operation successful\n");
    phev_exit(ctx);
    exit(0);
}
static void operationCallbackNoExit(phevCtx_t *ctx, void *value)
{
    printf("Operation successful\n");
}

static int main_eventHandler(phevEvent_t *event)
{
    phevCtx_t *ctx = event->ctx;

    phev_args_opts_t *opts = (phev_args_opts_t *)phev_getUserCtx(ctx);

    switch (event->type)
    {
    case PHEV_REGISTER_UPDATE:
    {
        printf("Register %02X\n",event->reg);
        for (int i = 0; i < event->length; i++)
        {
            printf("%02X ", event->data[i]);
        }
        printf("\n");
        if (opts->verbose)
        {
            if (event->reg == KO_WF_DATE_INFO_SYNC_EVR)
            {
                printf("Date sync 20%d-%d-%d %d:%0d:%0d\n", event->data[0], event->data[1], event->data[2], event->data[3], event->data[4], event->data[5]);
            }
        }
        switch (opts->command)
        {
        case CMD_BATTERY:
        {
            if (event->reg == KO_WF_BATT_LEVEL_INFO_REP_EVR)
            {
                int batt = phev_batteryLevel(ctx);
                if (batt < 0)
                {
                    return 0;
                }
                printf("Battery level %d\n", batt);
                exit(0);
            }
            break;
        }
        case CMD_ISLOCKED:
        {
            if (event->reg == KO_WF_DOOR_STATUS_INFO_REP_EVR)
            {
                int islocked = phev_isLocked(ctx);
                if (islocked < 0)
                {
                printf("Doors in UNKNOWN STATE\n");
                } else if (islocked == 1){
                printf("Doors are Locked\n");
                } else{
                printf("Doors are UnLocked\n");
                }
                exit(0);
            }
            break;
        }
        case CMD_CHARGING_STATUS:
        {
            if (event->reg == KO_WF_OBCHG_OK_ON_INFO_REP_EVR)
            {
                int chargeStatus = phev_chargingStatus(ctx);
                if (chargeStatus < 0)
                {
                    return 0;
                }
                printf("Charge status %d\n", chargeStatus);
                exit(0);
            }
            break;
        }
        case CMD_HVAC_STATUS:
        {
            if (event->reg == KO_WF_TM_AC_STAT_INFO_REP_EVR)
            {
                phevServiceHVAC_t * ph =  phev_HVACStatus(ctx);

                printf("{\"operating\":%d,\"mode\":%d}",  ph->operating, ph->mode);
                exit(0);
            }
            break;
        }
        case CMD_REMAINING_CHARGING_STATUS:
        {
            if (event->reg == KO_WF_OBCHG_OK_ON_INFO_REP_EVR)
            {
                int remainingChargeStatus = phev_remainingChargeTime(ctx);
                if (remainingChargeStatus < 0)
                {
                    return 0;
                }
                printf("Remaining Charge status %d\n", remainingChargeStatus);
                exit(0);
            }
            break;
        }
        case CMD_DISPLAY_REG:
        {
            printf("Register : %d Data :", event->reg);
            for (int i = 0; i < event->length; i++)
            {
                printf("%02X ", event->data[i]);
            }
            printf("\n");
            break;
        }
        case CMD_GET_REG_VAL:
        {
            phevData_t *reg = phev_getRegister(ctx, opts->reg_operand);
            if (reg == NULL)
            {
                if (wait_for_regs > WAIT_FOR_REG_MAX)
                {
                    printf("REGISTER TIMEOUT\n");
                    exit(0);
                }
                wait_for_regs++;
                return 0;
            }
            printf("Get register %d : ", opts->reg_operand);
            for (int i = 0; i < reg->length; i++)
            {
                printf("%02X ", reg->data[i]);
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
    }
    case PHEV_CONNECTED:
    {
        return 0;
    }
    case PHEV_STARTED:
    {
        printf("Connected to car successfully\n");
        return 0;
    }
    case PHEV_VIN:
    {
        if (opts->verbose)
        {
            printf("VIN number : %s\n", event->data);
        }
        if (opts->command != CMD_UNSET && opts->command != CMD_INVALID)
        {
            switch (opts->command)
            {
            case CMD_HEADLIGHTS:
            {
                printf("Turning %s headlights\n", opts->operand_on ? "ON" : "OFF");
                phev_headLights(event->ctx, opts->operand_on, operationCallback);
                break;
            }
            case CMD_PARKING_LIGHTS:
            {
                printf("Turning %s parking lights\n", opts->operand_on ? "ON" : "OFF");
                phev_parkingLights(event->ctx, opts->operand_on, operationCallback);
                break;
            }
            case CMD_AIRCON:
            {
                printf("Turning air conditioning %s\n", opts->operand_on ? "ON" : "OFF");
                phev_airCon(event->ctx, opts->operand_on, operationCallback);
                break;
            }
            case CMD_UPDATE:
            {
                printf("Update All\n");
                phev_updateAll(event->ctx, operationCallback);
                break;
            }
            case CMD_AIRCON_MODE:
            {
                printf("Switching air conditioning mode to %d for %d mins\n", opts->operand_mode, opts->operand_time);
                if (opts->verbose)
                {
                    printf("Car Model: %d\n", opts->carModel);
                }
                if ( opts->carModel == 2019){
                    phev_airConMY19(event->ctx, opts->operand_mode, opts->operand_time, operationCallback);
                } else {
                    phev_airConMode(event->ctx, opts->operand_mode, opts->operand_time, operationCallback);
                }

                break;
            }
            }
        }
        return 0;
    }
    case PHEV_ECU_VERSION:
    {
        if (opts->verbose)
        {
            printf("ECU Version : %s\n", event->data);
        }
        return 0;
    }
    case PHEV_REGISTER_UPDATE_ACK:
        if(opts->verbose)
        {
            printf("Register %d Acknowledged\n",event->reg);
        }
        return 0;
    }

    return 0;
}

void *main_thread(void *ctx)
{
    phev_start((phevCtx_t *)ctx);
}
void print_intro()
{
    printf("Mitsubishi Outlander PHEV Remote CLI - ");
    printf("Designed and coded by Jamie Nuttall 2020\nMIT License\n\n");
    printf("Type 'x' then enter to quit.\n");
}

int main(int argc, char *argv[])
{
    phevCtx_t *ctx = NULL;

    phev_args_opts_t *opts = phev_args_parse(argc, argv);

    if (opts == NULL)
    {
        printf("ERROR Options could not be parsed");
        exit(1);
    }
    if (opts->error)
    {
        printf("ERROR %s\n", opts->error_message);
        exit(1);
    }
    phevSettings_t settings = {
        .host = opts->host,
        .mac = opts->mac,
        .port = opts->port,
        .registerDevice = (opts->command == CMD_REGISTER ? true : false),
        .handler = main_eventHandler,
        .ctx = (void *)opts,
    };

    print_intro();

    if (opts->command == CMD_REGISTER)
    {
        printf("Registering device\n");
        printf("Host : %s\nPort : %d\nMAC : %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", opts->host, opts->port, opts->mac[0], opts->mac[1], opts->mac[2], opts->mac[3], opts->mac[4], opts->mac[5]);
        ctx = phev_registerDevice(settings);
    }
    else
    {
        if (opts->verbose)
        {
            printf("Host : %s\nPort : %d\nMAC : %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", opts->host, opts->port, opts->mac[0], opts->mac[1], opts->mac[2], opts->mac[3], opts->mac[4], opts->mac[5]);
        }
        ctx = phev_init(settings);
    }

    pthread_t main;

    if (opts->command == CMD_UNSET)
    {
        printf("No command exiting.\n");
        exit(0);
    }
    if (opts->command == CMD_INVALID)
    {
        printf("Error %s.\n", opts->error_message);
        exit(0);
    }
    int ret = pthread_create(&main, NULL, main_thread, (void *)ctx);
    //main_thread((void *) ctx);
    char ch;
    do
    {
        ch = getchar();

        switch(ch)
        {
        case 'r':
        {
            printf("Disconnecting\n");
            phev_disconnect(ctx);
            break;
        }
        case 'l':
        {
            printf("Lights on\n");
            phev_headLights(ctx,true,operationCallbackNoExit);
            break;
        }
        case 'a':
        {
            printf("Aircon on\n");
            phev_airCon(ctx, true, operationCallbackNoExit);
        }
        }

    } while (ch != 'x' && phev_running(ctx));

    phev_disconnect(ctx);

    printf("Exiting\n");
    exit(0);
}
