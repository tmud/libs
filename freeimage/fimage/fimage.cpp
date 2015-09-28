#include "stdafx.h"
#include "fimage.h"
#include "multimap.h"
#pragma comment(lib, "msimg32.lib")

FIBITMAP* get(fimage fi) { return (FIBITMAP*)(fi); }

fimage fimage_load(const wchar_t* file, int extra)
{
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeU(file);
    if (fif == FIF_UNKNOWN)
        return NULL;
    if (fif == FIF_GIF || (fif == FIF_ICO && extra > 0))
    {
        if (extra < 0) return NULL;
        return loadMultimapImageU(file, fif, extra);
    }
    FIBITMAP *dib = FreeImage_LoadU(fif, file);
    unsigned int bpp = FreeImage_GetBPP(dib);
    if (bpp > 32 || (FreeImage_IsTransparent(dib) && bpp <= 8 && fif == FIF_PNG))
    {
        FIBITMAP* new_dib = FreeImage_ConvertTo32Bits(dib);
        FreeImage_Unload(dib);
        dib = new_dib;
    }
    return dib;
}

void fimage_unload(fimage fi)
{
    FIBITMAP *dib = get(fi);
    if (dib)
        FreeImage_Unload(dib);
}

fimage fimage_cut(fimage fi, int x, int y, int w, int h)
{
    FIBITMAP *dib = get(fi);
    if (!dib) return NULL;
    FIBITMAP *new_dib = FreeImage_Copy(dib, x, y, x+w, y+h);
    return new_dib;
}

void fimage_render_master(HDC dc, FIBITMAP* dib, int x, int y, int w, int h, int from_x, int from_y)
{
    if (!FreeImage_IsTransparent(dib))
    {
         SetStretchBltMode(dc, COLORONCOLOR);
         StretchDIBits(dc, x, y, w, h, from_x, from_y, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib),
         FreeImage_GetBits(dib), FreeImage_GetInfo(dib), DIB_RGB_COLORS, SRCCOPY);
    }
    else
    {
        HBITMAP bitmap = CreateDIBitmap(dc, FreeImage_GetInfoHeader(dib),
            CBM_INIT, FreeImage_GetBits(dib), FreeImage_GetInfo(dib), DIB_RGB_COLORS);
        HDC dcMem = CreateCompatibleDC(dc);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(dcMem, bitmap);

        unsigned int bpp = FreeImage_GetBPP(dib);
        if (bpp == 32)
        {
            //alpha per pixel render
            BLENDFUNCTION bf;
            bf.AlphaFormat = AC_SRC_ALPHA;
            bf.BlendFlags  = 0;
            bf.SourceConstantAlpha = 255;
            bf.BlendOp = AC_SRC_OVER;
            AlphaBlend(dc, x, y, w, h, dcMem, from_x, from_y, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), bf);
        }
        else
        {
            int tindex = FreeImage_GetTransparentIndex(dib);
            if (tindex != -1)
            {
                RGBQUAD *pal = FreeImage_GetPalette(dib);
                RGBQUAD tcolor = pal[tindex];
                COLORREF tc = RGB(tcolor.rgbRed, tcolor.rgbGreen, tcolor.rgbBlue);
                TransparentBlt(dc, x, y, w, h, dcMem, from_x, from_y, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), tc);
            }
            else
            {
                if (FreeImage_HasBackgroundColor(dib))
                {
                   RGBQUAD bc;
                   FreeImage_GetBackgroundColor(dib, &bc);
                   COLORREF bcolor = RGB(bc.rgbRed, bc.rgbGreen, bc.rgbBlue);
                   TransparentBlt(dc, x, y, w, h, dcMem, from_x, from_y, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), bcolor);
                }
                else
                {
                   SetStretchBltMode(dc, COLORONCOLOR);
                   StretchDIBits(dc, x, y, w, h, from_x, from_y, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib),
                   FreeImage_GetBits(dib), FreeImage_GetInfo(dib), DIB_RGB_COLORS, SRCCOPY);
                }
            }
        }
        SelectObject(dcMem, hOldBitmap);
        DeleteDC(dcMem);
        DeleteObject(bitmap);
    }
}

void fimage_render(HDC dc, fimage fi, int x, int y, fimage_render_ex *p)
{
    FIBITMAP *dib = get(fi);
    if (!dib) return;
    if (!p)
        fimage_render_master(dc, dib, x, y, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), 0, 0);
    else
    {
        if (p->w <= 0 || p->h <= 0)
          fimage_render_master(dc, dib, x, y, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), max(0, p->sx), max(0, p->sy));
        else
          fimage_render_master(dc, dib, x, y, p->w, p->h, max(0, p->sx), max(0, p->sy));  
    }
}

int fimage_width(fimage fi)
{
    FIBITMAP *dib = get(fi);
    if (!dib) return 0;
    unsigned int w = FreeImage_GetWidth(dib);
    return static_cast<int>(w);
}

int fimage_height(fimage fi)
{
    FIBITMAP *dib = get(fi);
    if (!dib) return 0;
    unsigned int w = FreeImage_GetHeight(dib);
    return static_cast<int>(w);
}

int fimage_memsize(fimage fi)
{
    FIBITMAP *dib = get(fi);
    if (!dib) return 0;
    unsigned int ms = FreeImage_GetMemorySize(dib);
    return static_cast<int>(ms);
}

void fimage_init()
{
    FreeImage_Initialise(TRUE);
}

void fimage_release()
{
    FreeImage_DeInitialise();
}

/*BOOL APIENTRY DllMain(HMODULE hModule,
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
}*/
