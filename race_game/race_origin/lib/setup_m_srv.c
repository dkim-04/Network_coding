/*****************************/
/* setup_m_srv.c */
/*****************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 

// server가 client 입장 받아들이기 전까지의 과정 준비하는 코드 (전체 인원 한명씩 다 받음)
int setup_m_srv(char* hostname, u_short port, int backlog){
    //호스트의 정보 가르키는 포인터 선언
    struct hostent *myhost; 
    //IPv4 주소(IP 주소 + 포트 번호)를 저장하는 구조체
    struct sockaddr_in me; 
    int s; 

    if ((myhost = gethostbyname(hostname)) == NULL){
        //네트워크 관련 함수 발생한 오류 출력
        herror("gethostbyname");
        return -1;
    }
    //me= 소켓 주소 정보 | 모든 바이트를 0으로 me 구조체만큼 채우기
    memset(&me, 0, sizeof(me));
    // 주소 체계를 IP4체계로 지정   
    me.sin_family = AF_INET;
    // 사용할 포트번호를 네트워크 형식으로 저장
    me.sin_port = htons(port);
    // gethostbyname으로 얻은 ip주소 sin_addr에 저장
    memcpy(&me.sin_addr, myhost->h_addr, myhost->h_length); 
    // 통신에 사용할 소켓 생성
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // 시스템 호출 or 라이브러리 함수 실패하면 실패 원인 출력
        perror("socket");
        return -1;
    }
    // 소켓에 IP 주소, 포트 번호 연결
    if (bind(s, (struct sockaddr *)&me, sizeof(me)) == -1) {
        perror("bind");
        return -1;
    }
    // client 연결 요청 받아들일 준비
    if (listen(s, backlog) == -1) {
        perror("listen");
        return -1;
    }
    fprintf(stderr, "Server is Running...\n");
    
    return s;
}
// 클라이언트 연결 수락 및 enter()함수를 이용한 client와 소켓 주고 받기
int m_accept(int soc, int limit, void (*func)(int, int)){
    int fd;
    int maxfd = soc;

    for (int i = 0; i < limit; i++) 
    {
        // client가 접속하면 연결을 수락한다
        if ((fd = accept(soc, NULL, NULL)) == -1) 
        {
            perror("accept");
            return -1;
        }
        // 만약 최대 파일디스크립터보다 크다면 고정 시키기
        if (fd > maxfd)
            maxfd = fd;
        // enter(i,fd)임 -> 1. 서버가 client name입력하라고 문자열 보냄 || client 응답받으면 
        func(i, fd);
        fprintf(stderr, "Client enters room[%d/%d]\n", i+1, limit);
    }

    close(soc);
    return maxfd;
}

int accept_client(int soc){
    int fd;
    if ((fd = accept(soc, NULL, NULL)) == -1) {
        perror("accept");
        return -1;
    }
    return fd;
}
