// session_man.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h> 

#include "session_man.h"
#include "race.h"

static int sock[MAX_PLAYERS];
static char name[MAX_PLAYERS][MAX_NAME_LENGTH];
static PLAYER p[MAX_PLAYERS];

static int width;
static fd_set mask;
static fd_set readOk;

static int total_lap;
static int num_player;
static int finished;
static int retired;
static GRADE g[MAX_PLAYERS];

static void recv_data();
static void send_data();
static void end();

// 이름을 입력하게 login_msg 보냄
void enter(int i, int fd){
    static char *login_msg = "Type your name : ";
    char msg[32];
    int len;

    sock[i] = fd;
    // 서버가 client에 type your name 문자열 보냄
    write(sock[i], login_msg, strlen(login_msg));
    
    if ((len = read(sock[i], name[i], MAX_NAME_LENGTH)) <= 0) 
    {
        close(fd);
        return;
    }
    // 플레이어 이름 순번대로 문자열로 저장
    sprintf(msg, "%d", i);
    // 플레이어에게 순번을 보내줌
    write(sock[i], msg, strlen(msg)+1);  // '\0'까지 보내기 위해 +1 함
    printf("[%d] %s\n", i, name[i]);
}

//플레이어 별 초기화 및 준비시키고 총 랩,플레이어 수 client write하고 종료
void init_session(int n, int lap, int maxfd){
    static char msg[10];
    static char buf[10];
    
    num_player = n;
    total_lap = lap;
    
    width = maxfd + 1;
    // select가 감시할 파일 디스크립터 목록을 만들기 위해 위해 초기화
    FD_ZERO(&mask);
    // 파일 디스크립터 0을 감시 목록에 추가
    FD_SET(0, &mask);
    // 각 플레이어의 통신용 소켓을 감시목록에 추가(client의 통신용 소켓과는 다름)
    for(int i=0; i<num_player; i++)
        FD_SET(sock[i], &mask);
    // msg 문자열에 플레이어 수 총 lap수 저장
    sprintf(msg, "%d %d", num_player, total_lap);
    
    // 준비된 최대 플레이어 수 및 준비된 counter 0으로 초기화
    int ready[MAX_PLAYERS] = {0};
    int ready_counter = 0;

    while (ready_counter < num_player) {
        readOk = mask;
        if (select(width, &readOk, NULL, NULL, NULL) == -1)
            return;

        for (int i = 0; i < num_player; i++) {
            if (!FD_ISSET(sock[i], &readOk))
                continue;
            // 플레이어별로 read 'OK' 소켓을 읽음
            int n = read(sock[i], buf, sizeof(buf) - 1);
            if (n <= 0)
                continue;

            buf[n] = '\0';

            if (!ready[i] && strcmp(buf, "OK") == 0) {
                ready[i] = 1;
                ready_counter++;
                //ready_counter 증가시키고 damage및 플레이어별 조건들 초기화
                p[i].lap = 1;
                p[i].x = 2+i;
                p[i].y = 20;
                p[i].damage = 0;
                printf("Player [%d] is Ready! (%d/%d)\n", i, ready_counter, num_player);
            }
        }
    }

    for (int i = 0; i < num_player; i++)
        // 플레이어 수 총 lap수 문자열로 보내기
        write(sock[i], msg, strlen(msg));
    // 종료 retired 0 처리
    finished = 0;
    retired = 0;
}

//
void session_man(){
    while(finished + retired < num_player){
        recv_data();
        send_data();
    }
    end();
}
// 플레이어 temp 경기중 데이터 받아서 데이터 반영
static void recv_data(){
    int rank;
    readOk = mask;
    // 경기 중에도 여러 클라이언트의 입력을 계속 감시하기 위해 select함
    if (select(width, &readOk, NULL, NULL, NULL) == -1)
        return;

    for(int i=0; i<num_player; i++){
        // mask안에 소켓이 들어 있으면  player temp 소켓 읽어서 플레이어 구조체에 저장
        if (FD_ISSET(sock[i], &readOk)) {
            int n = read(sock[i], &p[i], sizeof(PLAYER));

            // 오류 처리 조건
            if (n <= 0)
                continue;
            // 데미지를 최대로 받으면 retired 횟수 늘림
            if (p[i].damage >= MAX_DAMAGE){
                rank = num_player - retired - 1;
                g[rank].entrynum = i;
                strcpy(g[rank].name, name[i]);
                retired++;
            } 
            // 만약 플레이어 lap수가 최대 lap보다 크다면 랭크 = finished 1증가  
            if (p[i].lap > total_lap){
                rank = finished;
                g[rank].entrynum = i;
                strcpy(g[rank].name, name[i]);
                finished++;
            }
        }
    }
}
// 
static void send_data(){
    for(int i=0; i<num_player; i++){
        // 만약 readOk에 소켓 있으면 recv_data에서 반영된 데이터 다시 소켓에 입력해서 보내기
        if (FD_ISSET(sock[i], &readOk))
            write(sock[i], p, sizeof(PLAYER)*num_player);
    }
}

// 모든 플레이어의 g 구조체 보내기
static void end(){
    for(int i=0; i<num_player; i++)
        write(sock[i], g, sizeof(GRADE)*num_player);
}