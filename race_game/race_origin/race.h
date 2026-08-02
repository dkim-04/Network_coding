// race.h

#ifndef RACE_H
#define RACE_H

#define MAX_DAMAGE       10
#define MAX_PLAYERS      10
#define MAX_TOTAL_LAPS   10
#define MAX_NAME_LENGTH  16

#define PORT  10003


typedef struct{
    int lap;
    int x, y;
    int damage;
} PLAYER;

typedef struct{
    int entrynum;
    char name[MAX_NAME_LENGTH];
} GRADE;

#endif
