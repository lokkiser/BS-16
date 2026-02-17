#include "str_util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
//#include <stdarg.h>


#define STRING_UTIL_BUF_CNT     10
#define STRING_UTIL_BUF_SIZE    34
static char string_util_buf[STRING_UTIL_BUF_CNT][STRING_UTIL_BUF_SIZE];
static unsigned char string_util_buf_index = 0;

char * string_util_next_buf(void)
{
   ++string_util_buf_index;
   if (string_util_buf_index >= STRING_UTIL_BUF_CNT) string_util_buf_index = 0;
   return string_util_buf[string_util_buf_index];
}

void memncpy_s(void * dst, size_t dst_size, const void * src, size_t src_count)
{
  if (!dst || !src || !dst_size || !src_count) return;
  memcpy(dst, src, MIN(dst_size, src_count));
}

void strncpy_s(char * dst, size_t dst_size, const char * src, size_t src_count)
{
  if (!dst || !src || !dst_size) return;
  if (!src_count) {
	  dst[0] = '\0';
	  return;
  }
  memncpy_s(dst, dst_size - 1, src, src_count);
  dst[MIN(dst_size - 1, src_count)] = '\0';
}

void strcpy_s(char * dst, size_t dst_size, const char * src)
{
  if (!src) return;

  strncpy_s(dst, dst_size, src, strlen(src));
}

void strncat_s(char * dst, size_t dst_size, const char * src, size_t src_count)
{
  size_t i;

  if (!dst || !src || !dst_size || !src_count) return;

  for (i = 0; i < dst_size; ++i) {
    if (dst[i] == '\0') break;
  }
  if (i == dst_size) return;

  strncpy_s(dst + i, dst_size - i, src, src_count);
}

void strcat_s(char * dst, size_t dst_size, const char * src)
{
  if (!dst || !src || !dst_size) return;
  strncat_s(dst, dst_size, src, strlen(src));
}

void strrncat_s(char * dst, size_t dst_size, const char * src, size_t src_count)
{
  size_t dl;

  if (!dst || !src || !dst_size || !src_count || (src_count >= dst_size) ) return;

  for (dl = 0; dl < dst_size; ++dl) {
    if (dst[dl] == '\0') break;
  }
  if (dl == dst_size) return;

  strncpy_s(dst + src_count, dst_size - src_count, dst, dl);
  memncpy_s(dst, dst_size, src, src_count);
}


void strrcat_s(char * dst, size_t dst_size, const char * src)
{
  if (!dst || !src || !dst_size) return;
  strrncat_s(dst, dst_size, src, strlen(src));
}

void * memfind(void * mem, size_t mem_size, const void * what, size_t what_count)
{
  size_t i, sz;
  unsigned char * pmem, * pwhat;

  if ( !mem || !what || !mem_size || !what_count || (what_count > mem_size)) return NULL;

  sz = mem_size - what_count;
  pmem = (unsigned char *)mem;
  pwhat = (unsigned char *)what;

  for (i = 0; i <= sz; ++i) {
	  if (pmem[i] == pwhat[0]) {
		  if (memcmp(pmem + i, pwhat, what_count) == 0) return pmem + i;
	  }
  }
  return NULL;
}


char * strnstr_s(char * s1, size_t s1_size, const char * s2, size_t s2_count)
{
   return (char *)memfind(s1, s1_size, s2, s2_count);
}

char * strstr_s(char * s1, size_t s1_size, const char * s2)
{
   return (char *)memfind(s1, s1_size, s2, strlen(s2));
}

char * strchr_s(char * s, size_t s_size, const char c)
{
   return (char *)memfind(s, s_size, &c, 1);
}

char * strnright(char * str, size_t str_size, const char * substr, size_t substr_count)
{
  char * ptr;

  ptr = strnstr_s(str, str_size, substr, substr_count);
  if (!ptr) return NULL;
  return ptr + substr_count;
}

char * strright(char * str, size_t str_size, const char * substr)
{
  if (!substr) return NULL;

  return strnright(str, str_size, substr, strlen(substr));
}


