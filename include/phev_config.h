#ifndef _PHEV_CONFIG_H_
#define _PHEV_CONFIG_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "phev_core.h"
#ifdef __XTENSA__
#include "cJSON.h"
#else
#include <cjson/cJSON.h>
#endif
#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif

#define IMAGE_PREFIX "firmware-"


#define UPDATE_CONFIG_JSON "update"
#define UPDATE_CONFIG_NAME "update"
#define UPDATE_CONFIG_SSID "ssid"
#define UPDATE_CONFIG_PASSWORD "password"
#define UPDATE_CONFIG_HOST "host"
#define UPDATE_CONFIG_PATH "path"
#define UPDATE_CONFIG_PORT "port"
#define UPDATE_CONFIG_LATEST_BUILD "latestBuild"
#define UPDATE_CONFIG_OVER_GSM "overGsm"
#define UPDATE_CONFIG_FORCE_UPDATE "forceUpdate"

#define CONNECTION_CONFIG_JSON "carConnection"
#define CONNECTION_CONFIG_HOST "host"
#define CONNECTION_CONFIG_PORT "port"
#define CONNECTION_CONFIG_SSID "ssid"
#define CONNECTION_CONFIG_PASSWORD "password"

#define STATE_CONFIG_JSON "state"
#define STATE_CONFIG_CONNECTED_CLIENTS "connectedClients"
#define STATE_CONFIG_HEADLIGHTS_ON "headLightsOn"
#define STATE_CONFIG_PARKLIGHTS_ON "parkLightsOn"
#define STATE_CONFIG_AIRCON_ON "airConOn"

#define MAX_WIFI_SSID_LEN 32
#define MAX_WIFI_PASSWORD_LEN 64
typedef struct phevWifi_t
{
    char ssid[MAX_WIFI_SSID_LEN];
    char password[MAX_WIFI_PASSWORD_LEN];
} phevWifi_t;

typedef enum {
   TRUE      = 1,
   FALSE     = 0,
   NOTSET = 2
} triState_t;

typedef struct phevState_t {
    int connectedClients;
    triState_t headLightsOn;
    triState_t parkLightsOn;
    triState_t airConOn;
} phevState_t;

typedef struct phevConnectionConfig_t {
    char * host;
    uint16_t port;
    phevWifi_t carConnectionWifi;
} phevConnectionConfig_t;

typedef struct phevUpdateConfig_t {
    uint32_t latestBuild;
    uint32_t currentBuild;
    bool updateOverPPP;
    phevWifi_t updateWifi;
    char * updatePath;
    char * updateHost;
    uint16_t updatePort;
    char * updateImageFullPath;
    bool forceUpdate;
} phevUpdateConfig_t;

typedef struct phevConfig_t
{
    phevConnectionConfig_t connectionConfig;
    phevUpdateConfig_t updateConfig;
    phevState_t state;
} phevConfig_t;

phevConfig_t * phev_config_parseConfig(const char * config);
bool phev_config_checkForFirmwareUpdate(const phevUpdateConfig_t * config);
bool phev_config_checkForConnection(const phevState_t * state);
bool phev_config_checkForHeadLightsOn(const phevState_t * state);
bool phev_config_checkForParkLightsOn(const phevState_t * state);
bool phev_config_checkForAirConOn(const phevState_t * state);
bool phev_config_checkForHeadLightsOff(const phevState_t * state);
bool phev_config_checkForParkLightsOff(const phevState_t * state);
bool phev_config_checkForAirConOff(const phevState_t * state);

char * phev_config_displayConfig(const phevConfig_t * config);
bool phev_config_checkForOption(const cJSON * json, const char * option);
bool phev_config_checkForOption(const cJSON * json, const char * option);
char * phev_config_getConfigString(cJSON * json, char * option); 
uint16_t phev_config_getConfigInt(cJSON * json, char * option); 
long phev_config_getConfigLong(cJSON * json, char * option); 
bool phev_config_getConfigBool(cJSON * json, char * option); 
triState_t phev_config_getConfigTriState(cJSON * json, char * option); 

#endif