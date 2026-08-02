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

    
    
```

