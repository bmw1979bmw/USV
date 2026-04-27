#pragma once

extern "C" {
void __cdecl qsort_s (
    void *base,
    size_t num,
    size_t width,
    int (__cdecl *comp)(void *, const void *, const void *),
    void *context
    );

}