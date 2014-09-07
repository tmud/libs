#pragma once

#ifdef PLATFORM_WINDOWS
#define WINVER	0x0501					// support winXP+
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
