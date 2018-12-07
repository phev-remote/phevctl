#include <stdio.h>
#include "phev_config.h"
#ifdef __XTENSA__
#include "cJSON.h"
#else
#include <cjson/cJSON.h>
#endif

bool phev_config_checkForOption(const cJSON * json, const char * option)
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    return (value != NULL ? true : false); 
}
char * phev_config_getConfigString(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    
    return value->valuestring;
}
uint16_t phev_config_getConfigInt(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    
    return value->valueint;
}

long phev_config_getConfigLong(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        return 0;
    }
    return (long) value->valuedouble;
}
bool phev_config_getConfigBool(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        return false;
    }    
    return cJSON_IsTrue(value);
}

triState_t phev_config_getConfigTriState(cJSON * json, char * option) 
{
    cJSON * value = cJSON_GetObjectItemCaseSensitive(json, option);
    if(value == NULL) {
        return NOTSET;
    }    
    return cJSON_IsTrue(value);
}
void phev_config_setUpdateConfig(phevUpdateConfig_t * config, const char * ssid, 
                                        const char * password,
                                        const char * host,
                                        const char * path,
                                        uint16_t port,
                                        long long unsigned build,
                                        bool updateOverPPP,
                                        bool forceUpdate
                                        )
{
    strcpy(config->updateWifi.ssid,ssid);
    config->updateWifi.ssid[strlen(ssid)] = '\0';
    
    strcpy(config->updateWifi.password,password);

    config->updateWifi.password[strlen(password)] = '\0';
    
    config->updateHost = malloc(strlen(host));
    strcpy(config->updateHost,host);    
    
    config->updatePath = malloc(strlen(path));
    strcpy(config->updatePath,path);
    
    const char * buildPath = NULL;     
    asprintf(&buildPath,"%s%s%010llu.bin", config->updatePath,IMAGE_PREFIX,build);
    
    config->updateImageFullPath = buildPath;

    config->updatePort = port;

    config->latestBuild = build;

    config->currentBuild = BUILD_NUMBER;

    config->updateOverPPP = updateOverPPP;

    config->forceUpdate = forceUpdate;
}

void phev_config_parseUpdateConfig(phevConfig_t * config, cJSON * update)
{
    unsigned long long build = phev_config_getConfigLong(update,UPDATE_CONFIG_LATEST_BUILD);

    phev_config_setUpdateConfig(&config->updateConfig, phev_config_getConfigString(update,UPDATE_CONFIG_SSID), 
                                        phev_config_getConfigString(update,UPDATE_CONFIG_PASSWORD),
                                        phev_config_getConfigString(update,UPDATE_CONFIG_HOST),
                                        phev_config_getConfigString(update,UPDATE_CONFIG_PATH),
                                        phev_config_getConfigInt(update,UPDATE_CONFIG_PORT),
                                        build,
                                        phev_config_getConfigBool(update, UPDATE_CONFIG_OVER_GSM),
                                        phev_config_getConfigBool(update, UPDATE_CONFIG_FORCE_UPDATE)
                                    );
                                
    
}
void phev_config_parseConnectionConfig(phevConfig_t * config, cJSON * connection)
{
    config->connectionConfig.host = phev_core_strdup(phev_config_getConfigString(connection, CONNECTION_CONFIG_HOST));
    config->connectionConfig.port = phev_config_getConfigInt(connection, CONNECTION_CONFIG_PORT);

    strcpy(config->connectionConfig.carConnectionWifi.ssid, phev_config_getConfigString(connection, CONNECTION_CONFIG_SSID)); 
    strcpy(config->connectionConfig.carConnectionWifi.password, phev_config_getConfigString(connection, CONNECTION_CONFIG_PASSWORD)); 
}

void phev_config_parseStateConfig(phevConfig_t * config, cJSON * state)
{
    config->state.connectedClients = phev_config_getConfigInt(state, STATE_CONFIG_CONNECTED_CLIENTS);
    config->state.headLightsOn = phev_config_getConfigTriState(state, STATE_CONFIG_HEADLIGHTS_ON);
    config->state.parkLightsOn = phev_config_getConfigTriState(state, STATE_CONFIG_PARKLIGHTS_ON);
    config->state.airConOn = phev_config_getConfigTriState(state, STATE_CONFIG_AIRCON_ON);
}
char * phev_config_displayConfig(const phevConfig_t * config)
{
    const char * FORMAT = "Config\nCar Connection\n\tHost %s\n\tPort %d\n\tSSID %s\n\tPassword %s\n";
    char * out = NULL;
    asprintf(&out,FORMAT,config->connectionConfig.host, config->connectionConfig.port,
            config->connectionConfig.carConnectionWifi.ssid, config->connectionConfig.carConnectionWifi.password);
    return out;
}
phevConfig_t * phev_config_parseConfig(const char * config)
{
    phevConfig_t * phevConfig = malloc(sizeof(phevConfig_t));

    cJSON * json = cJSON_Parse((const char *) config);

    //char * string = cJSON_Print(json);

    if(json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return NULL;
    } 
    cJSON * update = cJSON_GetObjectItemCaseSensitive(json, UPDATE_CONFIG_JSON);

    if(update != NULL)
    {
        phev_config_parseUpdateConfig(phevConfig, update);
    } else {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return NULL;
    }

    cJSON * connection = cJSON_GetObjectItemCaseSensitive(json, CONNECTION_CONFIG_JSON);

    if(connection)
    {
        phev_config_parseConnectionConfig(phevConfig, connection);
    }

    cJSON * state = cJSON_GetObjectItemCaseSensitive(json, STATE_CONFIG_JSON);

    if(state)
    {
        phev_config_parseStateConfig(phevConfig, state);
    }

    cJSON_Delete(json);

    return phevConfig;
}
bool phev_config_checkForFirmwareUpdate(const phevUpdateConfig_t * config)
{
#ifndef NO_OTA
    return (config->latestBuild > config->currentBuild) || config->forceUpdate;
#else
    return false;    
#endif
}

bool phev_config_checkForConnection(const phevState_t * state)
{
    return (state->connectedClients > 0);
}

bool phev_config_checkForHeadLightsOn(const phevState_t * state)
{
    return (state->headLightsOn == TRUE ? true : false) ;
}

bool phev_config_checkForAirConOn(const phevState_t * state)
{
    return (state->airConOn == TRUE ? true : false);
}

bool phev_config_checkForParkLightsOn(const phevState_t * state)
{
    return (state->parkLightsOn == TRUE ? true : false) ;
}

bool phev_config_checkForHeadLightsOff(const phevState_t * state)
{
    return (state->headLightsOn == TRUE ? false : true) ;
}

bool phev_config_checkForAirConOff(const phevState_t * state)
{
    return (state->airConOn == TRUE ? false : true) ;
}

bool phev_config_checkForParkLightsOff(const phevState_t * state)
{
    return (state->parkLightsOn == TRUE ? false : true); 
}
