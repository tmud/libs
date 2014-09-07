
#pragma warning(disable: 4005)
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "common/lua/luabase.h"
#include <string>

int main(int argc, char **argv)
{
    if (argc != 2)
        { printf("tester.exe <script.lua>\r\n");  return 0; } 

    std::string p("..\\tests\\");
    p.append(argv[1]);

    lua_State *L = luaL_newstate();
    luaopen_package(L);
    if ( luaL_dofile(L, p.c_str()) != 0)
        printf("tester.exe: Finished with error\r\n");

    lua_close(L);
	return 0;
}
