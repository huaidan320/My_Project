#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "aoa_proxy.h"
#include "log.h"
#include "tcp.h"

#define LENGTH_OF_LISTEN_QUEUE  20

#define CMD_SOCKET_PORT_MD	    7240
#define VIDEO_SOCKET_PORT_MD	8240
#define MEDIA_SOCKET_PORT_MD    9240
#define TTS_SOCKET_PORT_MD	    9241
#define VR_SOCKET_PORT_MD	    9242
#define TOUCH_SOCKET_PORT_MD	9340

static void *cmd_server(void *d);
static void *video_server(void *d);
static void *media_server(void *d);
static void *tts_server(void *d);
static void *vr_server(void *d);
static void *touch_server(void *d);

static pthread_t cmd_thread;
static pthread_t video_thread;
static pthread_t media_thread;
static pthread_t tts_thread;
static pthread_t vr_thread;
static pthread_t touch_thread;

static void *cmd_server(void *d)
{
    logDebug("Start cmd server!\n");

    t_socketfd *socketfd = (t_socketfd *)d;

    /* Set a socket address structure server_addr to represent the server
       Internet address and port */
    struct sockaddr_in server_addr;
    
    struct sockaddr_in client_addr;
    int new_server_socket;
    
    socklen_t length = sizeof(client_addr);

    /* Set the contents of an area of memory to 0 */
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(CMD_SOCKET_PORT_MD);

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
    {
        logError("Create socket failed!\n");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        logError("Server Bind Port : %d Failed!\n", CMD_SOCKET_PORT_MD); 
        return NULL;
    }

    if(listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        logError("Server Listen Failed!\n"); 
        return NULL;
    }

    while(1)
    {
        new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if(new_server_socket < 0)
        {
            logError("Server cmd accept failed!\n");
            break;
        }

        if(socketfd->cmd_socketfd > 0)
        {
            logDebug("Close cmd new server socket!\n");
            close(socketfd->cmd_socketfd);
        }
        
        //Record the local soket link descriptor
        socketfd->cmd_socketfd = new_server_socket;
        socketfd->cmd_socketDead = 0;

        logDebug("Accept cmd socket success!\n");       
    }

    logDebug("Stop cmd server!\n");
    
    close(server_socket);
    
    return NULL;
}

static void *video_server(void *d)
{
    logDebug("Start video server!\n");

    t_socketfd *socketfd = (t_socketfd *)d;

    /* Set a socket address structure server_addr to represent the server
       Internet address and port */
    struct sockaddr_in server_addr;
    
    struct sockaddr_in client_addr;
    int new_server_socket;
    
    socklen_t length = sizeof(client_addr);

    /* Set the contents of an area of memory to 0 */
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(VIDEO_SOCKET_PORT_MD);

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
    {
        logError("Create socket failed!\n");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        logError("Server Bind Port : %d Failed!\n", VIDEO_SOCKET_PORT_MD); 
        return NULL;
    }

    if(listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        logError("Server Listen Failed!\n"); 
        return NULL;
    }

    while(1)
    {
        new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if(new_server_socket < 0)
        {
            logError("Server video accept failed!\n");
            break;
        }

        if(socketfd->video_socketfd > 0)
        {
            logDebug("Close video new server socket!\n");
            close(socketfd->video_socketfd);
        }
 
        //Record the local soket link descriptor
        socketfd->video_socketfd = new_server_socket;
        socketfd->video_socketDead = 0;

        logDebug("Accept video socket success!\n");
    }

    logDebug("Stop video server!\n");
    
    close(server_socket);
    
    return NULL;
}

static void *media_server(void *d)
{
    logDebug("Start media server!\n");

    t_socketfd *socketfd = (t_socketfd *)d;
    
    /* Set a socket address structure server_addr to represent the server
       Internet address and port */
    struct sockaddr_in server_addr;
    
    struct sockaddr_in client_addr;
    int new_server_socket;
    
    socklen_t length = sizeof(client_addr);

    /* Set the contents of an area of memory to 0 */
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(MEDIA_SOCKET_PORT_MD);

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
    {
        logError("Create socket failed!\n");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        logError("Server Bind Port : %d Failed!\n", MEDIA_SOCKET_PORT_MD); 
        return NULL;
    }

    if(listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        logError("Server Listen Failed!\n"); 
        return NULL;
    }

    while(1)
    {
        new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if(new_server_socket < 0)
        {
            logError("Server media accept failed!\n");
            break;
        }

        if(socketfd->media_socketfd > 0)
        {
            logDebug("Close media new server socket!\n");
            close(socketfd->media_socketfd);
        }

        //Record the local soket link descriptor
        socketfd->media_socketfd = new_server_socket;
        socketfd->media_socketDead = 0;

        logDebug("Accept media socket success!\n");
    }

    logDebug("Stop media server!\n");

    close(server_socket);

    return NULL;
}

