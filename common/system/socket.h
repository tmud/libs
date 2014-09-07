// #define PLATFORM_WINDOWS
// #define PLATFORM_FREEBSD
// #define PLATFORM_LINUX

#ifndef SOCKET_H
#define SOCKET_H

#ifdef PLATFORM_WINDOWS
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define FD_SETSIZE 16384
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
#endif

#if defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX)
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef int SOCKET;
#endif

#include <assert.h>
#include <set>

bool init_network();
void release_network();

class SocketSelector
{
public:
    SocketSelector();
    ~SocketSelector();
    void   AddSocket(SOCKET sock);
    void   DelSocket(SOCKET sock);
    int    Check(int mseconds);
    bool   IsRd(SOCKET sock);

private:
    fd_set current_set;
    fd_set rd_set;

#if defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX)
    int   maxfds;
    std::set<SOCKET> sdb;
#endif
};

struct SocketInfo
{
    SocketInfo() : port(0) { memset(ip_address, 0, 32); }
    char ip_address[32];
    int  port;
};

enum SocketHowto { HOWTO_RECEIVE = 0, HOWTO_SEND, HOWTO_BOTH };
class Socket
{
public:
    Socket();
    ~Socket();
    operator SOCKET() { return s; }

    int  create();
    int  bind(int port);
    int  listen(int backlog = 5);
    int  accept(Socket *newsocket, SocketInfo *info = NULL);
    int  send(const unsigned char* buf, int len, int *sended);
    int  recv(unsigned char *buf, int len, int *recieved);
    int  setreuseopt();
    int  setnonblockopt();
    int  shutdown(SocketHowto howto);
    int  close();
    int  isinvalid();
    void markinvalid();

private:
    SOCKET s;
};
#endif // SOCKET_H
