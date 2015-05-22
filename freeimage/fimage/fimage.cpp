#include "stdafx.h"
#include "fimage.h"

FIBITMAP* get(fimage fi) { return (FIBITMAP*)(fi); }

fimage fimage_load(const wchar_t* file)
{
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeU(file);
    FIBITMAP *fb = FreeImage_LoadU(fif, file );
    return fb;
}

void fimage_unload(fimage fi)
{
    FIBITMAP *fb = get(fi);
    if (fb)
        FreeImage_Unload(fb);
}

void fimage_render(HDC dc, fimage fi, int x, int y)
{
    FIBITMAP *fb = get(fi);

    RECT rcDest = { x, y, x+FreeImage_GetWidth(fb), y+ FreeImage_GetHeight(fb) };

    SetStretchBltMode(dc, COLORONCOLOR);
    StretchDIBits(dc, rcDest.left, rcDest.top,

    rcDest.right-rcDest.left, rcDest.bottom-rcDest.top,
    0, 0, FreeImage_GetWidth(fb), FreeImage_GetHeight(fb),
    FreeImage_GetBits(fb), FreeImage_GetInfo(fb), DIB_RGB_COLORS, SRCCOPY);

}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        FreeImage_Initialise();
        break;
    case DLL_PROCESS_DETACH:
        FreeImage_DeInitialise();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
