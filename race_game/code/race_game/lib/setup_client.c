#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<string.h>


int setup_client(char *hostname,u_short port)
{
    struct hostent *servhost;
    struct sockaddr_in server;
    int s;

    if ((servhost=gethostbyname(hostname))==NULL)
    {
        fprintf(stderr,"badhostname!\n");
        return -1;
    }

    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port =htons(port);
    memcpy(&server.sin_addr,servhost->h_addr,servhost->h_length);

    if ((s=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        fprintf(stderr,"socket allocation failed\n");
        return -1;
    }

    if (connect(s,(struct sockaddr*)&server,sizeof(server))==-1)
    {
        fprintf(stderr,"cannot connect.\n");
        return -1;
    }
    fprintf(stderr,"connected.\n");

    return s;
}