#ifndef __UTIL_H__
#define __UTIL_H__

#include "ctypes.h"

#define EPSILON 0.000001f // rough epsilon
#define PI 3.1415926f

void SAFE_DELETE(const void *p);
void SAFE_DELETE_ARRAY(const void *p);

void set_bit(u32 &opt, const u32 bit);
i32 bit_set(const u32 &opt, const u32 bit);
void clear_bit(u32 &opt, const u32 bit);
void toggle_bit(u32 &opt, const u32 bit);

void clamp(u32 &v, u32 lower, u32 upper);
void clamp(i32 &v, i32 lower, i32 upper);

bool NearZero(const f32 v);

f32 InvSqrt(f32 x);

// djb2 string hashing
unsigned long hash_djb2(uc8 *str);

#endif