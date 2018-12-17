//#define LOG_LEVEL 1
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#include <stdlib.h>
#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#endif
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <unistd.h>
#include <time.h>
#include "phev_core.h"
#include "phev_register.h"
#include "msg_tcpip.h"
#include "logger.h"

#ifdef _WIN32
#define TCP_READ recv
#define TCP_WRITE send
#else
#define TCP_READ read
#define TCP_WRITE write
#endif

#define TCP_CONNECT connect
#define TCP_SOCKET socket
#define TCP_HTONS htons
#define TCP_READ_TIMEOUT 1000
const static char * APP_TAG = "MAIN";


static bool ping_start = false;

#define DEFAULT_BUFLEN 1024

static void bufferDump(size_t length,uint8_t * buffer) 
{
    if(length <= 0 || buffer == NULL) return;

    char out[17];
    memset(&out,'\0',17);
        
    //printf("%s: ",tag);
    int i = 0;
    for(i=0;i<length;i++)
    {
        printf("%02x ",buffer[i]);
        out[i % 16] = (isprint(buffer[i]) ? buffer[i] : '.');
        if((i+1) % 8 == 0) printf(" ");
        if((i+1) % 16 ==0) {
            out[16] = '\0';
            printf(" | %s |\n%s: ",out);
        }
    }
    if((i % 16) + 1 != 0)
    {
        int num = (16 - (i % 16)) * 3;
        num = ((i % 16) < 8 ? num + 1 : num);
        out[(i % 16)] = '\0';
        char padding[(16 * 3) + 2];
        memset(&padding,' ',num+1);
        padding[(16-i)*3] = '\0';
        printf("%s | %s |\n",padding,out);
    }
    printf("\n");
}
int tcp_client_connectSocket(const char *host, uint16_t port); 

int tcp_client_read(int soc, uint8_t * buf, size_t len);

int tcp_client_write(int soc, uint8_t * buf, size_t len);

void my_ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms - (tv->tv_sec * 1000)) * 1000;
}

static int tcp_poll_read(int soc, int timeout_ms)
{
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(soc, &readset);
    struct timeval timeout;
    my_ms_to_timeval(timeout_ms, &timeout);
    return select(soc + 1, &readset, NULL, NULL, &timeout);
}
static int tcp_read(int soc, uint8_t *buffer, int len, int timeout_ms)
{
    int poll = -1;
    if ((poll = tcp_poll_read(soc, timeout_ms)) <= 0) {
        return poll;
    }
#ifdef _WIN32
    int read_len = TCP_READ(soc, buffer, len,0);
#else
    int read_len = TCP_READ(soc, buffer, len);
#endif
    if (read_len == 0) {
        return -1;
    }
    hexdump("TCP",buffer,read_len,0);
    return read_len;
}
#ifdef _WIN32

