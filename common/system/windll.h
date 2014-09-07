#pragma once
#ifdef PLATFORM_WINDOWS
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID)
{	
	return TRUE;
}
#endif
