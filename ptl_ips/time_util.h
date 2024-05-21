#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <stdint.h>

void mark_init();

void mark_destroy();

void mark_begin(void* ptr);

void mark_end(void* ptr);

#endif // TIME_UTIL_H