char * strntrim(char * str, size_t str_count)
{
  size_t i;
  char * res = NULL;
  unsigned char * uch =  (unsigned char *)str;

  if ((str == NULL) || (str_count == 0)) return NULL;


  for (i = 0; i < str_count; ++i) {
    if ( uch[i] > 32) {
      res = str + i;
      break;
    }
  }

  if (res == NULL) return NULL;

  for (i = str_count; i > 0; --i) {
    if ( uch[i - 1] > 32) {
      if (i != str_count) str[i] = '\0';
      break;
    }
  }

  return res;
}

char * strtrim(char * str)
{
  return strntrim(str, strlen(str));
}

bool memcmp_s(void * mem1, size_t mem1_size, const void * mem2, size_t mem2_size)
{
  if (!mem1 || !mem2 || !mem1_size || !mem2_size) return FALSE;
  if (mem2_size > mem1_size)  return FALSE;
  return (memcmp(mem1, mem2, mem2_size) == 0);
}

bool strncmp_s(char * s1, size_t s1_size, const char * s2, size_t s2_count)
{
  return memcmp_s(s1, s1_size, s2, s2_count);
}

bool strcmp_s(char * s1, size_t s1_size, const char * s2)
{
  if (!s2) return FALSE;

  return strncmp_s(s1, s1_size, s2, strlen(s2));
}

bool strequals(char * s1, const char * s2)
{
	size_t len;

	if (!s1 || !s2) return FALSE;

	len = strlen(s1);
	if (len != strlen(s2)) return FALSE;
	return (memcmp(s1, s2, len) == 0);
}

bool strcaseequals(char * s1, char const *s2)
{
	size_t len, i;

	if (!s1 || !s2) return FALSE;

	len = strlen(s1);
	if (len != strlen(s2)) return FALSE;

	for (i = 0; i < len; ++i) {
		if (tolower(s1[i]) != tolower(s2[i])) return FALSE;
	}
	return TRUE;
}

bool str2lower(char * dest, size_t dest_size, const char * src)
{
	size_t len, i, cnt;

	if (!dest || !src) return FALSE;

	len = strlen(src);

	cnt = MIN(len,  dest_size - 1);

	for (i = 0; i < cnt; ++i) {
		dest[i] = tolower(src[i]);
	}
	dest[cnt] = '\0';
	return TRUE;
}

bool str2upper(char * dest, size_t dest_size, const char * src)
{
	size_t len, i, cnt;

	if (!dest || !src) return FALSE;

	len = strlen(src);

	cnt = MIN(len,  dest_size - 1);

	for (i = 0; i < cnt; ++i) {
		dest[i] = toupper(src[i]);
	}
	dest[cnt] = '\0';
	return TRUE;
}


char * strgate(char * str, size_t str_size, const char * begin, const char * end, size_t *len)
{
  char * ptrb, * ptre;

  if (!str || !str_size || !begin || !end) return NULL;

  if ( (ptrb = strstr_s(str, str_size, begin)) != NULL ) {
    ptrb += strlen(begin);
    if ( (ptre = strstr_s(ptrb, str_size - (ptrb - str), end)) != NULL ) {
      *len = ptre - ptrb;
      return ptrb;
    }
  }
  return NULL;
}

void str_replace_s(char * in, char * out, uint16_t out_size, const char * replace_old, const char * replace_new)
{
   char * ptr, * ptre, * in_end_ptr;

   in_end_ptr = in + strlen(in);
   out[0] = '\0';
   ptr = in;

   if (!in || !out || !out_size || !replace_old || !replace_new) return;

   while ((ptre = strstr_s(ptr, in_end_ptr - ptr, replace_old)) != NULL) {
       strncat_s(out, out_size, ptr, ptre - ptr);
       if (strlen(replace_new)) strcat_s(out, out_size, replace_new);
       ptr = ptre + strlen(replace_old);
   }
   if (strlen(ptr)) strcat_s(out, out_size, ptr);
}

void memrev(void * mem, size_t mem_size)
{
  size_t i, j, sz = mem_size / 2;
  unsigned char val, * pmem = (unsigned char *)mem;

  for (i = 0; i < sz; ++i) {
    j = mem_size - i - 1;
    val = pmem[i];
    pmem[i] = pmem[j];
    pmem[j] = val;
  }
}

