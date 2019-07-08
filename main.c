#include <stdio.h>

#include "phev.h"

int main_eventHandler(phevEvent_t * event)
{
    printf("Event %d\n",event->type);
    if(event->type == PHEV_REGISTER_UPDATE) 
    {
        printf("Register : %d Data :",event->reg);
        for(int i=0;i<event->length;i++)
        {
            printf("%d ",event->data[i]);
        }
        printf("\n");
    }
    return 0;
}

int main()
{
    char mac[] = {0,0,0,0,0,0};
    phevSettings_t settings = {
        .host = "192.168.1.177",
        .mac = mac,
        .port = 8080,
        .handler = main_eventHandler,
    };
    printf("Hello\n");


    phevCtx_t * ctx = phev_init(settings);

    phev_start(ctx);
    int ch;
    do
    {
        ch = getchar();
        
    } while(ch!='Q' && ch != 'q');

}