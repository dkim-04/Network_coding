# Race_game 코드 흐름


## 1. 서버 동작 과정
```mermaid
flowchart LR
    main --> input_hostname
    input_hostname --> setup_m_srv
    setup_m_srv --> m_accept
    m_accept --> init_session
    init_session --> session_man
```

## 2. client 동작 과정
```mermaid
flowchart LR
    main --> input_hostname
    input_hostname --> setup_client
    setup_client --> init_session
    init_session --> session
```

## 3. Client-Server 통신 과정
```mermaid
sequenceDiagram
    participant Server
    participant Client

    Note over Server: m_accept()가 enter()를 호출

    Server->>Client: write(sock, msg)<br/>hostname 입력 요청 메시지 전송

    Client->>Client: read(sock, buf)<br/>hostname 입력 요청 메시지 수신

    Client->>Server: write(sock, buf)<br/>hostname 전송

    Server->>Server: read(sock, buf)<br/>클라이언트가 보낸 hostname 수신

    Server->>Client: write(sock,msg)<br/>각 클라이언트에 플레이어 순번 전송
    
    Client->>Client: read(sock, buf)<br/>자신의 순번 수신
    
    Client->>Server: write(sock, buf)<br/>서버에 'OK'문자열 전송

    Server->>Server: read(sock, buf)<br/>클라이언트가 보낸 "OK" 수신

    Server->>Client: write(sock,msg)<br/>플레이어 총 수 및 총 랩 수 문자열 전송

    Client->>Client: read(sock, buf)<br/>플레이어 총 수 및 총 랩 수 문자열 수신 및 정수로 전환

    Client->>Server: write(sock, buf)<br/>send_my_data()에서 -> Player temp 구조체 전송

     Server->>Server: read(sock, buf)<br/>각 플레이어별 recv_data()로 Player temp 구조체 수신 및 P구조체 덮어씌우기
    
    Server->>Client: write(sock,buf)<br/> send_data()로 변경된 p_구조체 전송

    Client->>Client: read(sock, buf)<br/>recv_all_data()로 P구조체 수신 및 덮어씌우기

    Client->>Server: write(sock, buf)<br/>send_my_data()-> recv_all_data() 반복
    
     Server->>Client: write(sock,buf)<br/>send_my_data()-> recv_all_data() 반복

    Server->>Client: write(sock,buf)<br/>end()에서 각 플레이어에게 g구조체 전송

     Client->>Client: read(sock, buf)<br/>game_over()에서 g구조체 수신 받고 결과 출력 후 종료
    
    
```

