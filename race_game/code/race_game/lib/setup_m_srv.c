#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>


//호스트 명, 포트 , 대기행렬 크기
// 인수로 받고 listem 까지 실행 

int setup_m_srv(hostname,port,backlog)
char *hostname;
u_short port;
int backlog;
{
    struct hostent *myhost;
    struct sockaddr_in me;

    int s; //accept용 디스크립터

    int option=1;

    if ((myhost= gethostbyname(hostname))==NULL)
    {
        fprintf(stderr,"badhostname\n");
        return -1;
    }

    memset(&me,0,sizeof(me));
    me.sin_family= AF_INET;
    me.sin_port= htons(port);
    memcpy(&me.sin_addr, myhost -> h_addr, myhost -> h_length);


    if ((s=socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        fprintf(stderr,"socket allocation failed.\n");
        return -1;
    }

    if (bind(s, (struct sockaddr*)&me,sizeof(me)) == -1)
    {
        fprintf(stderr,"cannot bind.\n");
        return -1;
    }
    fprintf(stderr,"successfully bound, now listen\n");

    if (listen(s,backlog) == -1)
    {
        fprintf(stderr, "cannot listen.\n");
        return -1;
    }
    fprintf(stderr,"listen ok, now ready to accept.\n");

    //accept 용 디스크립터 반환
    return s;

}

//멀티 억셉트

int m_accept(w_soc, limit, func)
int w_soc; //리슨한 소켓
int limit;  //accept상한
void (*func)();
{
    int i;

    int fd;

    for (i=0; i<limit; i++)
    {
        if ((fd=accept(w_soc,NULL,NULL)) == -1)
        {
            fprintf(stderr,"accept error\n");
            return -1;
        }
        (*func)(i,fd);
    }
    close(w_soc);
    return fd;
}