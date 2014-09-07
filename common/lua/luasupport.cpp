#include "luasupport.h"
#include <stdarg.h>
#include <vector>
#include <string>

int luaT_error(lua_State *L, const char* msg)
{
	lua_pushnil(L);
	lua_pushstring(L, msg);
	return 2;
}

int luaT_errorf(lua_State *L, const char* fmt, ...)
{
    char buffer[16];
    va_list args;
    va_start(args, fmt);

    std::string msg;
    const char *f = fmt;
    while (fmt && *fmt) 
    {
        const char *f = strchr(fmt, '%');
        if (!f)
        {
            msg.append(fmt);
            break;
        }
        else
        {
            msg.append(fmt, f-fmt);

            int pos = strcspn(f+1, " %");
            if (pos == strlen(f+1))
                pos++;
            std::string tp(f, pos);
            switch (tp.at(pos-1))
            {
                case 's':
                    msg.append(va_arg(args, char*));
                break;
                case 'd':
                    sprintf(buffer, tp.c_str(), va_arg(args, int));
                    msg.append(buffer);
                break;
                case 'f':
                    sprintf(buffer, tp.c_str(), va_arg(args, double));
                    msg.append(buffer);
                break;
                default:
                    va_arg(args, void*);
                break;
            }
            fmt = f + pos;
        }
    }
    va_end(args);
    return luaT_error(L, msg.c_str());
}

struct luaT_object
{
    void *object;
    int type;
};

int luaT_pushobject(lua_State *L, void* object, int type, const char* mt_name)
{
    luaT_object *o = (luaT_object*)lua_newuserdata (L, sizeof(luaT_object));
    o->object = object;
    o->type = type;
    luaL_getmetatable(L, mt_name);
    lua_setmetatable(L, -2);
    return 1;
}

bool luaT_check(lua_State *L, int n, ...)
{ 
    if (lua_gettop(L) < n)
        return false;

    bool result = true;
    va_list args;
    va_start(args, n);
    for (int i=1; i<=n; ++i)
    {        
        int type = va_arg(args, int);
        int stype = lua_type(L, i);
        if (type == LUA_TUSERDATA && stype == LUA_TUSERDATA)
            continue;
        if (stype == LUA_TUSERDATA)
        {
            luaT_object *o = (luaT_object*)lua_touserdata(L, i);
            stype = o->type;
        }        
        if (stype  != type)
            { result = false; break; }
    }
    va_end(args);
    return result;
}

#if defined(_DEBUG) && defined(PLATFORM_WINDOWS)
#include <windows.h>
void luaT_showLuaStack(lua_State* L, const char* label)
{
    char dbuf[128];
    sprintf(dbuf, "Code label: %s\r\n", label);
    OutputDebugStringA(dbuf);
    int n = lua_gettop(L);
    sprintf(dbuf, "Stack size: %d\r\n", n);
    OutputDebugStringA(dbuf);

    int j = -1;
    for (int i=n; i>=1; --i)
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
        std::string t(dbuf);
        sprintf(dbuf, "[%d][%d] %s\r\n", i, j--, t.c_str());
        OutputDebugStringA(dbuf);
    }
}
#endif
