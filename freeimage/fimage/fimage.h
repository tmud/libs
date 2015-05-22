#pragma once

typedef void* fimage;

fimage fimage_load(const wchar_t* file);
void fimage_render(HDC dc, fimage fi, int x, int y);
void fimage_unload(fimage fi);
