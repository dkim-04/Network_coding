# 레이싱 게임 코드

### 코드 구조도
```
race_game
├──race/
    ├──session_man.c
    ├──
    ├──

```

### 함수 기능들
- 1. session_man.c
        ├── void enter(i,fd) -> m_accept()에 넘겨주는 콜백 함수
        ├── void init_session(n,fin,maxfd) -> select 감시 위해 mask 초기화해 비트마스크 준비후 초기 데이터 전원에게 보냄
        ├── void session_man() -> 참가자 최대 수 전까지 data recv -> send 하고 종료 
        ├── void recv_data() -> 
        ├── void send_dat() ->
        ├── void end() ->
- 2. server.c
        ├── main(int argc,char *argv[])