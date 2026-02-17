#ifndef BYTEBUFF_UTIL_H
#define BYTEBUFF_UTIL_H

#include "def_util.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct _bytebuf 
{
  unsigned char* buf;
  size_t buf_len;
  size_t windex, rindex;
  size_t available;
} bytebuf_t;


#define BYTEBUF_STATIC_INIT(buf, buf_size)			{buf, buf_size, 0, 0, 0}

void bytebuf_init(bytebuf_t* bytebuf, unsigned char* buf, size_t buf_len);
void bytebuf_clear(bytebuf_t* bytebuf);

bool bytebuf_put_byte(bytebuf_t* bytebuf, unsigned char val);
bool bytebuf_get_byte(bytebuf_t* bytebuf, unsigned char* val);

size_t bytebuf_put_bytes(bytebuf_t* bytebuf, unsigned char* vals, size_t count);
size_t bytebuf_get_bytes(bytebuf_t* bytebuf, unsigned char* vals, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* BYTEBUF_UTIL_H */
