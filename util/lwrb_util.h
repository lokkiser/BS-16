/*
 * lwrb_util.h
 *
 *  Created on: 15 дек. 2023 г.
 *      Author: Rafael Boltachev
 *
 *      Библиотека для lwrb
 */

#ifndef UTIL_LWRB_UTIL_H_
#define UTIL_LWRB_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lwrb.h"

//положить пакет с длиной btq в lwrb
size_t lwrb_pack_write(LWRB_VOLATILE lwrb_t* buff, const void* data, size_t btw);
//прочитать пакет (возвражает длину пакета
size_t lwrb_pack_read(LWRB_VOLATILE lwrb_t* buff, void* data);


size_t lwrb_write16(LWRB_VOLATILE lwrb_t* buff, const void* data16, size_t btw16);
size_t lwrb_read16(LWRB_VOLATILE lwrb_t* buff, void* data16, size_t btr16);
size_t lwrb_get_free16(LWRB_VOLATILE lwrb_t* buff);
size_t lwrb_get_full16(LWRB_VOLATILE lwrb_t* buff);


#ifdef __cplusplus
}
#endif


#endif /* UTIL_LWRB_UTIL_H_ */
