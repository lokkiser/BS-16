#include "bytebuf_util.h"

void bytebuf_clear(bytebuf_t * bytebuf)
{
    bytebuf->windex = 0;
    bytebuf->rindex = 0;
    bytebuf->available = 0;
}

void bytebuf_init(bytebuf_t * bytebuf, unsigned char * buf, size_t buf_len)
{
   bytebuf->buf = buf;
   bytebuf->buf_len = buf_len;
   bytebuf_clear(bytebuf);
}


bool bytebuf_put_byte(bytebuf_t * bytebuf, unsigned char val)
{
   if (bytebuf->available >= bytebuf->buf_len)
     return false;


   bytebuf->buf[bytebuf->windex] = val;
   bytebuf->windex++;
   if (bytebuf->windex >= bytebuf->buf_len) {
     bytebuf->windex = 0;
   }
   bytebuf->available++;
   return true;
}


size_t bytebuf_put_bytes(bytebuf_t * bytebuf, unsigned char *vals, size_t count)
{
  size_t i;

  for (i = 0; i < count; ++i) {
     if (!bytebuf_put_byte(bytebuf, vals[i])) break;
  }

  return i;
}



bool bytebuf_get_byte(bytebuf_t * bytebuf, unsigned char * val)
{
    if (bytebuf->available < 1)
      return false;

    *val = bytebuf->buf[bytebuf->rindex];
    bytebuf->rindex++;
    if (bytebuf->rindex >= bytebuf->buf_len) {
      bytebuf->rindex = 0;
    }
    bytebuf->available--;

   return true;
}



size_t bytebuf_get_bytes(bytebuf_t * bytebuf, unsigned char * vals, size_t count)
{
  size_t i;

  for (i = 0; i < count; ++i) {
     if (!bytebuf_get_byte(bytebuf, &vals[i])) break;
  }

  return i;
}