void ull_to_str(unsigned long long value, char * buf, int buf_size)
{
   char * ptr;

   if (!buf || !buf_size) return;


   ptr = buf;
   do
   {
      if (ptr - buf >= buf_size) break;
      *ptr = value % 10 + '0';
      value /= 10;
      ptr++;
   }
   while (value != 0);

   memrev(buf, ptr - buf);

   buf[ptr - buf] = '\0';
}

void ll_to_str(long long value, char * buf, int buf_size)
{
   if (value < 0) {
     if (buf_size < 1) return;
     buf[0] = '-';
     ull_to_str(-value, buf + 1, buf_size - 1);
   }
   else ull_to_str(value, buf, buf_size);
}


char * ull2str(unsigned long long value)
{
  char * buf;

  buf = string_util_next_buf();
  ull_to_str(value, buf, STRING_UTIL_BUF_SIZE);

  return buf;
}

char * ll2str(long long value)
{

  char * buf;

  buf = string_util_next_buf();
  ll_to_str(value, buf, STRING_UTIL_BUF_SIZE);
  return buf;
}


char * ull2strfmt(unsigned long long value, const char * fmt)
{
  char * buf;

  buf = string_util_next_buf();
  snprintf(buf, STRING_UTIL_BUF_SIZE, fmt, value);
  return buf;
}


unsigned long long str2ull(char * str)
{
  //return strtoul(str, NULL, 10);
  //return strtoull(str, NULL, 10);

	unsigned long long res;
	char * ptr;
	bool digit_found;

	if (!str) return 0;

	digit_found = FALSE;
	ptr = str;
	res = 0;

	while (*ptr) {
		if ((*ptr < '0' || *ptr > '9')) {
			if (digit_found) break;
		} else {
			digit_found = TRUE;
			res *= 10;
			res += (*ptr - '0');
		}
		ptr++;
	}
	return res;
}

unsigned long str2ul(char * str)
{
	return str2ull(str);
}


char * ul2str(unsigned long val)
{
   char * buf;

   buf = string_util_next_buf();

   ull_to_str(val, buf, STRING_UTIL_BUF_SIZE);

   return buf;
}

long long str2ll(char * str)
{
	char * ptr;

	if (!str) return 0;

	ptr = str;

	while (*ptr) {
		if (*ptr == '-') {
			return -str2ull(ptr + 1);
		}
		else if ((*ptr >= '0') && (*ptr <= '9')) {
			return str2ull(ptr);
		}
		ptr++;
	}
	return 0;
}

bool strnisdigit(char * str, unsigned char str_count)
{
  unsigned char i;

  for (i = 0; i < str_count; ++i) {
    if ( (str[i] < '0') || (str[i] > '9') ) return FALSE;
  }
  return TRUE;
}

bool strisdigit(char * str)
{
  if (!str) return FALSE;

  return strnisdigit(str, strlen(str));
}


void float_to_str(float val, unsigned char cnt, char * buf, int buf_size)
{
   char * ptr;
   unsigned char i;

   ptr = buf;

   if (val < 0.0) {
       ptr[0] = '-';
       ptr++;
       val = -val;
   }

   ull_to_str(val, ptr,  buf_size - (ptr - buf));
   if (!cnt) return;

   ptr = buf + strlen(buf);
   *ptr = '.';
   ptr++;
   *ptr = '\0';
   if (ptr - buf >= buf_size) return;

   val -= (int)val;

   for (i = 0; i < cnt; i++) {
      if (ptr - buf >= buf_size) return;
      val = val * 10;
      *ptr = (int)val + '0';
      val -= (int)val;
      ptr++;
   }
   *ptr = '\0';
}

char * float2str(float val)
{
  char * buf;

  buf = string_util_next_buf();
  float_to_str(val, 1, buf, STRING_UTIL_BUF_SIZE);

  return buf;
}

char * float2str2(float val)
{
  char * buf;

  buf = string_util_next_buf();
  float_to_str(val, 2, buf, STRING_UTIL_BUF_SIZE);

  return buf;
}

