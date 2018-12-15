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
#include "phev_register.h"
#include "msg_tcpip.h"
#include "logger.h"

#define TCP_READ read
#define TCP_WRITE write
#define TCP_CONNECT connect
#define TCP_SOCKET socket
#define TCP_HTONS htons
#define TCP_READ_TIMEOUT 1000
const static char * APP_TAG = "MAIN";

#define DEFAULT_BUFLEN 1024

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
    int read_len = TCP_READ(soc, buffer, len);
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
    printf("Here");
    
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("192.168.8.46", "8080", &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    printf("Here");
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
        printf("Here2");
    
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
    printf("Connecting to %s %d\n",host,port);
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
    hexdump("TCP",buf, num,0);
    
    
    LOG_V(APP_TAG,"END - read");
    
    return num;
}
int tcp_client_write(int soc, uint8_t * buf, size_t len)
{
    LOG_V(APP_TAG,"START - write");
    
    int num = send(soc,buf,len,0);

    hexdump("TCP",buf,len,0);    
    LOG_D(APP_TAG,"Wriiten %d bytes from tcp stream", num);
    
    LOG_V(APP_TAG,"END - write");
    
    return num;
}
void outgoingHandler(messagingClient_t * client, message_t * message)
{
    LOG_BUFFER_HEXDUMP(APP_TAG,message->data,message->length,0);
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
phev_pipe_ctx_t * create_pipe(void)
{
    messagingSettings_t inSettings = {
        .incomingHandler = incomingHandler,
        .outgoingHandler = outgoingHandler,
    };
    
    tcpIpSettings_t outSettings = {
        .connect = connectToCar, 
        .read = tcp_client_read,
        .write = tcp_client_write,
	    .host = "192.168.8.46",
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

void reg_complete(void)
{
    printf("Registration complete");
}

uint8_t currentPing;
bool successfulPing;
time_t lastPingTime;

void ping(msg_pipe_ctx_t * ctx)
{
    LOG_V(APP_TAG,"START - ping");
    if(((currentPing + 1) % 30) == 0) 
    {
        LOG_D(APP_TAG,"Send time sync");
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime(&now);
        
        const uint8_t pingTime[] = {
            timeinfo.tm_year - 100,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec,
            1
        };
        phevMessage_t * dateCmd = phev_core_commandMessage(KO_WF_DATE_INFO_SYNC_SP,pingTime, sizeof(pingTime));
        message_t * message = phev_core_convertToMessage(dateCmd);
        msg_pipe_outboundPublish(ctx,  message);
        //msg_utils_destroyMsg(message);
        //phev_core_destroyMessage(dateCmd);
    
    }
    successfulPing = false;
    phevMessage_t * ping = phev_core_pingMessage(currentPing++);
    message_t * message = phev_core_convertToMessage(ping);
    //phev_controller_sendMessage(ctx, message);
    msg_pipe_outboundPublish(ctx,  message);
    //msg_utils_destroyMsg(message);
    //phev_core_destroyMessage(ping);
    LOG_V(APP_TAG,"END - ping");
    
}
void resetPing(void)
{
    currentPing = 0;
    lastPingTime = 0;
}

int main()
{
    phev_pipe_ctx_t * pipe = create_pipe();

    phevRegisterSettings_t settings = {
        .pipe = pipe,
        .mac = { 0xb8,0x27,0xeb,0xb2,0xb1,0x5c },
        .eventHandler = (phevPipeEventHandler_t) phev_register_eventHandler,
        .complete = (phevRegistrationComplete_t) reg_complete,
    };

    phevRegisterCtx_t * ctx = phev_register_init(settings);
    time_t now;
    resetPing();
    while(1) 
    {
        msg_pipe_loop(pipe->pipe);
        time(&now);
        if(now > lastPingTime) {
            ping(pipe->pipe);
            time(&lastPingTime);
        }
    }
    return 0;
}
