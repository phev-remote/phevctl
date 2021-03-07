#include <stddef.h>
#include "phevargs.h"

int phev_args_validate(int arg_num,phev_args_opts_t * opts)
{
    switch (opts->command)
    {
        case CMD_HEADLIGHTS:
        case CMD_PARKING_LIGHTS:
        case CMD_AIRCON:
        {
            if(arg_num == 2)
            {
                return 0;
            }
            break;
        }
        case CMD_AIRCON_MODE:
        {
            if(arg_num == 3)
            {
                return 0;
            }
            break;
        }
        case CMD_REGISTER:
        {
            if(arg_num == 1)
            {
                return 0;
            }
            break;
        }
        case CMD_GET_REG_VAL:
        {
            if(arg_num == 2)
            {
                return 0;
            }
            break;
        }
        case CMD_UPDATE:
        case CMD_BATTERY:
        case CMD_ISLOCKED:
        {
            if(arg_num == 1)
            {
                return 0;
            }
            break;
        }
        case CMD_CHARGING_STATUS:
        {
            if(arg_num == 1)
            {
                return 0;
            }
            break;
        }
        case CMD_HVAC_STATUS:
        {
            if(arg_num == 1)
            {
                return 0;
            }
            break;
        }
        case CMD_REMAINING_CHARGING_STATUS:
        {
            if(arg_num == 1)
            {
                return 0;
            }
            break;
        }
        case CMD_MONITOR:
        {
            if(arg_num == 1)
            {
                return 0;
            }
            break;
        }
    }
    opts->error_message = "Incorrect number of operands";
    opts->error = true;
    return 1;
}

int phev_args_process_operands(char * arg, int arg_num, phev_args_opts_t * opts)
{
    switch (opts->command)
    {
        case CMD_REGISTER: {
            opts->error = true;
            opts->error_message = "Too many operands";
            break;
        }
        case CMD_HEADLIGHTS:
        case CMD_PARKING_LIGHTS:
        case CMD_AIRCON: {
            if(arg_num == 1)
            {
                if(strcmp(arg,ON) == 0)
                {
                    opts->operand_on = true;
                    break;
                }
                if(strcmp(arg,OFF) == 0)
                {
                    opts->operand_on = false;
                    break;
                }
                opts->error = true;
                opts->error_message = "Operand must be on or off";
                break;
            }

            opts->error = true;
            opts->error_message = "Too many operands";
            break;
        }
        case CMD_AIRCON_MODE:
        {
            if(arg_num == 1)
            {
                if(strcmp(arg,HEAT) == 0)
                {
                    opts->operand_mode = 2;
                    break;
                }
                if(strcmp(arg,COOL) == 0)
                {
                    opts->operand_mode = 1;
                    break;
                }
                if(strcmp(arg,WINDSCREEN) == 0)
                {
                    opts->operand_mode = 3;
                    break;
                }
                opts->error = true;
                opts->error_message = "Unrecognised operand";
                break;
            }

            if(arg_num == 2)
            {
                if(strlen(arg) == 2 && isdigit(arg[0]) && isdigit(arg[1]))
                {
                    opts->operand_time = atoi(arg);

                    if(opts->operand_time != 10 &&
                        opts->operand_time != 20 &&
                        opts->operand_time != 30
                    )
                    {
                        opts->error = true;
                        opts->error_message = "Unrecognised operand";
                        break;
                    }
                }
                else
                {
                    opts->error = true;
                    opts->error_message = "Unrecognised operand";
                }
            }
            break;
        }
        case CMD_GET_REG_VAL: {
            if(strlen(arg) == 2 && isdigit(arg[0]) && isdigit(arg[1]) && arg_num == 1)
            {
                opts->reg_operand = atoi(arg);
            }
            else
            {
                opts->error = true;
                opts->error_message = "Not a number";
            }
            break;
        }
        case CMD_MONITOR:
        case CMD_CHARGING_STATUS:
        case CMD_HVAC_STATUS:
        case CMD_ISLOCKED:
        case CMD_REMAINING_CHARGING_STATUS:
        case CMD_BATTERY: {
            break;
        }
    }
    return opts->error ? 1 : 0;
}