int tcp_client_connectSocket(const char *host, uint16_t port) 
{
    LOG_V(APP_TAG,"START - connectSocket");
    LOG_D(APP_TAG,"Host %s, Port %d",host,port);

    if(host == NULL) 
    {
        LOG_E(APP_TAG,"Host not set");
        return -1;
    }    
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(host, "8080", &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        
        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    LOG_V(APP_TAG,"END - connectSocket");
    
    return ConnectSocket;
}
#else
int tcp_client_connectSocket(const char *host, uint16_t port) 
{
    LOG_V(APP_TAG,"START - connectSocket");
    LOG_D(APP_TAG,"Host %s, Port %d",host,port);

    if(host == NULL) 
    {
        LOG_E(APP_TAG,"Host not set");
        return -1;
    }    
    struct sockaddr_in addr;
    /* set up address to connect to */
    memset(&addr, 0, sizeof(addr));
    //addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = TCP_HTONS(port);
    addr.sin_addr.s_addr = inet_addr(host);

    LOG_I(APP_TAG,"Host %s Port %d",host,port);
  
    int sock = TCP_SOCKET(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        LOG_E(APP_TAG,"Failed to open socket");
  
        return -1;
    }
    int ret = TCP_CONNECT(sock, (struct sockaddr *)(&addr), sizeof(addr));
    if(ret == -1)
    {
        LOG_E(APP_TAG,"Failed to connect");
  
        return -1;
    }
  
    LOG_I(APP_TAG,"Connected to host %s port %d",host,port);
    
    //global_sock = sock;
    LOG_V(APP_TAG,"END - connectSocket");
    
    return sock;
}
#endif
int tcp_client_read(int soc, uint8_t * buf, size_t len)
{
    LOG_V(APP_TAG,"START - read");
    
    int num = tcp_read(soc,buf,len,TCP_READ_TIMEOUT);

    LOG_D(APP_TAG,"Read %d bytes from tcp stream", num);
    LOG_BUFFER_HEXDUMP(APP_TAG,buf, num,LOG_DEBUG);
    
    
    LOG_V(APP_TAG,"END - read");
    
    return num;
}
int tcp_client_write(int soc, uint8_t * buf, size_t len)
{
    LOG_V(APP_TAG,"START - write");
#ifdef _WIN32
    int num = TCP_WRITE(soc,buf,len,0);
#else
    int num = TCP_WRITE(soc,buf,len);
#endif    
    LOG_D(APP_TAG,"Wriiten %d bytes from tcp stream", num);
    
    LOG_V(APP_TAG,"END - write");
    
    return num;
}

void outgoingHandler(messagingClient_t * client, message_t * message)
{
    LOG_I(APP_TAG,"Outgoing Handler data");
    //bufferDump(message->length,message->data);
}
message_t * incomingHandler(messagingClient_t *client) 
{
    return NULL;
}
int dummy_connect(messagingClient_t * client)
{
    client->connected = true;
    return 0;
}
int connectToCar(const char *host, uint16_t port)
{
    LOG_D(APP_TAG,"Connect to car - Host %s Port %d",host,port);

    return tcp_client_connectSocket(host,port);
}

phev_pipe_ctx_t * create_pipe(const char * host)
{
    messagingSettings_t inSettings = {
        .incomingHandler = incomingHandler,
        .outgoingHandler = outgoingHandler,
    };
    
    tcpIpSettings_t outSettings = {
        .connect = connectToCar, 
        .read = tcp_client_read,
        .write = tcp_client_write,
	    .host = strdup(host),
	    .port = 8080,
    };
         
    messagingClient_t * in = msg_core_createMessagingClient(inSettings);
    messagingClient_t * out = msg_tcpip_createTcpIpClient(outSettings);

    phev_pipe_settings_t settings = {
        .ctx = NULL,
        .in = in,
        .out = out,
        .inputSplitter = NULL,
        .outputSplitter = NULL,
        .inputResponder = NULL,
        .outputResponder = (msg_pipe_responder_t) phev_pipe_commandResponder,
        .outputOutputTransformer = (msg_pipe_transformer_t) phev_pipe_outputEventTransformer,
        .preConnectHook = NULL,
        .outputInputTransformer = (msg_pipe_transformer_t) phev_pipe_outputChainInputTransformer,
    };

    return phev_pipe_createPipe(settings);
}
static bool reg_completed = false;
void reg_complete(phevRegisterCtx_t * ctx)
{
    printf("Registration complete VIN is %s\n",ctx->vin);
    reg_completed = true;
    phevMessage_t * headLightsOn = phev_core_simpleRequestCommandMessage(KO_WF_H_LAMP_CONT_SP, 1);
    message_t * message = phev_core_convertToMessage(headLightsOn);
    phev_core_destroyMessage(headLightsOn);

    msg_pipe_outboundPublish(ctx->pipe->pipe,  message);
}

void printHelp(void)
{
    printf("HELP\n");
}
char * getHostFromArg(int argc, char *argv[])
{
    printf("Getting Host\n");
    for(int i = 1;i < argc;i++)
    {
        if(strcmp("--host",argv[i]) == 0)
        {
            if(i == argc) 
            {
                printf("No host passed\n");
                return NULL;
            } else {
                return strdup(argv[i+1]);
            }
        }
    }
    return NULL;
}

uint8_t * getMacFromString(const char * str)
{
    uint8_t * mac = malloc(6);
                
    if(strlen(str) == 17)
    {
        sscanf(str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
                    
        return mac;
    } else {
        printf("Invalid MAC %s\n",str);        
        return NULL;
    }
}
uint8_t * getMacFromArg(int argc, char *argv[])
{
    printf("Getting MAC\n");
    for(int i = 1;i < argc;i++)
    {
        if(strcmp("--mac",argv[i]) == 0)
        {
            if(i == argc) 
            {
                printf("No MAC passed\n");
                return NULL;
            } else {
                printf("Got mac %s\n",argv[i+1]);
                return strdup(argv[i+1]);
            }
        }
    }
    return NULL;
}

void errorHandler(void * ctx)
{
    printf("Registration Error\n");
    exit(-1);
}
int main(int argc, char *argv[])
{
    printf("Starting\n");

    printf("Number of arguments %d\n",argc);

    if(argc < 4) 
    {
        printHelp();
        return 0;
    }
    

    char * macStr = getMacFromArg(argc,argv);
    
    uint8_t * mac = getMacFromString(macStr);
    
    if(mac == NULL)
    {
        printf("Error invalid MAC %s\n",macStr);
        return -1;
    }

    char * host = getHostFromArg(argc,argv);
    
    if(host == NULL)
    {
        host = strdup("192.168.1.46");
    }

    printf("Host %s and MAC %s\n",host,macStr);
    
    phev_pipe_ctx_t * pipe = create_pipe(host);

    phevRegisterSettings_t settings = {
        .pipe = pipe,
        .complete = (phevRegistrationComplete_t) reg_complete,
        .errorHandler = (phevErrorHandler_t) errorHandler,
    };

    memcpy(&settings.mac,mac,6);
    
    phevRegisterCtx_t * ctx = phev_register_init(settings);
    
    while(1) 
    {
        phev_pipe_loop(pipe);
        if(reg_completed) 
        {
            printf("\nCompleted registration\n");
            phev_pipe_deregisterEventHandler(pipe,NULL);
            reg_completed = false;
        }
    }
    return 0;
}
