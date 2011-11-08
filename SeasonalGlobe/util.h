#ifndef __UTIL_H__
#define __UTIL_H__

#include "ctypes.h"

#define EPSILON 0.000001f // rough epsilon

void SAFE_DELETE(const void *p);
void SAFE_DELETE_ARRAY(const void *p);

void set_bit(i32 &opt, const u32 bit);
i32 bit_set(const i32 &opt, const u32 bit);
void clear_bit(i32 &opt, const u32 bit);
void toggle_bit(i32 &opt, const u32 bit);

void clamp(u32 &v, u32 lower, u32 upper);
void clamp(i32 &v, i32 lower, i32 upper);

bool NearZero(const f32 v);

f32 InvSqrt(f32 x);

// djb2 string hashing
unsigned long hash_djb2(unsigned char *str);

#endif