int phev_args_process_command(char * arg, int arg_num, phev_args_opts_t * opts)
{
    if(strcmp(arg,REGISTER) == 0 && arg_num ==0)
    {
        opts->command = CMD_REGISTER;
    }
    if(strcmp(arg,HEADLIGHTS) == 0 && arg_num == 0)
    {
        opts->command = CMD_HEADLIGHTS;
    }
    if(strcmp(arg,PARKING_LIGHTS) == 0 && arg_num == 0)
    {
        opts->command = CMD_PARKING_LIGHTS;
    }
    if(strcmp(arg,BATTERY) == 0 && arg_num == 0)
    {
        opts->command = CMD_BATTERY;
    }
    if(strcmp(arg,ISLOCKED) == 0 && arg_num == 0)
    {
        opts->command = CMD_ISLOCKED;
    }
    if(strcmp(arg,CHARGING_STATUS) == 0 && arg_num == 0)
    {
        opts->command = CMD_CHARGING_STATUS;
    }
    if(strcmp(arg,REMAINING_CHARGING_STATUS) == 0 && arg_num == 0)
    {
        opts->command = CMD_REMAINING_CHARGING_STATUS;
    }
    if(strcmp(arg,HVAC_STATUS) == 0 && arg_num == 0)
    {
        opts->command = CMD_HVAC_STATUS;
    }
    if(strcmp(arg,AIRCON) == 0 && arg_num == 0)
    {
        opts->command = CMD_AIRCON;
    }
    if(strcmp(arg,UPDATE) == 0 && arg_num == 0)
    {
        opts->command = CMD_UPDATE;
    }
    if(strcmp(arg,GET) == 0 && arg_num == 0)
    {
        opts->command = CMD_GET_REG_VAL;
    }
    if(strcmp(arg,MONITOR) == 0 && arg_num == 0)
    {
        opts->command = CMD_MONITOR;
    }
    if(strcmp(arg,AIRCON_MODE) == 0 && arg_num == 0)
    {
        opts->command = CMD_AIRCON_MODE;
    }
    return 0;
}

static error_t phev_args_parse_opt(int key, char *arg, struct argp_state *state) {
    phev_args_opts_t * opts = state->input;

    switch (key) {
    case 'p': {
        uint16_t port = 8080;
        if(arg != NULL) {
           port = atoi(arg);
           if(port == 0)
           {
               opts->error = true;
               opts->error_message = "Port number invalid";
               break;
           }
        } else {
            opts->error = true;
            opts->error_message = "No port number";
            break;
        }
        opts->port = port;
        break;
    }
    case 'm': {
        if(arg != NULL)
        {
            if (strlen(arg) == 17)
            {
                uint8_t *mac = malloc(6);
                sscanf(arg, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
                if(mac != NULL)
                {
                    opts->mac = mac;
                } else {
                    opts->error = true;
                    opts->error_message = "MAC invalid";
                }
                break;
            } else {
                opts->error = true;
                opts->error_message = "MAC invalid";
                break;
            }
        }
        opts->mac = PHEV_ARGS_DEFAULT_MAC;
        break;
    }
    case 'h': {
        if(arg !=NULL)
        {
            opts->host = strdup(arg);
        }
        break;
    }
    case 'u': {
        if(arg !=NULL)
        {
            opts->uri = strdup(arg);
        }
        break;
    }
    case 'v': {
        opts->verbose = true;
        break;
    }
    case 'c': {
        opts->carModel = atoi(arg);
        break;
    }
    case ARGP_KEY_END:
    {
        if(opts->error)
        {
            opts->command = CMD_INVALID;
            printf("\nERROR : %s\n",opts->error_message);
            argp_usage(state);
        }
        if(phev_args_validate(state->arg_num,opts))
        {
            opts->command = CMD_INVALID;
            printf("\nERROR : %s\n",opts->error_message);
            argp_usage(state);
        }
        break;
    }
    case ARGP_KEY_ARG: {
        if(state->arg_num == 0)
        {
            if(phev_args_process_command(strdup(arg), state->arg_num,opts))
            {
                opts->command = CMD_INVALID;
            }
            break;
        }

        if(phev_args_process_operands(strdup(arg), state->arg_num,opts))
        {
            opts->command = CMD_INVALID;
        }
        break;
    }

    default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp phev_args_argp = { phev_args_options, phev_args_parse_opt, phev_args_args_doc, phev_args_doc, 0, 0, 0 };

phev_args_opts_t * phev_args_parse(int argc, char *argv[])
{
    phev_args_opts_t * arguments = malloc(sizeof(phev_args_opts_t));

    arguments->host = strdup("192.168.8.46");
    arguments->uri = strdup("tcp://localhost:1883");
    arguments->mac = PHEV_ARGS_DEFAULT_MAC;
    arguments->port = 8080;
    arguments->init = false;
    arguments->command_topic = strdup("defaultin");
    arguments->topic = strdup("defaultout");
    arguments->verbose = false;
    arguments->command = CMD_UNSET;
    arguments->error = false;
    arguments->error_message = "";

    argp_parse(&phev_args_argp, argc, argv, 0, 0, arguments);

    return arguments;
}
