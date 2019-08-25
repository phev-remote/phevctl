#include <stddef.h>
#include "phevargs.h"


int phev_args_validate(int arg_num,phev_args_opts_t * opts)
{
    switch (opts->command)
    {
        case CMD_HEADLIGHTS:
        case CMD_AIRCON: 
        {
            if(arg_num == 2)
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
    }
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
    }
    if(!opts->error)
    {
        return 0;
    } else {
        return 1;
    }
}
int phev_args_process_command(char * arg, int arg_num, phev_args_opts_t * opts)
{
    if(strcmp(arg,REGISTER) == 0 && arg_num ==0)
    {
        opts->command = CMD_REGISTER;
    }
    if(strcmp(arg,HEADLIGHTS) == 0 && arg_num == 0)
    {
        printf("Arg num %d\n",arg_num);
        opts->command = CMD_HEADLIGHTS;
    }
    if(strcmp(arg,BATTERY) == 0 && arg_num == 0)
    {
        opts->command = CMD_BATTERY;
    }
    if(strcmp(arg,AIRCON) == 0 && arg_num == 0)
    {
        opts->command = CMD_AIRCON;
    }
    if(strcmp(arg,GET) == 0 && arg_num == 0)
    {
        opts->command = CMD_GET_REG_VAL;
    }
    if(strcmp(arg,MONITOR) == 0 && arg_num == 0)
    {
        opts->command = CMD_DISPLAY_REG;
    }
    if(strcmp(arg,ON) == 0 && arg_num == 1)
    {
        opts->operand_on = true;
    }
    if(strcmp(arg,OFF) == 0 && arg_num == 1)
    {
        opts->operand_on = false;
    } 
    if(strlen(arg) == 2 && isdigit(arg[0]) && isdigit(arg[1]) && arg_num == 1)
    {
        //opts->uint_value = atoi(arg);
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
        }
        //arguments->port = port;
        break;
    }
    case 'm': {
        if(arg != NULL)
        {
            if (strlen(arg) == 17)
            {
                uint8_t *mac = malloc(6);
                sscanf(arg, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
                //arguments->mac = mac;
                break;
            }
        }
        //arguments->mac = DEFAULT_MAC;
        break;
    } 
    case 'h': {
        if(arg !=NULL) 
        {
      //      arguments->host = strdup(arg);
        }
        break;
    }
    case 'u': {
        if(arg !=NULL)
        {
        //    arguments->uri = strdup(arg);
        }
        break;
    }
    case 't': {
        if(arg !=NULL)
        {
    //        arguments->topic = strdup(arg);
        }
        break;
    }
    case 'c': {
        if(arg !=NULL)
        {
     //       arguments->command_topic = strdup(arg);
        }
        break;
    }
    case 'v': {
    //    arguments->verbose = true;
        break;
    }
    case ARGP_KEY_END:
    {
        printf("Hello %d\n",state->arg_num);

        if(opts->error)
        {
            opts->command = CMD_INVALID;
            argp_usage(state);
        }
        if(phev_args_validate(state->arg_num,opts))
        {
            opts->command = CMD_INVALID;
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

    argp_parse(&phev_args_argp, argc, argv, ARGP_NO_EXIT, 0, arguments);
    
    return arguments;
}
