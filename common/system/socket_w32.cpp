#ifdef PLATFORM_WINDOWS
#include "socket.h"

bool init_network()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0) 
		return false;
	return true;
}

void release_network()
{
    WSACleanup();    
}

SocketSelector::SocketSelector()
{
    FD_ZERO(&current_set);
    FD_ZERO(&rd_set);
}

SocketSelector::~SocketSelector()
{
}

void SocketSelector::AddSocket(SOCKET sock)
{
    FD_SET(sock, &current_set);
}

void SocketSelector::DelSocket(SOCKET sock)
{
    FD_CLR(sock, &current_set);
}

int SocketSelector::Check(int mseconds)
{
    rd_set = current_set;
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = mseconds;
    int n = select(0, &rd_set, NULL, NULL, &tv );
    if (n == SOCKET_ERROR) { n = -1; }
    return n;   
}

bool SocketSelector::IsRd(SOCKET sock)
{
    return FD_ISSET(sock, &rd_set) ? true : false;
}

Socket::Socket() : s(INVALID_SOCKET) {}
Socket::~Socket() {}

int Socket::create()
{
    if (s == INVALID_SOCKET)
        s = socket(AF_INET, SOCK_STREAM, 0);
    return (s == INVALID_SOCKET) ?  0 : 1;
}

int Socket::bind(int port)
{
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    
    server_addr.sin_port = htons(port);    
    return (::bind(s, (sockaddr *)&server_addr, sizeof(server_addr))) ? 0 : 1;
}

int Socket::listen(int backlog )
{
    return (::listen(s, backlog)) ? 0 : 1;
}

int Socket::accept(Socket *newsocket, SocketInfo *info)
{
    if (!newsocket || !newsocket->isinvalid()) return 0;
    sockaddr_in peer; 
    int peerlen = sizeof(peer);
    SOCKET ns = ::accept(s, (sockaddr *)&peer, &peerlen);
    if (ns != INVALID_SOCKET)
    {
        newsocket->s = ns;
        if (info)
        {
            info->port = peer.sin_port;
            strcpy(info->ip_address, inet_ntoa(peer.sin_addr));
        }
    }
    return (ns != INVALID_SOCKET) ? 1 : 0;
}

int Socket::send(const unsigned char* buf, int len, int *sended)
{    
    int rc = ::send(s, (const char*)buf, len, 0);
    if (rc == SOCKET_ERROR) *sended = 0;
    else *sended = rc;
    return (rc == SOCKET_ERROR) ? 0 : 1;
}

int Socket::recv(unsigned char *buf, int len, int *recieved)
{
    int rc = ::recv(s, (char*)buf, len, 0);
    if (rc == SOCKET_ERROR) 
    {
        *recieved = 0;    
        if (WSAGetLastError() == WSAEWOULDBLOCK) return 1;    // нет данных
    }
    else if (rc == 0)                                         // соединение закрыто
    {
        *recieved = 0;
        return 0;
    }
    else *recieved = rc;
    return (rc == SOCKET_ERROR) ? 0 : 1;
}

int Socket::setreuseopt()
{
    int opt = 1;
    return (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) ? 0 : 1;
}

int Socket::setnonblockopt()
{
    unsigned long val = 1;
    return (ioctlsocket(s, FIONBIO, &val)) ? 0 : 1;
}

int Socket::shutdown(SocketHowto howto) 
{
    int ht = -1;
    switch (howto)
    {
       case HOWTO_RECEIVE:
           ht = 0;
       break;
       case HOWTO_SEND:
           ht = 1;
       break;
       case HOWTO_BOTH:
           ht = 2;
       break;
    }

    if (ht == -1) return 0;        
    return (::shutdown(s, ht)) ? 0 : 1;
}

int Socket::close()
{
    return (closesocket(s)) ? 0 : 1;
}

int Socket::isinvalid()
{
    return (s == INVALID_SOCKET) ? 1 : 0;
}

void Socket::markinvalid()
{
    s = INVALID_SOCKET;
}

#endif // PLATFORM_WINDOWS
