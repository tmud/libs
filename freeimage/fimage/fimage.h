#pragma once

void fimage_init();
void fimage_release();

typedef void* fimage;                                   // support jpg, png, bmp, gif, ico
fimage fimage_load(const wchar_t* file, int extra);     // extra - ico (size of icon), gif (number of frame), other ignored.
void fimage_unload(fimage fi);                          // unload image
fimage fimage_cut(fimage fi, int x, int y, int w, int h); // cut subimage from another
int  fimage_width(fimage fi);                           // width of image
int  fimage_height(fimage fi);                          // height
int  fimage_memsize(fimage fi);                         // size of image in memory

struct fimage_render_ex                                 // render extends parameters
{
    int w, h;                                           // scaling (width/height of dest. rect); w/h=0 - default (no scale)
    int sx, sy;                                         // source image position (part of full image); sx/sy=0 - default
    int sw, sh;                                         // source image size, sw/sh=0 - default (full image size)
};
int fimage_render(HDC dc, fimage fi, int x, int y, fimage_render_ex *p); // p can be NULL, return value - result (0-error,!=0-ok)

#ifndef FIMAGE_EXPORTS
#ifdef _DEBUG
#pragma comment(lib, "fimaged.lib")
#else
#pragma comment(lib, "fimage.lib")
#endif
#endif
