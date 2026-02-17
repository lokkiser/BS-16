#ifndef vector_utilH
#define vector_utilH

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct _vector_item {
  struct _vector_item * next;
  unsigned char data[];
} vector_item_t;

typedef struct _vector {
    vector_item_t * first;
    size_t data_size;
    size_t count;
    bool busy;
} vector_t;

#pragma pack(pop)

//init with vector item size
void vector_init(vector_t *v, size_t data_size);

//push data
bool vector_push(vector_t * v, void * data);

//get vector count
size_t vector_count(vector_t * v);

//get pointer to vector data from index
void * vector_get_at(vector_t *v, size_t index);

//set vector data from index
bool vector_set_at(vector_t *v, size_t index, void * data);

//free vector
void vector_free(vector_t *v);

//get first element from vector (FIFO)
bool vector_pop(vector_t *v, void * data);

//delete vector data from index
bool vector_delete_at(vector_t *v, size_t index);


#ifdef __cplusplus
}
#endif

#endif
