// server.c

#include <stdio.h>
#include <stdlib.h>

#include "snet.h"
#include "race.h"
#include "session_man.h"

int main(int argc, char *argv[]){
    long num_player;
    long total_lap;
    int sock;
    int maxfd;
    char *end;
    char hostname[256];

    // argc[0]= 서버 실행 파일 이름 | argc[1]= player 수 |  argc[2]= 총 lab 도는 수 
    if(argc != 3){
        fprintf(stderr, "Usage: %s <players> <total laps>\n", argv[0]);
        exit(1);
    }
    
    // 총 플레이어 수 문자열을 long으로 전환
    num_player = strtol(argv[1], &end, 10);
    // 만약 입력된 플레이어 수가 저장된 최대 플레이어보다 많다면 오류 출력 및 종료
    if (num_player <= 0 || num_player > MAX_PLAYERS) {
        fprintf(stderr, "invalid player number. please select 1 ~ %d\n", MAX_PLAYERS);
        exit(1);
    }
    // 총 랩 수 long int로 전환
    total_lap = strtol(argv[2], &end, 10);
    // 만약 총 랩이 0보다 작거나 저장된 최대값보다 크면 오류 출력 및 종료
    if (total_lap <= 0 || total_lap > MAX_TOTAL_LAPS) {
        fprintf(stderr, "invalid total laps. please select 1 ~ %d\n", MAX_TOTAL_LAPS);
        exit(1);
    }
    
    // host_name 입력받기
    if (input_hostname(hostname, sizeof(hostname)) == -1){
        fprintf(stderr, "invalid hostname\n");
        exit(1);
    }
    // 소켓
    if ((sock = setup_m_srv(hostname, PORT, num_player)) == -1){
        fprintf(stderr, "cannot setup server\n");
        exit(1);
    }

    maxfd = m_accept(sock, num_player, enter);

    init_session(num_player, total_lap, maxfd);
    session_man();
     
    return 0;
}
