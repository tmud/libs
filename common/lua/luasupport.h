#pragma once

#include "luabase.h"
bool luaT_check(lua_State *L, int n, ...);
int luaT_error(lua_State *L, const char* msg);		// return nil and error msg
int luaT_errorf(lua_State *L, const char* fmt, ...);
int  luaT_pushobject(lua_State *L, void* object, int type, const char* mt_name);

#if defined(_DEBUG) && defined(PLATFORM_WINDOWS)
void luaT_showLuaStack(lua_State* L, const char* label);
#define SS(L,n) luaT_showLuaStack(L,n)
#else
#define SS(L,n)
#endif
