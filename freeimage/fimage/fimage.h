#pragma once

void fimage_init();
void fimage_release();

typedef void* fimage;                                   // support jpg, png, bmp, gif, ico
fimage fimage_load(const wchar_t* file, int extra);     // extra - ico (size of icon), gif (number of frame), other ignored.
void fimage_unload(fimage fi);                          // unload image
fimage fimage_cut(fimage fi, int x, int y, int w, int h); // cut subimage from another
int  fimage_width(fimage fi);                           // width of image
int  fimage_height(fimage fi);                          // height of images
int  fimage_memsize(fimage fi);                         // size of image in memory

struct fimage_render_ex                                 // render extends parameters
{
    int w, h;                                           // scaling (width/height of dest. rect); w/h=0 - default (no scale)
    int sx, sy;                                         // source image position (part of full image); sx/sy=0 - default
    int sw, sh;                                         // source image size, sw/sh=0 - default (full image size)
};
int fimage_render(HDC dc, fimage fi, int x, int y, fimage_render_ex *p); // p can be NULL, return value - result (0-error,!=0-ok)

int  fimage_getpixel(fimage fi, int x, int y, COLORREF* c);// get image pixel (16/24/32 bits images), return 0/1 - false/true
int  fimage_setpixel(fimage fi, int x, int y, COLORREF c); // set image pixel (16/24/32 bits images), return 0/1 - false/true
int  fimage_getindex(fimage fi, int x, int y, int *index); // get color index (1/4/8 bits images)
int  fimage_setindex(fimage fi, int x, int y, int index);  // set color index (1/4/8 bits images)
int  fimage_getpalette(fimage fi, int index, COLORREF* c); // get index color
int  fimage_setpalette(fimage fi, int index, COLORREF c);  // set index color

#ifndef FIMAGE_EXPORTS
#ifdef _DEBUG
#pragma comment(lib, "fimaged.lib")
#else
#pragma comment(lib, "fimage.lib")
#endif
#endif
