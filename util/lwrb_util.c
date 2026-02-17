/*
 * lwrb_util.c
 *
 *  Created on: 15 дек. 2023 г.
 *      Author: Rafael Boltachev
 */

#include "lwrb_util.h"
#include "def_util.h"
#include <stddef.h>

size_t lwrb_pack_write(LWRB_VOLATILE lwrb_t* buff, const void* data, size_t btw)
{
	size_t w;

	if (lwrb_get_free(buff) < sizeof(btw) + btw) return 0;

	w = lwrb_write(buff, &btw, sizeof(btw));

	if (w != sizeof(btw)) return 0;

	return lwrb_write(buff, data, btw);
}

size_t lwrb_pack_read(LWRB_VOLATILE lwrb_t* buff, void* data)
{
	size_t rc, r;

	r = lwrb_read(buff, &rc, sizeof(rc));

	if (r != sizeof(rc)) return 0;

	return lwrb_read(buff, data, rc);
}

size_t lwrb_write16(LWRB_VOLATILE lwrb_t* buff, const void* data16, size_t btw16)
{
	return lwrb_write(buff, data16, btw16 * 2) / 2;
}

size_t lwrb_read16(LWRB_VOLATILE lwrb_t* buff, void* data16, size_t btr16)
{
	return lwrb_read(buff, data16, btr16 * 2) / 2;
}


size_t lwrb_get_free16(LWRB_VOLATILE lwrb_t* buff)
{
	return lwrb_get_free(buff) / 2;
}

size_t lwrb_get_full16(LWRB_VOLATILE lwrb_t* buff)
{
	return lwrb_get_full(buff) / 2;
}
