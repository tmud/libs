#pragma once

void fimage_init();
void fimage_release();

typedef void* fimage;                                   // support jpg, png, bmp, gif, ico
fimage fimage_load(const wchar_t* file, int extra);     // extra - ico (size of icon), gif (number of frame), other ignored.
void fimage_render(HDC dc, fimage fi, int x, int y);    // render without scaling
void fimage_renderex(HDC dc, fimage fi, int x, int y, int w, int h);    // render in rect with rescalling
void fimage_unload(fimage fi);                          // uload image
int  fimage_width(fimage fi);                           // width of image
int  fimage_height(fimage fi);                          // height
int  fimage_memsize(fimage fi);                         // size of image in memory

#ifndef FIMAGE_EXPORTS
#ifdef _DEBUG
#pragma comment(lib, "fimaged.lib")
#else
#pragma comment(lib, "fimage.lib")
#endif
#endif
