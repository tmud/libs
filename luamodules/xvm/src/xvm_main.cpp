#include "common/system/pch.h"
#include "common/system/windll.h"
#include "common/lua/luamodule.h"

int create(lua_State *L)
{
    return 0;
}

extern "C" LUAMOD_API int luaopen_xvm (lua_State *L) 
{
    lua_register(L, "create", create);
    return 0;
}
