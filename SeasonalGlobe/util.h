#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <vector>
using namespace std;

#include "ctypes.h"

#define SAFE_DELETE(m) if(m) { delete m; m = 0; }
#define SAFE_DELETE_ARRAY(m) if(m) { delete [] m; m = 0; }

// pass source and a null char pointer (memory allocated in func)
// The _unsafe versions of the copystr() functions do no checks (i.e. dont check src != NULL and dont check strlen(src)>0, or
// the passed len variable > 0). The _unsafe functions should only ever be used when you know src!=NULL and the string has length>0
c8* copystr(const c8 *src);
c8* copystr(const c8 *src, const i32 len);
c8* copystr_unsafe(const c8 *src);
c8* copystr_unsafe(const c8 *src, const i32 len);

c8* substr(const c8*src, u32 start, u32 end);

bool compare_str_lim(const c8 *src, const c8 *targ, u32 start, u32 end);

bool compare_str_lim(const c8 *src, const c8 *targ, u32 src_start, u32 src_end,
	u32 targ_start, u32 targ_end);

void split(const string &s, c8 delim, vector<string> &v);
void printf_array(f32 *arr, u32 sz);
void print_array(f32 *arr, c8* pftype, u32 sz);
void printi_array(u32 *arr, u32 sz);

const bool file_exists(const c8 *filename);
c8 *read_src(const c8 *file); // adds newline characters back when reading
c8 *read_src_raw(const c8 *file); // doesn't add newline characters back when reading
c8 *read_src_fast(const c8 *file); // gets file size then reads entire file into memory in single read
std::vector<c8*> read_src_to_vec(const c8* file); // 1 line per vector element (empty if no file)
std::vector<c8*> read_src_to_vec(const c8* file, bool incBlankLines); // 1 line per vector element (empty if no file)

// Useful as sometimes you have a rough idea how many lines the file will have. For example, for WaveFront OBJ files, a reasonable heuristic is:
// Number Of Lines (originalVecSize) = File Size in Bytes / 30 (around 30 bytes per line, assuming ASCII). Providing the original vector size, you may
// gain some efficiency, as less resizes are required
std::vector<c8*> read_src_to_vec(const c8* file, bool incBlankLines, const u32 originalVecSize);

// Note: vector cleanup functions should be added on a case by case basis, rather than using templates, which may slow
// down the compilation process significantly, aswell as making the code more confusing in some cases (see: STL library)
// Removes all the strings in the vector then clears it.
void cleanup_str_vec(std::vector<c8*> &v);

void set_bit(i32 &opt, u32 bit);
i32 bit_set(i32 &opt, u32 bit);
void clear_bit(i32 &opt, u32 bit);
void toggle_bit(i32 &opt, u32 bit);

// djb2 string hashing
u32 hash(u32 *str);

const char* bstr(const bool b);

bool fast_strcmp(char *a, char *b);
bool fast_strcmp(char *a, char *b, u32 len);

void clamp(u32 &v, u32 lower, u32 upper);
void clamp(i32 &v, i32 lower, i32 upper);

#endif