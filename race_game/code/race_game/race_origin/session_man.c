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

void enter(int i, int fd){
    static char *login_msg = "Type your name : ";
    char msg[32];
    int len;

    sock[i] = fd;
    write(sock[i], login_msg, strlen(login_msg));
    
    if ((len = read(sock[i], name[i], MAX_NAME_LENGTH)) <= 0) {
        close(fd);
        return;
    }
    
    sprintf(msg, "%d", i);
    write(sock[i], msg, strlen(msg)+1);  // '\0'까지 보내기 위해 +1 함
    printf("[%d] %s\n", i, name[i]);
}

void init_session(int n, int lap, int maxfd){
    static char msg[10];
    static char buf[10];
    
    num_player = n;
    total_lap = lap;
    
    width = maxfd + 1;
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    for(int i=0; i<num_player; i++)
        FD_SET(sock[i], &mask);
    
    sprintf(msg, "%d %d", num_player, total_lap);
    
    int ready[MAX_PLAYERS] = {0};
    int ready_counter = 0;

    while (ready_counter < num_player) {
        readOk = mask;
        if (select(width, &readOk, NULL, NULL, NULL) == -1)
            return;

        for (int i = 0; i < num_player; i++) {
            if (!FD_ISSET(sock[i], &readOk))
                continue;

            int n = read(sock[i], buf, sizeof(buf) - 1);
            if (n <= 0)
                continue;

            buf[n] = '\0';

            if (!ready[i] && strcmp(buf, "OK") == 0) {
                ready[i] = 1;
                ready_counter++;
                p[i].lap = 1;
                p[i].x = 2+i;
                p[i].y = 20;
                p[i].damage = 0;
                printf("Player [%d] is Ready! (%d/%d)\n", i, ready_counter, num_player);
            }
        }
    }
    for (int i = 0; i < num_player; i++)
        write(sock[i], msg, strlen(msg));
    finished = 0;
    retired = 0;
}

void session_man(){
    while(finished + retired < num_player){
        recv_data();
        send_data();
    }
    end();
}
// 데이터 
static void recv_data(){
    int rank;
    readOk = mask;
    if (select(width, &readOk, NULL, NULL, NULL) == -1)
        return;

    for(int i=0; i<num_player; i++){
        if (FD_ISSET(sock[i], &readOk)) {
            int n = read(sock[i], &p[i], sizeof(PLAYER));

            if (n <= 0)
                continue;
            if (p[i].damage >= MAX_DAMAGE){
                rank = num_player - retired - 1;
                g[rank].entrynum = i;
                strcpy(g[rank].name, name[i]);
                retired++;
            } 
            if (p[i].lap > total_lap){
                rank = finished;
                g[rank].entrynum = i;
                strcpy(g[rank].name, name[i]);
                finished++;
            }
        }
    }
}

static void send_data(){
    for(int i=0; i<num_player; i++){
        if (FD_ISSET(sock[i], &readOk))
            write(sock[i], p, sizeof(PLAYER)*num_player);
    }
}
static void end(){
    for(int i=0; i<num_player; i++)
        write(sock[i], g, sizeof(GRADE)*num_player);
}