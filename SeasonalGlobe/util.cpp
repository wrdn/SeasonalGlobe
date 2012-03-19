#include "util.h"
#include <math.h>
#include <sys/stat.h>
#include <pmmintrin.h>
#include <stdio.h>
#include <stdlib.h>

// sets bits in opt based on bit
void set_bit(u32 &opt, const u32 bit)
{
	opt |= bit;
};

// returns true (not 0) if "bit" set in "opt"
i32 bit_set(const u32 &opt, const u32 bit)
{
	return (opt & bit);
};

// clears "bit" in "opt"
void clear_bit(u32 &opt, const u32 bit)
{
	opt &= (~bit);
};

// toggles "bit" in "opt"
void toggle_bit(u32 &opt, const u32 bit)
{
	opt ^= bit;
};

// clamp unsigned int "v" to "lower" and "upper" bounds
void clamp(u32 &v, u32 lower, u32 upper)
{
	if(v < lower) { v = lower; }
	else if(v > upper) { v = upper; }
};

// clamp signed int "v" to "lower" and "upper" bounds
void clamp(i32 &v, i32 lower, i32 upper)
{
	if(v < lower) { v = lower; }
	else if(v > upper) { v = upper; }
};

// retursn true if float value near zero (i.e. abs(v)<EPSILON
bool NearZero(const f32 v)
{
		return fabs(v) < EPSILON;
};

// Fast inverse square root, from http://pizer.wordpress.com/2008/10/12/fast-inverse-square-root/
f32 InvSqrt(f32 x)
{
   u32 i = 0x5F1F1412 - (*(u32*)&x >> 1);
   f32 tmp = *(f32*)&i;
   return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
};

// Source: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash_djb2(const uc8 *str)
{
	unsigned long hash = 5381;
	u32 c='a'; // give c a valid value so we can get into the loop

	while(c) // rewrote this slightly to get rid of Parasoft and Visual Studio Release mode warning (assignment in conditional expression)
	{
		c = *str;
		if(c==0) { break; }
		hash = ((hash << 5) + hash) + c;
		++str;
	}

	return hash;
}

// degrees to radians
f32 DEGTORAD(f32 degs)
{
	return (degs * (PI / 180.0f));
};

// radians to degrees
f32 RADTODEG(f32 rads)
{
	return (rads * (180.0f / PI));
};

// returns fractional part of c
f32 fract(f32 c)
{
	return (c - floor(c));
};

// lerp from min to max using 0 < t < 1
f32 lerp(f32 min, f32 max, f32 t)
{
	return (min + (t * (max-min)));
};

// returns true if file exists
const bool file_exists(const c8 *filename)
{
	struct stat f; // careful not to mix up the structure and function
	return (stat(filename,&f) == 0); // 0 if file exists
}

// random float between min and max
f32 randflt(f32 min, f32 max)
{
    f32 random = ((f32) rand()) / (f32) RAND_MAX;

    f32 range = max - min;  
    return (random*range) + min;
};

// sse reciprocal sqrt (fast)
f32 reciprocal_sqrt(f32 f)
{
	/*
	// SSE Version (Fast):
	f32 result; // reciprocal square root using SSE (accurate to ~11/12 mantissa bits)
	_mm_store_ss(&result, _mm_rsqrt_ss( _mm_load_ss(&f) ));
	return result;

	// More accurate scalar version:
	return 1.0f / sqrt(f);
	*/
	f32 result; // reciprocal square root using SSE (accurate to ~11/12 mantissa bits)
	_mm_store_ss(&result, _mm_rsqrt_ss( _mm_load_ss(&f) ));
	return result;
};