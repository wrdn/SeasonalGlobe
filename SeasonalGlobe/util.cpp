#include "util.h"
#include <math.h>

void SAFE_DELETE(const void *p) { if(p) { delete p; p = 0; } };
void SAFE_DELETE_ARRAY(const void *p) { if(p) { delete [] p; p = 0; } };

void set_bit(i32 &opt, const u32 bit)
{
	opt |= bit;
};

i32 bit_set(const i32 &opt, const u32 bit)
{
	return (opt & bit);
};

void clear_bit(i32 &opt, const u32 bit)
{
	opt &= (~bit);
};

void toggle_bit(i32 &opt, const u32 bit)
{
	opt ^= bit;
};

void clamp(u32 &v, u32 lower, u32 upper)
{
	if(v < lower) { v = lower; }
	else if(v > upper) { v = upper; }
};

void clamp(i32 &v, i32 lower, i32 upper)
{
	if(v < lower) { v = lower; }
	else if(v > upper) { v = upper; }
};

bool NearZero(const f32 v)
{
		return fabs(v) < EPSILON;
};

// Fast inverse square root, from http://pizer.wordpress.com/2008/10/12/fast-inverse-square-root/
f32 InvSqrt(f32 x)
{
   u32 i = 0x5F1F1412 - (*(u32*)&x >> 1);
   float tmp = *(float*)&i;
   return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
};