/*
 * md5_util.h
 *
 *  Created on: 20 авг. 2025 г.
 *      Author: Rafael Boltachev
 */

#ifndef UTIL_MD5_UTIL_H_
#define UTIL_MD5_UTIL_H_

#include <stdint.h>

//должно быть digest[16]
void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);


#endif /* UTIL_MD5_UTIL_H_ */
