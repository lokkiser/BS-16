/*
 * linebuf_util.c
 *
 *  Created on: 14 мая 2024 г.
 *      Author: Rafael Boltachev
 */


#include "linebuf_util.h"

void line_buf_init(line_buf_t * line_buf)
{
  dynamic_buf_init(&line_buf->dynamic_buf, 10000);
  line_buf->firs_cnt = 0;
}


bool line_buf_put(line_buf_t * line_buf, void * data, size_t data_len)
{
     return dynamic_buf_push(&line_buf->dynamic_buf, data, data_len);
}


bool line_buf_pop(line_buf_t * line_buf, void * buf, size_t buf_size, size_t * data_len)
{

  dynamic_buf_item_t  * item;
  size_t alloc_size, new_data_len;
  unsigned char * ptr;
  size_t count;
  size_t dl;

  if (!line_buf->dynamic_buf.first) return FALSE;

  ptr = buf;
  count = buf_size;
  while (line_buf->dynamic_buf.first && count && (count >= line_buf->dynamic_buf.first->data_len)) {
        if (!dynamic_buf_pop(line_buf, ptr, line_buf->dynamic_buf.first->data_len, &dl)) {
               *data_len = buf_size - count;
               line_buf->firs_cnt = 0;
               return TRUE;
        }
        count -= dl;
        ptr += dl;
  }

  if (!count) {
    *data_len = buf_size;
    line_buf->firs_cnt = 0;
    return TRUE;
  }

  if (!line_buf->dynamic_buf.first) {
    *data_len = buf_size - count;
    line_buf->firs_cnt = 0;
    return TRUE;
  }

  memcpy(ptr, line_buf->dynamic_buf.first->data + line_buf->firs_cnt, count);
  *data_len = buf_size;

  line_buf->firs_cnt += count;

  return TRUE;
}

void line_buf_clear(line_buf_t * line_buf)
{
  dynamic_buf_clear(&line_buf->dynamic_buf);
  line_buf->firs_cnt = 0;
}