float str2float (char * str)
{
  unsigned char abc;

  float ret = 0, fac = 1;
  for (abc = 9; abc & 1; str++) {
    abc  =  *str == '-' ?
              (abc & 6 ? abc & 14 : (abc & 47) | 36)
            : *str == '+' ?
              (abc & 6 ? abc & 14 : (abc & 15) | 4)
            : *str > 47 && *str < 58 ?
              abc | 18
            : (abc & 8) && *str == '.' ?
              (abc & 39) | 2
            : !(abc & 2) && (*str == ' ' || *str == '\t') ?
              (abc & 47) | 1
            :
              abc & 46;
    if (abc & 16) {
      ret = abc & 8 ? *str - 48 + ret * 10 : (*str - 48) / (fac *= 10) + ret;
    }
  }
  return abc & 32 ? -ret : ret;
}

void mem2hexstr(void *mem, size_t mem_len, char * str, size_t str_count, const char * delim)
{
  char tmp[3];
  size_t i, sz = MIN(mem_len, (str_count / 2) - 1);
  unsigned char * umem = (unsigned char *)mem;

  str[0] = '\0';
  for (i = 0; i < sz; ++i) {
    sprintf(tmp, "%02X", umem[i]);
    if (delim && (str[0] != '\0')) strcat_s(str, str_count, delim);
    strcat_s(str, str_count, tmp);
  }
}


void usmem2hexstr(unsigned short * data, size_t data_len, char * str, size_t str_count, const char * delim)
{
  char tmp[5];
  size_t i;

  str[0] = '\0';
  for (i = 0; i < data_len; ++i) {
    sprintf(tmp, "%04X", data[i]);
    if (delim && (str[0] != '\0')) strcat_s(str, str_count, delim);
    strcat_s(str, str_count, tmp);
  }
}


size_t hexstr2mem(char * str, size_t str_count, const char * delim, void * mem, size_t mem_size)
{
  char buf[3];
  size_t i, j, delim_len;
  unsigned char * umem = (unsigned char *)mem;

  buf[2] = '\0';
  delim_len = strlen(delim);

  i = 0;
  j = 0;
  while (i < str_count) {

    memcpy(buf, &str[i], 2);
    umem[j] = strtoul(buf, NULL, 16);
    j++;
    if (j >= mem_size) break;

    i += 2;
    if (delim && (i + delim_len < str_count) && (memcmp(&str[i], delim, delim_len) == 0)) {
        i += delim_len;
    }

  }

  return j;
}

char * ull2hexstr(unsigned long long val)
{
  static char __ull2tohex_str[17];
  int i;
  unsigned char * uptr = (unsigned char *)&val;
  char tmp[3];

  __ull2tohex_str[0] = '\0';
  for (i = sizeof(val) - 1; i > -1 ; --i) {
    if (uptr[i] == 0) continue;
    if (uptr[i] > 0x0F) sprintf(tmp, "%02X", uptr[i]);
    else sprintf(tmp, "%01X", uptr[i]);
    strcat(__ull2tohex_str, tmp);
  }

  return __ull2tohex_str;
}

unsigned long long hexstr2ull(char * hexstr)
{
  char buf[3], * ptr;
  unsigned long long res = 0;
  unsigned char *pres = (unsigned char *)&res;
  int i, slen = strlen(hexstr);
  char * ptr_end = hexstr + slen;


  ptr = hexstr;
  i = (slen / 2) - 1;
  if (2 * (slen / 2) != slen) {
    buf[0] = ptr[0];
    buf[1] = '\0';
    pres[i + 1] = strtoul(buf, NULL, 16);
    ptr++;
  }

  buf[2] = '\0';
  while ( (i > -1) && (ptr < ptr_end) ) {
     memcpy(buf, ptr, 2);
     pres[i] = strtoul(buf, NULL, 16);
     ptr += 2;
     i--;
  }

  return res;
}

char * basename(char * path)
{
  int len;
  
  if (path == NULL) return NULL;

  len = strlen(path);
  if (len == 0) return NULL;
  while ((--len) >= 0) {
    if (path[len] == '/') {
      if (strlen(path + len + 1) == 0) return NULL;
      return path + len + 1;
    }
  }
  return path;
}

