/*
 * statbuf_util.h
 *
 *  Created on: 14 мая 2024 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_STATBUF_UTIL_H_
#define UTIL_STATBUF_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include "def_util.h"
#include <dynbuf_util.h>


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



typedef struct _line_buf 
{
  dynamic_buf_t dynamic_buf;
  size_t firs_cnt;
} line_buf_t;


//init static buffer
void line_buf_init(line_buf_t* line_buf);

//put data with size data_len
bool line_buf_put(line_buf_t* line_buf, void* data, size_t data_len);


bool line_buf_pop(line_buf_t* line_buf, void* buf, size_t buf_size, size_t* data_len);

//clear static buffer
void line_buf_clear(line_buf_t* line_buf);


#ifdef __cplusplus
}
#endif


#endif /* UTIL_STATBUF_UTIL_H_ */
