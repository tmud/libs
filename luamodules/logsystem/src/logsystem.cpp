#include "pch.h"

void log_print(const char* log)
{
}

int log(lua_State *L)
{
    int n = lua_gettop(L);
    if (n == 0)
    {
        log_print("log without parameters");
        return 0;
    }

    /*char dbuf[128];
    for (int i=1; i<=n; ++i)
    {
        int type = lua_type(L, i);
        switch (type)
        {
           case LUA_TNIL:
             strcpy(dbuf, "nil");
             break;
           case LUA_TNUMBER:
             sprintf(dbuf, "number: %s", lua_tostring(L, i));
             break;
           case LUA_TBOOLEAN:
             sprintf(dbuf, "boolean: %s", lua_tostring(L, i));
             break;
           case LUA_TSTRING:
             sprintf(dbuf, "string: %s", lua_tostring(L, i));
             break;        
           case LUA_TUSERDATA:
             sprintf(dbuf, "userdata: 0x%x", lua_topointer(L, i));
             break;
           case LUA_TLIGHTUSERDATA:
             sprintf(dbuf, "lightuserdata: 0x%x", lua_topointer(L, i));
             break;
           case LUA_TFUNCTION:
             sprintf(dbuf, "function: 0x%x", lua_topointer(L, i));
             break;
           case LUA_TTHREAD:
             strcpy(dbuf, "thread");
             break;
           case LUA_TTABLE:
             sprintf(dbuf, "table: 0x%x", lua_topointer(L, i));
             break;        
           default:
             strcpy(dbuf, "???");
             break;
         }
    }*/

    return 0;
}

int logsystem_openlog(lua_State *L)
{
    if (!luaT_check(L, LUA_TSTRING))
        return luaT_error(L, "[logsystem] Invalid parameter");
    
    

    lua_pushinteger(L, 0);
    lua_pushcclosure(L, log, 0);
    return 1;
}

int logsystem_gc(lua_State *L)
{
    return 0;
}

const luaL_Reg netserver_methods[] = 
{
    {"openlog",  logsystem_openlog},    
    {0,0}
};

extern "C" LUAMOD_API int luaopen_logsystem (lua_State *L) 
{
    luaL_newlib(L, netserver_methods);
    lua_newtable(L);
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, logsystem_gc);
    lua_settable(L, -3);    
    lua_setmetatable (L, -2);    
    lua_setglobal(L, "logsystem");
    return 0;
}
