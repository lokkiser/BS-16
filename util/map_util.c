#include "map_util.h"
#include <string.h>



bool map_put(map_t * map, void * key, size_t key_len, void * value, size_t value_len)
{
	  map_item_t * item;
	  size_t alloc_size;

	  alloc_size = sizeof(map_item_t) + key_len + value_len;



	  if (map->first) {
	     item = map->first;
	     while (item->next) {
	       item = item->next;
	     }
	     item->next = (map_item_t  *)malloc(alloc_size);
	     item = item->next;
	  }
	  else {
		  map->first = (map_item_t  *)malloc(alloc_size);
		  item = map->first;
	  }

	  if (!item) {
	       return false;
	  }

	  item->key_len = key_len;
	  item->value_len = value_len;
	  memcpy(item->data, key, key_len);
	  memcpy(item->data + key_len, value, value_len);
	  item->next = NULL;
	  map->count++;
	  return true;
}

void * map_get(map_t * map, void * key, size_t key_len, size_t * value_len)
{
	  map_item_t  * item;

	  item = map->first;
	  while (item) {
		  if ((item->key_len >= key_len) && (memcmp(item->data, key, key_len) == 0)) {
			  *value_len = item->value_len;
			  return item->data + key_len;
		  }
		  item = item->next;
	  }

	  *value_len = 0;
	  return NULL;
}

void map_clear(map_t * map)
{
	map_item_t  * item, * next;

	if (!map->first) return;

	item = map->first;
	while (item) {
		next = item->next;
		free(item);
		item = next;
	}
	map->first = NULL;
	map->count = 0;
}

void map_init(map_t * map)
{
	map->first = NULL;
	map->count = 0;
	map->busy = false;
}

bool map_delete(map_t * map, void * key, size_t key_len)
{
	  map_item_t  * item, * prev, * next;

	  item = map->first;
	  prev = NULL;
	  while (item) {
		  if ((item->key_len == key_len) && (memcmp(item->data, key, key_len) == 0)) {
			  next = item->next;
			  free(item);
			  if (prev) prev->next = next;
			  else map->first = next;
			  map->count--;
			  return true;
		  }
		  prev = item;
		  item = item->next;
	  }

	  return false;
}


bool map_change(map_t * map, void * key, size_t key_len, void * value, size_t value_len)
{
	  map_item_t  * item, * prev, * next;
	  size_t alloc_size;

	  item = map->first;
	  prev = NULL;
	  while (item) {
		  if ((item->key_len == key_len) && (memcmp(item->data, key, key_len) == 0)) {
			  next = item->next;
			  free(item);
			  alloc_size = sizeof(map_item_t) + key_len + value_len;
			  item = (map_item_t  *)malloc(alloc_size);
			  item->key_len = key_len;
			  item->value_len = value_len;
			  memcpy(item->data, key, key_len);
			  memcpy(item->data + key_len, value, value_len);
			  item->next = next;
			  if (prev) prev->next = item;
			  else map->first = item;
			  return true;
		  }
		  prev = item;
		  item = item->next;
	  }

	  return false;
}
