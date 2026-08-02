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


    Note over Server: m_accept에서 enter()로 인해<br/>hostname 입력 요청 메시지 전송
    Server->>Client: write(sock,msg)
    Note over Client: hostname 입력 요청 메시지 수신

    Client->>Client: read(sock,buf)
    Note over Client: 이름 입력 후 '\n' -> '\0'<br/>전환하여 이름 전송

    Client->>Server: write(sock,buf)
```

