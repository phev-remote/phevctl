#ifndef _PHEV_ARGS_H_
#define _PHEV_ARGS_H_

#ifndef VERSION
#define VERSION "unknown"
#endif
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <stdint.h>

#define HEADLIGHTS "headlights"
#define BATTERY "battery"
#define AIRCON "aircon"
#define REGISTER "register"
#define MONITOR "monitor"
#define GET "get"
#define ON "on"
#define OFF "off"
#define WAIT_FOR_REG_MAX 40

typedef enum phev_args_commands_t { CMD_UNSET, CMD_INVALID, CMD_STATUS, CMD_REGISTER, CMD_HEADLIGHTS, CMD_BATTERY, CMD_AIRCON, CMD_GET_REG_VAL, CMD_DISPLAY_REG } phev_args_commands_t;

typedef struct phev_args_opts_t {
    bool init;
    char * host;
    const uint8_t * mac;
    int port;
    char * uri;
    char * topic;
    char * command_topic;
    bool verbose;
    bool operand_on;
    uint8_t reg_operand;
    bool error;
    char * error_message;
    phev_args_commands_t command;

} phev_args_opts_t;

static const uint8_t PHEV_ARGS_DEFAULT_MAC[] = {0,0,0,0,0,0};

static const char * phev_args_argp_program_version = "Version\t" VERSION;
static const char * phev_args_argp_program_bug_address = "jamie@wattu.com";
static char phev_args_doc[] = "\n\nProgram to control the car via the remote WiFi interface.  Requires this device to be connected to the REMOTE**** access point with a valid IP address, which is on the 192.168.8.x subnet.\n\nTHIS PROGRAM COMES WITH NO WARRANTY ANY DAMAGE TO THE CAR OR ANY OTHER EQUIPMENT IS AT THE USERS OWN RISK.";
static char phev_args_args_doc[] = "register\nbattery\naircon [on|off]\nheadlights [on|off]\nmonitor\nget <register>";
static struct argp_option phev_args_options[] = { 
    { "mac", 'm', "<MAC ADDRESS>",0, "MAC address."},
//    { "init", 'i', 0,0, "Initialise and register with the car - car must be in registration mode."},
    { "host", 'h', "<HOST NAME>",OPTION_HIDDEN, "IP address of car - defaults to 192.168.8.46."},
    { "port", 'p', "<PORT NUMBER>",OPTION_HIDDEN, "Port to use - defaults to 8080"},
    { "uri", 'u',"<URI>",OPTION_HIDDEN,"URI for MQTT server"},
    { "topic",'t',"<TOPIC NAME>",OPTION_HIDDEN,"MQTT output topic"},
    { "cmdtopic",'c',"<COMMAND TOPIC NAME>",OPTION_HIDDEN,"MQTT command topic"},
    { "verbose",'v',0,0,"Verbose output"},
    { 0 } 
};
phev_args_opts_t * phev_args_parse(int argc, char *argv[]);


#endif