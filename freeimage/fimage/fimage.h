#pragma once

typedef void* fimage;

fimage fimage_load(const wchar_t* file);
void   fimage_unload(fimage fi);
