#ifndef __UTIL_H__ // common utility functions (with no need to be in a class)
#define __UTIL_H__

#include "ctypes.h"

const f32 EPSILON = 0.000001f; // rough epsilon
const f32 PI = 3.1415926f; // rough PI
const f32 PIOVER180 = PI/180.0f;

void set_bit(u32 &opt, const u32 bit); // set bit in integer opt
i32 bit_set(const u32 &opt, const u32 bit); // returns whether or not a bit is set
void clear_bit(u32 &opt, const u32 bit); // clears bit
void toggle_bit(u32 &opt, const u32 bit); // toggles bit

void clamp(u32 &v, u32 lower, u32 upper); // clamp unsigned integer
void clamp(i32 &v, i32 lower, i32 upper); // clamp signed integer

bool NearZero(const f32 v); // returns true if v near zero i.e. abs(v) <= EPSILON

f32 InvSqrt(f32 x); // carmacks inverse sqrt

f32 reciprocal_sqrt(f32 f); // returns reciprocal square root

// djb2 string hashing
unsigned long hash_djb2(const uc8 *str);

f32 DEGTORAD(f32 degs); // degrees to radians
f32 RADTODEG(f32 rads); // radians to degrees

f32 fract(f32 c); // returns fractional part of c

f32 lerp(f32 min, f32 max, f32 t); // lerp between min and max using 0 < t < 1

const bool file_exists(const c8 *filename); // returns true if file exists

f32 randflt(f32 min, f32 max); // returns random float between min and max

#endif