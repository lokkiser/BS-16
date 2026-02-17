/*
 * dynbuf_util.h
 *
 *  Created on: 28 мар. 2024 г.
 *      Author: Rafael Boltachev
 *
 *      Библиотека динамического буфера
 */

#include "dynbuf_util.h"
#include <string.h>



void dynamic_buf_init(dynamic_buf_t * dynamic_buf, size_t max_alloc_size)
{
  dynamic_buf->first = NULL;
  dynamic_buf->busy = false;
  dynamic_buf->max_alloc_size = max_alloc_size;
  dynamic_buf->alloc_size = 0;
  dynamic_buf->data_size = 0;
  dynamic_buf->count = 0;
}

bool dynamic_buf_push(dynamic_buf_t* dynamic_buf, void* data, size_t data_len)
{
  dynamic_buf_item_t* item;
  size_t alloc_size;

  alloc_size = sizeof(dynamic_buf_item_t) + data_len;


  if (dynamic_buf->alloc_size + alloc_size > dynamic_buf->max_alloc_size) return false;

  if (dynamic_buf->first) {
     item = dynamic_buf->first;
     while (item->next) {
       item = item->next;
     }
     item->next = (dynamic_buf_item_t  *)malloc(alloc_size);
     item = item->next;
  }
  else {
     dynamic_buf->first = (dynamic_buf_item_t  *)malloc(alloc_size);
     item = dynamic_buf->first;
  }

  if (!item) {
       return false;
  }

  memcpy(item->data, data, data_len);
  item->data_len = data_len;
  item->next = NULL;
  dynamic_buf->alloc_size += alloc_size;
  dynamic_buf->data_size += data_len;
  dynamic_buf->count++;
  return true;
}



bool dynamic_buf_pop(dynamic_buf_t * dynamic_buf, void * buf, size_t buf_size, size_t * data_len)
{
  dynamic_buf_item_t  * item;
  size_t alloc_size;


  *data_len = 0;

  if (!dynamic_buf->first) return false;

  alloc_size = sizeof(dynamic_buf_item_t) + dynamic_buf->first->data_len;


  *data_len = dynamic_buf->first->data_len > buf_size ? buf_size : dynamic_buf->first->data_len;
  memcpy(buf, dynamic_buf->first->data, *data_len);
  item = dynamic_buf->first->next;
  dynamic_buf->data_size -= dynamic_buf->first->data_len;
  free(dynamic_buf->first);
  dynamic_buf->first = item;
  dynamic_buf->alloc_size -= alloc_size;
  dynamic_buf->count--;
  return true;
}



void dynamic_buf_clear(dynamic_buf_t * dynamic_buf)
{
  dynamic_buf_item_t  * item, * next;

  //if (!dynamic_buf->first) return;

  item = dynamic_buf->first;
  while (item) {
     next = item->next;
     free(item);
     item = next;
  }
  dynamic_buf->first = NULL;
  dynamic_buf->alloc_size = 0;
  dynamic_buf->data_size = 0;
  dynamic_buf->count = 0;
}


bool dynamic_buf_is_empty(dynamic_buf_t * dynamic_buf)
{
   return dynamic_buf->first == NULL;
}

bool dynamic_buf_get_busy(dynamic_buf_t * dynamic_buf)
{
   return dynamic_buf->busy;
}

void dynamic_buf_set_busy(dynamic_buf_t * dynamic_buf, bool value)
{
	dynamic_buf->busy = value;
}

size_t dynamic_buf_get_data_size(dynamic_buf_t * dynamic_buf)
{
	return dynamic_buf->data_size;
}

size_t dynamic_buf_get_free_size(dynamic_buf_t * dynamic_buf)
{
	return dynamic_buf->max_alloc_size - dynamic_buf->alloc_size;
}


size_t dynamic_buf_get_count(dynamic_buf_t * dynamic_buf)
{
	return dynamic_buf->count;
}


void * dynamic_buf_get_at(dynamic_buf_t * dynamic_buf, size_t index, size_t * data_len)
{
	  dynamic_buf_item_t  * item;
	  size_t i;

	  i = 0;
	  item = dynamic_buf->first;
	  while (item) {
		 if (index == i) {
			 *data_len = item->data_len;
			 return item->data;
		 }
	     item = item->next;
	     i++;
	  }

	  *data_len = 0;
	  return NULL;
}

bool dynamic_buf_delete_at(dynamic_buf_t * dynamic_buf, size_t index)
{
	  dynamic_buf_item_t  * item, * next, * prev;
	  size_t i;

	  i = 0;
	  item = dynamic_buf->first;
	  prev = NULL;
	  while (item) {
		 if (index == i) {
			 next = item->next;
			 dynamic_buf->data_size -= item->data_len;
			 dynamic_buf->alloc_size -= (sizeof(dynamic_buf_item_t) + item->data_len);
			 free(item);
			 if (prev) prev->next = next;
			 else dynamic_buf->first = next;
			 dynamic_buf->count--;
			 return true;
		 }
	     prev = item;
	     item = item->next;
	     i++;
	  }

	  return false;
}

bool dynamic_buf_pop_line(dynamic_buf_t * dynamic_buf, void * buf, size_t buf_size, size_t * data_len)
{
  size_t size;
  unsigned char * ptr;
  size_t dl;
  size_t alloc_size;
  dynamic_buf_item_t  * item;

  *data_len = 0;

  if (!dynamic_buf->first) return false;

  size = buf_size;
  ptr = buf;


  while (size) {

    if (!dynamic_buf->first) {
       return buf_size > size;
    }

    if (size == dynamic_buf->first->data_len) {
      dynamic_buf_pop(dynamic_buf, ptr, size, &dl);
      *data_len += dl;
      return true;
    }
    else if (size > dynamic_buf->first->data_len) {
      dynamic_buf_pop(dynamic_buf, ptr, dynamic_buf->first->data_len, &dl);
      *data_len += dl;
      ptr += dl;
      size -= dl;
    }
    else if (size < dynamic_buf->first->data_len) {

      alloc_size = sizeof(dynamic_buf_item_t) + dynamic_buf->first->data_len - size;
      dynamic_buf->alloc_size -= (sizeof(dynamic_buf_item_t) + dynamic_buf->first->data_len);
      dynamic_buf->alloc_size += alloc_size;

      item = (dynamic_buf_item_t  *)malloc(alloc_size);



      memcpy(ptr, dynamic_buf->first->data, size);
      *data_len += size;
      memcpy(item->data, dynamic_buf->first->data + size, dynamic_buf->first->data_len - size);
      item->data_len = dynamic_buf->first->data_len - size;
      item->next = dynamic_buf->first->next;
      free(dynamic_buf->first);
      dynamic_buf->first = item;
      dynamic_buf->data_size -= size;
      return true;
    }


  }

  return buf_size > size;
}