char * int2str(int val)
{
   char * buf;

   buf = string_util_next_buf();

   if (val < 0) {
     buf[0] = '-';
     ull_to_str(-val, buf + 1, STRING_UTIL_BUF_SIZE - 1);
   }
   else ull_to_str(val, buf, STRING_UTIL_BUF_SIZE);

   return buf;
}

int str2int(char * str)
{
	//if (str) return atoi(str);
	if (str) return str2ll(str);
	else return 0;
}

unsigned long hexstr2ul(char * str)
{
	return strtoul(str, NULL, 16 );
}

char * ul2hexstr(unsigned long val)
{
	unsigned long x;
	int i;
	unsigned char digit;
	char ch;
	static char __ull2hexstr[9];

	__ull2hexstr[8] = '\0';
	x = val;
	i = 7;
	while (i >= 0)
	{
		digit = (x & 0x0F);
		if (digit > 9) ch = 'A' + digit - 10;
		else ch = '0' + digit;
		__ull2hexstr[i] = ch;
		x >>= 4;
		i -= 1;
	}

	return __ull2hexstr;
}


char * us2hexstr(unsigned short val)
{
	unsigned short x;
	int i;
	unsigned char digit;
	char ch;
	static char __us2hexstr[5];

	__us2hexstr[4] = '\0';
	x = val;
	i = 3;
	while (i >= 0)
	{
		digit = (x & 0x0F);
		if (digit > 9) ch = 'A' + digit - 10;
		else ch = '0' + digit;
		__us2hexstr[i] = ch;
		x >>= 4;
		i -= 1;
	}

	return __us2hexstr;
}



char * uc2hexstr(unsigned char val)
{
	static char __uc2hexstr[3];

	sprintf(__uc2hexstr, "%02X", val);
	return __uc2hexstr;
}

char * bool2str(bool val)
{
	return val ? "true" : "false";
}

bool str2bool(char * str)
{
	if (strcmp_s(str, strlen(str), "true")) return TRUE;
	if (strcmp_s(str, strlen(str), "false")) return FALSE;

	return (str2int(str) ? TRUE : FALSE);
}

void uch_to_bitstr(unsigned char val, char * buf, size_t buf_size)
{
   unsigned char c;

   buf[0] = '\0';
   c = 0x80;
   while (c) {
        strcat_s(buf, buf_size, val & c ? "1" : "0");
        c = c >> 1;
   }
}

char * uch2bitstr(unsigned char val)
{
	char * buf;

	buf = string_util_next_buf();
	uch_to_bitstr(val, buf, STRING_UTIL_BUF_SIZE);
	return buf;
}

char * us2bitstr(unsigned short val)
{
	char * buf;

	buf = string_util_next_buf();
	uch_to_bitstr(val >> 8, buf, STRING_UTIL_BUF_SIZE);
	uch_to_bitstr(val & 0xFF, buf + strlen(buf), STRING_UTIL_BUF_SIZE - strlen(buf));
	return buf;
}

char * ul2bitstr(unsigned long val)
{
	char * buf;

	buf = string_util_next_buf();
	uch_to_bitstr(val >> 24, buf, STRING_UTIL_BUF_SIZE);
	uch_to_bitstr((val >> 16) & 0xFF, buf + strlen(buf), STRING_UTIL_BUF_SIZE - strlen(buf));
	uch_to_bitstr((val >> 8) & 0xFF, buf + strlen(buf), STRING_UTIL_BUF_SIZE - strlen(buf));
	uch_to_bitstr(val & 0xFF, buf + strlen(buf), STRING_UTIL_BUF_SIZE - strlen(buf));
	return buf;
}

char * ull2bitstr(unsigned long long val)
{
	static char __ull2bitstr_buf[65];
	char * ptr;
	unsigned char * bytes;
	int i;

	bytes = (unsigned char *)&val;
	ptr = __ull2bitstr_buf;

	for (i = 7; i > -1; i--) {
		uch_to_bitstr(bytes[i], ptr, sizeof(__ull2bitstr_buf) - (ptr - __ull2bitstr_buf));
		ptr += 8;
	}
	return __ull2bitstr_buf;
}


