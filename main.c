#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "phev_register.h"
#include "msg_tcpip.h"

const static char * APP_TAG = "MAIN";

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
    return read_len;
}

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
    
    global_sock = sock;
    LOG_V(APP_TAG,"END - connectSocket");
    
    return sock;
}

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
    
    int num = TCP_WRITE(soc,buf,len);
    
    LOG_D(APP_TAG,"Wriiten %d bytes from tcp stream", num);
    
    LOG_V(APP_TAG,"END - write");
    
    return num;
}
void outgoingHandler(messagingClient_t * client, message_t * message)
{
    printf("New Message\n");
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
phev_pipe_ctx_t * test_phev_register_create_pipe_helper(void)
{
    messagingSettings_t inSettings = {
        .incomingHandler = incomingHandler,
        .outgoingHandler = outgoingHandler,
    };
    
    tcpIpSettings_t outSettings = {
        .connect = connectToCar, 
        .read = tcp_client_read,
        .write = tcp_client_write,
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

int main()
{

    return 0;
}