static void *tts_server(void *d)
{
    logDebug("Start tts server!\n");

    t_socketfd *socketfd = (t_socketfd *)d;

    /* Set a socket address structure server_addr to represent the server
       Internet address and port */
    struct sockaddr_in server_addr;
    
    struct sockaddr_in client_addr;
    int new_server_socket;
    
    socklen_t length = sizeof(client_addr);

    /* Set the contents of an area of memory to 0 */
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(TTS_SOCKET_PORT_MD);

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
    {
        logError("Create socket failed!\n");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        logError("Server Bind Port : %d Failed!\n", TTS_SOCKET_PORT_MD); 
        return NULL;
    }

    if(listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        logError("Server Listen Failed!\n"); 
        return NULL;
    }

    while(1)
    {
        new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if(new_server_socket < 0)
        {
            logError("Server tts accept failed!\n");
            break;
        }

        if(socketfd->tts_socketfd > 0)
        {
            logDebug("Close tts new server socket!\n");
            close(socketfd->tts_socketfd);
        }
        
        //Record the local soket link descriptor
        socketfd->tts_socketfd = new_server_socket;
        socketfd->tts_socketDead = 0;

        logDebug("Accept tts socket success!\n");
    }

    logDebug("Stop tts server!\n");
    
    close(server_socket);
    
    return NULL;
}

static void *vr_server(void *d)
{
    logDebug("Start tts server!\n");

    t_socketfd *socketfd = (t_socketfd *)d;

    /* Set a socket address structure server_addr to represent the server
       Internet address and port */
    struct sockaddr_in server_addr;
    
    struct sockaddr_in client_addr;
    int new_server_socket;
    
    socklen_t length = sizeof(client_addr);

    /* Set the contents of an area of memory to 0 */
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(VR_SOCKET_PORT_MD);

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
    {
        logError("Create socket failed!\n");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        logError("Server Bind Port : %d Failed!\n", VR_SOCKET_PORT_MD); 
        return NULL;
    }

    if(listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        logError("Server Listen Failed!\n"); 
        return NULL;
    }

    while(1)
    {
        new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if(new_server_socket < 0)
        {
            logError("Server vr accept failed!\n");
            break;
        }

        if(socketfd->vr_socketfd > 0)
        {
            logDebug("Close vr new server socket!\n");
            close(socketfd->vr_socketfd);
        }

        //Record the local soket link descriptor
        socketfd->vr_socketfd = new_server_socket;
        socketfd->vr_socketDead = 0;

        logDebug("Accept vr socket success!\n");
    }

    logDebug("Stop vr server!\n");
    
    close(server_socket);
    
    return NULL;
}

static void *touch_server(void *d)
{
    logDebug("Start touch server!\n");

    t_socketfd *socketfd = (t_socketfd *)d;

    /* Set a socket address structure server_addr to represent the server
       Internet address and port */
    struct sockaddr_in server_addr;
    
    struct sockaddr_in client_addr;
    int new_server_socket;
    
    socklen_t length = sizeof(client_addr);

    /* Set the contents of an area of memory to 0 */
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(TOUCH_SOCKET_PORT_MD);

    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
    {
        logError("Create socket failed!\n");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        logError("Server Bind Port : %d Failed!\n", TOUCH_SOCKET_PORT_MD); 
        return NULL;
    }

    if(listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        logError("Server Listen Failed!\n"); 
        return NULL;
    }

    while(1)
    {
        new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if(new_server_socket < 0)
        {
            logError("Server touch accept failed!\n");
            break;
        }

        if(socketfd->touch_socketfd > 0)
        {
            logDebug("Close touch new server socket!\n");
            close(socketfd->touch_socketfd);
        }
        
        //Record the local soket link descriptor
        socketfd->touch_socketfd = new_server_socket;
        socketfd->touch_socketDead = 0;

        logDebug("Accept touch socket success!\n");
    }

    logDebug("Stop touch server!\n");
    
    close(server_socket);
    
    return NULL;    
}

void create_start_server(t_socketfd *socketfd)
{ 
    pthread_create(&cmd_thread, NULL, cmd_server, (void *)socketfd);
    pthread_create(&video_thread, NULL, video_server, (void *)socketfd);
    pthread_create(&media_thread, NULL, media_server, (void *)socketfd);
    pthread_create(&tts_thread, NULL, tts_server, (void *)socketfd);
    pthread_create(&vr_thread, NULL, vr_server, (void *)socketfd);
    pthread_create(&touch_thread, NULL, touch_server, (void *)socketfd);
}
