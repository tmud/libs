#include "stdafx.h"
#include "multimap.h"
#include <stdio.h>
#pragma warning(disable: 4996)

unsigned DLL_CALLCONV
myReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
    return (unsigned)fread(buffer, size, count, (FILE *)handle);
}
unsigned DLL_CALLCONV
myWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
    return (unsigned)fwrite(buffer, size, count, (FILE *)handle);
}
int DLL_CALLCONV
mySeekProc(fi_handle handle, long offset, int origin) {
    return fseek((FILE *)handle, offset, origin);
}
long DLL_CALLCONV
myTellProc(fi_handle handle) {
    return ftell((FILE *)handle);
}

FIBITMAP* loadMultimapImageU(const wchar_t* file, FREE_IMAGE_FORMAT fif, int extra)
{
    FreeImageIO io;
    io.read_proc = myReadProc;
    io.write_proc = myWriteProc;
    io.seek_proc = mySeekProc;
    io.tell_proc = myTellProc;
   
    // Open src stream in read-only mode
    FILE *hfile = _wfopen(file, L"rb");
    if (hfile == NULL) 
        return NULL;

    FIBITMAP *result = NULL;
    FIMULTIBITMAP *mdip = FreeImage_OpenMultiBitmapFromHandle(fif, &io, (fi_handle)hfile);
    if (mdip)
    {
        int count = FreeImage_GetPageCount(mdip);   
        if (fif == FIF_GIF)
        {
            if (extra >=0 && extra < count)
            {
                FIBITMAP *dip = FreeImage_LockPage(mdip, extra);
                result = FreeImage_Clone(dip);
                FreeImage_UnlockPage(mdip, dip, FALSE);
            }
        }
        if (fif == FIF_ICO)
        {   
            for (int i = 0; i < count; ++i)
            {
                FIBITMAP *dip = FreeImage_LockPage(mdip, i);
                if (!dip) continue;
                int w = FreeImage_GetWidth(dip);
                int h = FreeImage_GetHeight(dip);
                if (w == extra && h == extra) {
                    result = FreeImage_Clone(dip);
                }
                FreeImage_UnlockPage(mdip, dip, FALSE);
                if (result) break;
            }
        }
        FreeImage_CloseMultiBitmap(mdip);
    }
    fclose(hfile);
    return result;
}