void arr_to_str(void * arr, size_t arr_count, var_type_t arr_type, char * str, size_t str_size, bool hex)
{
	size_t i;
	bool * pbool = arr;
	unsigned char * puint8 = arr;
	char * pint8 = arr;
	unsigned short * puint16 = arr;
	short * pint16 = arr;
	unsigned long * puint32 = arr;
	long * pint32 = arr;
	unsigned long long * puint64 = arr;
	long long * pint64 = arr;
	float * pfloat = arr;

	str[0] = '\0';

	for (i = 0; i < arr_count; ++i) {

		if (str[0] != '\0') strcat_s(str, str_size, ",");

		if (hex) {
			switch (arr_type) {
				case VAR_TYPE_BOOL: strcat_s(str, str_size, bool2str(pbool[i])); break;
				case VAR_TYPE_UINT8: strcat_s(str, str_size, uc2hexstr(puint8[i])); break;
				case VAR_TYPE_INT8: strcat_s(str, str_size, uc2hexstr(pint8[i])); break;
				case VAR_TYPE_UINT16: strcat_s(str, str_size, us2hexstr(puint16[i])); break;
				case VAR_TYPE_INT16: strcat_s(str, str_size, us2hexstr(pint16[i])); break;
				case VAR_TYPE_UINT32: strcat_s(str, str_size, ul2hexstr(puint32[i])); break;
				case VAR_TYPE_INT32: strcat_s(str, str_size, ul2hexstr(pint32[i])); break;
				case VAR_TYPE_UINT64: strcat_s(str, str_size, ull2hexstr(puint64[i])); break;
				case VAR_TYPE_INT64: strcat_s(str, str_size, ull2hexstr(pint64[i])); break;
				default: break;
			}

		} else {
			switch (arr_type) {
				case VAR_TYPE_BOOL: strcat_s(str, str_size, bool2str(pbool[i])); break;
				case VAR_TYPE_UINT8: strcat_s(str, str_size, int2str(puint8[i])); break;
				case VAR_TYPE_INT8: strcat_s(str, str_size, int2str(pint8[i])); break;
				case VAR_TYPE_UINT16: strcat_s(str, str_size, int2str(puint16[i])); break;
				case VAR_TYPE_INT16: strcat_s(str, str_size, int2str(pint16[i])); break;
				case VAR_TYPE_UINT32: strcat_s(str, str_size, int2str(puint32[i])); break;
				case VAR_TYPE_INT32: strcat_s(str, str_size, ull2str(pint32[i])); break;
				case VAR_TYPE_UINT64: strcat_s(str, str_size, ull2str(puint64[i])); break;
				case VAR_TYPE_INT64: strcat_s(str, str_size, ll2str(pint64[i])); break;
				case VAR_TYPE_FLOAT: strcat_s(str, str_size, float2str(pfloat[i])); break;
				default: break;
			}
		}
	}
}

size_t str_to_arr(const char * str, void * arr, var_type_t arr_type)
{

	char * ptr;
	size_t i;
	bool * pbool = arr;
	unsigned char * puint8 = arr;
	char * pint8 = arr;
	unsigned short * puint16 = arr;
	short * pint16 = arr;
	unsigned long * puint32 = arr;
	long * pint32 = arr;
	unsigned long long * puint64 = arr;
	long long * pint64 = arr;

	ptr = (char *)str;
	i = 0;

	while (ptr) {

		switch (arr_type) {
			case VAR_TYPE_BOOL: pbool[i] = str2bool(ptr); break;
			case VAR_TYPE_UINT8: puint8[i] = str2int(ptr); break;
			case VAR_TYPE_INT8:  pint8[i] = str2int(ptr);; break;
			case VAR_TYPE_UINT16: puint16[i] = str2int(ptr); break;
			case VAR_TYPE_INT16: pint16[i] = str2int(ptr); break;
			case VAR_TYPE_UINT32: puint32[i] = str2ull(ptr); break;
			case VAR_TYPE_INT32: pint32[i] = str2ll(ptr); break;
			case VAR_TYPE_UINT64: puint64[i] = str2ull(ptr); break;
			case VAR_TYPE_INT64: pint64[i] = str2ll(ptr); break;
			default: break;
		}

		i++;
		ptr = strright(ptr, strlen(ptr), ",");
	}
	return i;
}


