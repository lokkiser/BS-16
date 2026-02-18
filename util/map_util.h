#ifndef dynbufH
#define dynbufH

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
/*
#ifndef BOOL
#define BOOL	unsigned char
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif
*/
#ifndef NULL
#define NULL	0
#endif

#pragma pack(push, 1)

typedef struct map_item {
  struct map_item * next;
  size_t key_len;
  size_t value_len;
  unsigned char data[];
} map_item_t;


typedef struct _map {
  map_item_t * first;
  bool busy;
  size_t count;
} map_t;

#pragma pack(pop)

//init
void map_init(map_t * map);

//put key and value
bool map_put(map_t * map, void * key, size_t key_len, void * value, size_t value_len);

//get value with value_size
void * map_get(map_t * map, void * key, size_t key_len, size_t * value_size);

//clear
void map_clear(map_t * map);

//delete
bool map_delete(map_t * map, void * key, size_t key_len);

//change value
bool map_change(map_t * map, void * key, size_t key_len, void * value, size_t value_len);

#ifdef __cplusplus
}
#endif

#endif
