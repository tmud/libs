#include "pch.h"
#include "socket.h"
#include "listen_socket.h"

std::vector<ListenSocket*> listen_sockets;
SocketSelector selector;

class NetworkCleaner {
public:
    NetworkCleaner() : initialized(false) {}
    ~NetworkCleaner() 
    {
        if (!initialized)
            return;
        for (int i=0,e=listen_sockets.size(); i<e; ++i)
            delete listen_sockets[i];            
        release_network();        
    }
    bool init()
    {
        if (initialized)
            return true;
        initialized = init_network();
        return initialized;
    }
private:
    bool initialized;
} _network;

//------------------------------------------------------------------------------
int netserver_open(lua_State *L)
{
    if (!luaT_check(L, 2, LUA_TNUMBER, LUA_TTABLE))
        return luaT_error(L, "netserver: listen [incorrect parameters]");
    if (!_network.init())
        return luaT_error(L, "netserver: listen [network not initialized]");
    int port = lua_tointeger(L, 1);
    if (port < 0 || port > 65535)
        return luaT_errorf(L, "netserver: listen [incorrect (%d) port number]", port);

    for (int i=0,e=listen_sockets.size(); i<e; ++i) 
    {
        ListenSocket *s = listen_sockets[i];
        if (s->port == port) 
            return luaT_errorf(L, "netserver: listen [port (%d) busy]", port);
    }

    ListenSocket *new_socket = NULL;
    Socket s;
    if (s.create())
    {
       if (s.setreuseopt() &&
           s.bind(port) &&
           s.setnonblockopt() &&
           s.listen(10))
       {
           new_socket = new (std::nothrow) ListenSocket();
           if (new_socket)
           {
               new_socket->socket = s;
               new_socket->port = port;
               listen_sockets.push_back(new_socket);
               selector.AddSocket(s);
           }
       }
       if (!new_socket)
           s.close();
    }
    if (!new_socket)
        return luaT_errorf(L, "netserver: listen [port (%d) does't opened]", port);
    
    lua_pushboolean(L, 1);
    return 1;
}

int netserver_close(lua_State *L)
{
    if (!luaT_check(L, 1, LUA_TNUMBER))
       return luaT_error(L, "netserver: close [incorrect parameter]");
     
    int index = -1;
    int port = lua_tointeger(L, -1);
    for (int i=0,e=listen_sockets.size(); i<e; ++i) 
    {
        ListenSocket *s = listen_sockets[i];
        if (s->port == port && !s->closing) { index = i; break; }
    }

    if (index == -1)
        return luaT_errorf(L, "netserver: close [incorrect (%d) port number]", port);

    ListenSocket *s = listen_sockets[index];
    s->socket.shutdown(HOWTO_RECEIVE);
    s->closing = true;
    
    return 0;
}

int netserver_check(lua_State *L)
{
    if (!luaT_check(L, 1, LUA_TNUMBER))
       return luaT_error(L, "netserver: check [incorrect parameter]");
    int mseconds = lua_tointeger(L, -1);
    selector.Check(mseconds);
    return 0;
}

int netserver_gc(lua_State *L)
{
    return 0;
}

const luaL_Reg netserver_methods[] = 
{
    {"open",  netserver_open},
    {"close", netserver_close},
    {"check", netserver_check},
    {0,0}
};

extern "C" LUAMOD_API int luaopen_netserver (lua_State *L) 
{
    luaL_newlib(L, netserver_methods);
    lua_newtable(L);
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, netserver_gc);
    lua_settable(L, -3);    
    lua_setmetatable (L, -2);    
    lua_setglobal(L, "netserver");
    return 0;
}
//------------------------------------------------------------------------------
int socket_send(lua_State *L)
{
    return 0;
}

int socket_gc(lua_State *L)
{
    return 0;
}

const luaL_Reg sockets_methods[] = 
{
    {"send", socket_send},
    {"__gc", socket_gc},
    {0,0}
};

