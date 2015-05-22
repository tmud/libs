#include "stdafx.h"
#include "fimage.h"

FIBITMAP* get(fimage fi) { return (FIBITMAP*)(fi); }

fimage fimage_load(const wchar_t* file)
{
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeU(file);
    if (fif == FIF_UNKNOWN)
        return NULL;
    FIBITMAP *fb = FreeImage_LoadU(fif, file);
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
    if (!fb) return;
    fimage_renderex(dc, fi, x, y, x+FreeImage_GetWidth(fb), y+ FreeImage_GetHeight(fb));
}

void fimage_renderex(HDC dc, fimage fi, int x, int y, int w, int h)
{
    FIBITMAP *fb = get(fi);
    if (!fb) return;

    if (!FreeImage_IsTransparent(fb))
    {
         SetStretchBltMode(dc, COLORONCOLOR);
         StretchDIBits(dc, x, y, w, h, 0, 0, FreeImage_GetWidth(fb), FreeImage_GetHeight(fb),
         FreeImage_GetBits(fb), FreeImage_GetInfo(fb), DIB_RGB_COLORS, SRCCOPY);
    }
    else
    {
        unsigned int bpp = FreeImage_GetBPP(fb);
        HBITMAP bitmap = CreateDIBitmap(dc, FreeImage_GetInfoHeader(fb),
            CBM_INIT, FreeImage_GetBits(fb), FreeImage_GetInfo(fb), DIB_RGB_COLORS);
        HDC dcMem = CreateCompatibleDC(dc);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(dcMem, bitmap);

        if (bpp == 32)
        {
            //alpha per pixel render
            BLENDFUNCTION bf;
            bf.AlphaFormat = AC_SRC_ALPHA;
            bf.BlendFlags  = 0;
            bf.SourceConstantAlpha = 255;
            bf.BlendOp = AC_SRC_OVER;
            AlphaBlend(dc, x, y, w, h, dcMem, 0, 0, FreeImage_GetWidth(fb), FreeImage_GetHeight(fb), bf);
        }
        else
        {
            unsigned int colors = FreeImage_GetTransparencyCount(fb);

            if (false) 
            {                
                HDC dcMemTrans = CreateCompatibleDC(dc);
                HBITMAP mask = CreateBitmap(FreeImage_GetWidth(fb), FreeImage_GetHeight(fb), 1, 1, FreeImage_GetTransparencyTable(fb));
                HBITMAP hOldBitmapMaks = (HBITMAP)SelectObject(dcMemTrans, mask);

                //BitBlt(dc, x, y, w, h, dcMem, 0, 0, SRCCOPY);

              	BitBlt(dc, x, y, w, h, dcMem, 0, 0, SRCINVERT);
            	//BitBlt(dc, x, y, w, h, dcMemTrans, 0, 0, SRCAND);
	            BitBlt(dc, x, y, w, h, dcMem, 0, 0, SRCINVERT);
           
                SelectObject(dcMemTrans, hOldBitmapMaks);                
                DeleteDC(dcMemTrans);
                DeleteObject(mask);
            }
            else 
            {

            /*if (FreeImage_HasBackgroundColor(fb))
            {
                RGBQUAD tcolor;
                FreeImage_GetBackgroundColor(fb, &tcolor);
                COLORREF tc = RGB(tcolor.rgbRed, tcolor.rgbGreen, tcolor.rgbBlue);
                TransparentBlt(dc, x, y, w, h, dcMem, 0, 0,  FreeImage_GetWidth(fb), FreeImage_GetHeight(fb), tc);
            }
            else*/
            {
                int tindex = FreeImage_GetTransparentIndex(fb);
                if (tindex != -1)
                {
                    
                    RGBQUAD *pal = FreeImage_GetPalette(fb);
                    RGBQUAD tcolor = pal[tindex];
                    COLORREF tc = RGB(tcolor.rgbRed, tcolor.rgbGreen, tcolor.rgbBlue);
                    TransparentBlt(dc, x, y, w, h, dcMem, 0, 0,  FreeImage_GetWidth(fb), FreeImage_GetHeight(fb), tc);
                }
                else
                {

                    if (FreeImage_HasBackgroundColor(fb))
                    {
                        RGBQUAD tcolor;
                        FreeImage_GetBackgroundColor(fb, &tcolor);
                        COLORREF tc = RGB(tcolor.rgbRed, tcolor.rgbGreen, tcolor.rgbBlue);
                        TransparentBlt(dc, x, y, w, h, dcMem, 0, 0,  FreeImage_GetWidth(fb), FreeImage_GetHeight(fb), tc);
                    }
                    else

                    {



                    SetStretchBltMode(dc, COLORONCOLOR);
                    StretchDIBits(dc, x, y, w, h,
                    0, 0, FreeImage_GetWidth(fb), FreeImage_GetHeight(fb),
                    FreeImage_GetBits(fb), FreeImage_GetInfo(fb), DIB_RGB_COLORS, SRCCOPY);

                    }
                }
            }}
        }

        SelectObject(dcMem, hOldBitmap);
        DeleteDC(dcMem);
        DeleteObject(bitmap);
    }
}

int fimage_width(fimage fi)
{
    FIBITMAP *fb = get(fi);
    if (!fb) return 0;
    unsigned int w = FreeImage_GetWidth(fb);
    return static_cast<int>(w);
}

int fimage_height(fimage fi)
{
    FIBITMAP *fb = get(fi);
    if (!fb) return 0;
    unsigned int w = FreeImage_GetHeight(fb);
    return static_cast<int>(w);
}

int fimage_memsize(fimage fi)
{
    FIBITMAP *fb = get(fi);
    if (!fb) return 0;
    unsigned int ms = FreeImage_GetMemorySize(fb);
    return static_cast<int>(ms);
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
