#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<string.h>

int setup_server(char *hostname,u_short port)
{
    struct hostent *myhost;
    struct sockaddr_in me;
    int s,s_waiting;

    if ((myhost=gethostbyname(hostname))==NULL)
    {
        fprintf(stderr,"badhostname!\n");
        return -1;
    }

    memset(&me,0,sizeof(me));
    me.sin_family=AF_INET;
    me.sin_port =htons(port);
    memcpy(&me.sin_addr,myhost->h_addr,myhost->h_length);

    if ((s_waiting = socket(AF_INET,SOCK_STREAM,0))< 0)
    {
        fprintf(stderr,"socket allocation failed.\n");
        return -1;
    }

    if (bind(s_waiting, (struct sockaddr*)&me,sizeof(me))==-1)
    {
        fprintf(stderr,"cannot bind.\n");
        return -1;
    }

    fprintf(stderr,"successfully bound,now listens.\n");

    listen(s_waiting,1);
    s=accept(s_waiting,NULL,NULL);
    close(s_waiting);
    return s;
}