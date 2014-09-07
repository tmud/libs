#ifdef PLATFORM_FREEBSD
#include "socket.h"

bool init_network()
{
	return true;
}

void release_network()
{
}

SocketSelector::SocketSelector() : maxfds(-1)
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
    sdb.insert(sock);
    if (sock > maxfds) maxfds = sock;
}

void SocketSelector::DelSocket(SOCKET sock)
{
    FD_CLR(sock, &current_set);
    sdb.erase(sock);
    if (sock == maxfds)
    {
        std::set<SOCKET>::iterator it = std::max_element(sdb.begin(), sdb.end());
        if (it == sdb.end())
            maxfds = -1;
        else
            maxfds = *it;
    }
}

int SocketSelector::Check(int mseconds)
{
    rd_set = current_set;
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = mseconds;
    int n = select(maxfds + 1, &rd_set, NULL, NULL, &tv );
    if (n < 0) { n = -1; }
    return n;   
}

bool SocketSelector::IsRd(SOCKET sock)
{
    return FD_ISSET(sock, &rd_set) ? true : false;
}

Socket::Socket() : s(-1) {}
Socket::~Socket() {}

int Socket::create()
{
    if (s == -1)
        s = socket(AF_INET, SOCK_STREAM, 0);
    return (s < 0) ?  0 : 1;
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
    socklen_t peerlen = sizeof(peer);
    SOCKET ns = ::accept(s, (sockaddr *)&peer, &peerlen);
    if (ns != -1)
    {
        newsocket->s = ns;
        if (info)
        {
            info->port = peer.sin_port;
            strcpy(info->ip_address, inet_ntoa(peer.sin_addr));
        }
    }
    return (ns != -1) ? 1 : 0;
}

int Socket::send(const uchar* buf, int len, int *sended)
{
    int rc = ::send(s, (const char*)buf, len, 0);
    if (rc == -1) *sended = 0;
    else *sended = rc;
    return (rc == -1) ? 0 : 1;
}

int Socket::recv(uchar *buf, int len, int *recieved)
{
    int rc = ::recv(s, (char*)buf, len, 0);
    if (rc == -1) 
    {
        *recieved = 0;        
    }
    else if (rc == 0)  
    {
        *recieved = 0;
        return 0;
    }
    else *recieved = rc;    
    return (rc == -1) ? 0 : 1;
}

int Socket::setreuseopt()
{
    int opt = 1;
    return (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) ? 0 : 1;
}

int Socket::setnonblockopt()
{
    int flags = fcntl(s, F_GETFL, 0);
    if (flags < 0) return 0;
    flags |= O_NONBLOCK;
    return fcntl(s, F_SETFL, flags) < 0 ? 0 : 1;
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
    return (::close(s)) ? 0 : 1;
}

int Socket::isinvalid()
{
    return (s == -1) ? 1 : 0;
}

void Socket::markinvalid()
{
    s = -1;
}

#endif
