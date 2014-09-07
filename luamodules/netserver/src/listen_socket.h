#pragma once

class ListenSocket
{
public:
    ListenSocket();
    ~ListenSocket();

    int port;
    Socket socket;
    bool closing;
};
