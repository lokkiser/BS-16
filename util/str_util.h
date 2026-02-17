#ifndef STR_UTIL_H
#define STR_UTIL_H

#include "def_util.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


//safe copy
void memncpy_s(void * dst, size_t dst_size, const void * src, size_t src_count);
//safe copy of the line with 0 at the end of the line
void strncpy_s(char * dst, size_t dst_size, const char * src, size_t src_count);
void strcpy_s(char * dst, size_t dst_size, const char * src);
//safe catenation with 0 at the end of the line
void strncat_s(char * dst, size_t dst_size, const char * src, size_t src_count);
void strcat_s(char * dst, size_t dst_size, const char * src);
//safe reverse catenation with 0 at the end of the line
void strrncat_s(char * dst, size_t dst_size, const char * src, size_t src_count);
void strrcat_s(char * dst, size_t dst_size, const char * src);
//search in mem memory for the fragment what
void * memfind(void * mem, size_t mem_size, const void * what, size_t what_count);
//search in string s1 for substring s2
char * strnstr_s(char * s1, size_t s1_size, const char * s2, size_t s2_count);
char * strstr_s(char * s1, size_t s1_size, const char * s2);
//search string s for character c
char * strchr_s(char * s, size_t s_size, const char c);
//shift right behind line substr
char * strnright(char * str, size_t str_size, const char * substr, size_t substr_count);
char * strright(char * str, size_t str_size, const char * substr);
//trim
char * strntrim(char * str, size_t str_count);
char * strtrim(char * str);
// compare two memory areas
bool memcmp_s(void * mem1, size_t mem1_size, const void * mem2, size_t mem2_size);
//compare two lines
bool strncmp_s(char * s1, size_t s1_size, const char * s2, size_t s2_count);
bool strcmp_s(char * s1, size_t s1_size, const char * s2);
bool strequals(char * s1, const char * s2);
bool strcaseequals(char * s1, char const *s2);
bool str2lower(char * dest, size_t dest_size, const char * src);
bool str2upper(char * dest, size_t dest_size, const char * src);
//the line at the gate from begin to end
char * strgate(char * str, size_t str_size, const char * begin, const char * end, size_t *len);
//replace old to new
void str_replace_s(char * in, char * out, uint16_t out_size, const char * replace_old, const char * replace_new);
//reverse memory
void memrev(void * mem, size_t mem_size);
//filename from path
char * basename(char * path);
//64 bit unsigned value to string
char * ull2str(unsigned long long value);
char * ull2strfmt(unsigned long long value, const char * fmt);
unsigned long long str2ull(char * str);
unsigned long str2ul(char * str);
char * ul2str(unsigned long val);
void ull_to_str(unsigned long long value, char * buf, int buf_size);
//64 bit signed value to string
char * ll2str(long long value);
long long str2ll(char * );
void ll_to_str(long long value, char * buf, int buf_size);
//is the string an int digit?
bool strnisdigit(char * str, unsigned char str_count);
bool strisdigit(char * str);
//float to string
char * float2str(float val);
char * float2str2(float val);
float str2float(char * str);
void float_to_str(float val, unsigned char cnt, char * buf, int buf_size);
//memory to dump
void mem2hexstr(void *mem, size_t mem_len, char * str, size_t str_count, const char * delim);
size_t hexstr2mem(char * str, size_t str_count, const char * delim, void * mem, size_t mem_size);

void usmem2hexstr(unsigned short * data, size_t data_len, char * str, size_t str_count, const char * delim);

// 64 bit value to dump
char * ull2hexstr(unsigned long long val);
unsigned long long hexstr2ull(char * hexstr);
//int in str
char * int2str(int val);
int str2int(char * str);
//bool in str
char * bool2str(bool val);
bool str2bool(char * str);

unsigned long hexstr2ul(char * str);
char * ul2hexstr(unsigned long val);

char * us2hexstr(unsigned short val);
char * uc2hexstr(unsigned char val);

void uch_to_bitstr(unsigned char val, char * buf, size_t buf_size);
char * uch2bitstr(unsigned char  val);
char * us2bitstr(unsigned short val);
char * ul2bitstr(unsigned long val);
char * ull2bitstr(unsigned long long val);


typedef enum {
	VAR_TYPE_BOOL = 0,
	VAR_TYPE_UINT8,
	VAR_TYPE_INT8,
	VAR_TYPE_UINT16,
	VAR_TYPE_INT16,
	VAR_TYPE_UINT32,
	VAR_TYPE_INT32,
	VAR_TYPE_UINT64,
	VAR_TYPE_INT64,
	VAR_TYPE_FLOAT
} var_type_t;


void arr_to_str(void * arr, size_t arr_count, var_type_t arr_type, char * str, size_t str_size, bool hex);
size_t str_to_arr(const char * str, void * arr, var_type_t arr_type);



#define VSPRINTF_S(buf, buf_size, fmt)  { va_list arg; va_start(arg, fmt); vsnprintf(buf, buf_size, fmt, arg); va_end(arg); buf[buf_size - 1] = '\0'; }


#ifdef __cplusplus
}
#endif

#endif /* STRING_UTIL_H */
