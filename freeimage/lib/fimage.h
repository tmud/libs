#pragma once

typedef void* fimage;
fimage fimage_load(const wchar_t* file, int extra);
void fimage_render(HDC dc, fimage fi, int x, int y);
void fimage_renderex(HDC dc, fimage fi, int x, int y, int w, int h);
void fimage_unload(fimage fi);
int fimage_width(fimage fi);
int fimage_height(fimage fi);
int fimage_memsize(fimage fi);

#ifndef FIMAGE_EXPORTS
#pragma comment(lib, "fimage.lib")
#endif
