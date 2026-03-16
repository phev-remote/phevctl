#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define LOG_LEVEL LOG_DEBUG

#include <phev/phev_core.h>
uint8_t *xorDataWithValue(const uint8_t *data, uint8_t xor)
{

    uint8_t length = (data[1] ^ xor) + 2;
    uint8_t *decoded = malloc(length);

    for (int i = 0; i < length; i++)
    {
        decoded[i] = data[i] ^ xor;
    }
    return decoded;
}
bool validateChecksum(const uint8_t *data)
{
    uint8_t chksum = phev_core_checksum(data);

    return (chksum == data[data[1] + 1]);
}
int main(int argc, char *argv[])
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    uint8_t *message = NULL;
    phevMessage_t phevMsg;
    bool incoming = false;
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Cannot open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (argv[2][0] == 'i')
    {
        incoming = true;
    }
    printf("File %s opened incoming %s -- %s\n", argv[1], (incoming ? "Y" : "N"), argv[2]);
    read = getline(&line, &len, fp);
    while (read != -1)
    {
        size_t numBytes = read >> 1;
        int j = 0;
        message = malloc(numBytes);
        for (int i = 0; i < read - 1; i += 2)
        {
            char byteStr[2];

            strncpy(byteStr, line + i, 2);
            uint8_t num = (u_int8_t)strtol(byteStr, NULL, 16);
            message[j++] = num;
        }
        uint8_t *data = NULL;
        uint8_t xor = message[2];
        uint8_t mask = ((message[0] ^ xor) & 0x01);
        if (xor < 2)
        {
            data = message;
        }
        else
        {
            if ((message[0] ^ xor) > 0xe0)
            {
                if ((message[0] ^ xor) != 0xf3)
                {
                    xor ^= mask;
                }
                data = xorDataWithValue(message, xor);
            }
            else
            {
                xor = (message[2] & 0xfe) ^ ((message[0] & 0x01) ^ 1);
                data = xorDataWithValue(message, xor);
            }
        }

        if (data)
        {
            printf(">> ");
            for (int i = 0; i < numBytes; i++)
            {
                printf("%02X ", message[i]);
            }
            printf("<< >> ");
            for (int i = 0; i < numBytes; i++)
            {
                printf(" %02X", data[i]);
            }
            printf("\n");
        }
        read = getline(&line, &len, fp);
    }

    fclose(fp);
    exit(EXIT_SUCCESS);
}
