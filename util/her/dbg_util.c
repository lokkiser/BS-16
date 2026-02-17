/*
 * ulog_util.c
 *
 *  Created on: 25 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#include "dbg_util.h"
#include "str_util.h"
#include <stdarg.h>

static char dbg_buf[MAX_STR_SIZE] = {0};
static dbg_get_time_t * dbg_get_time = NULL;
static bool dbg_use_time = false;
static bool dbg_write_now = true;

void dbg_init(dbg_get_time_t * dbg_get_time_func)
{
	dbg_get_time = dbg_get_time_func;
	dbg_use_time = dbg_get_time_func ? true : false;
	dbg_write_now = true;
	dbg_buf[0] = '\0';
}

void dbg_set_use_time(bool val)
{
	dbg_use_time = val;
}

void dbg_set_write_now(bool val)
{
	dbg_write_now = val;
}


void dbg(const char * fmt, ... )
{
	char buf[MAX_STR_SIZE];
	size_t len;


	buf[0] = '\0';

	if (dbg_use_time && dbg_get_time) {

		dbg_get_time(buf, sizeof(buf));
		len = strlen(buf);
		if (len) strcat_s(buf, sizeof(buf), " ");
	}

	len = strlen(buf);

	va_list arg;

	va_start(arg, fmt);

	vsnprintf(buf + len, sizeof(buf) - len, fmt, arg);

	va_end(arg);

	buf[sizeof(buf) - 1] = '\0';

	if (dbg_write_now) {
		dbg_write(buf);
	}
	else {
		strcat_s(dbg_buf, sizeof(dbg_buf), buf);
	}
}

void dbg_task(void)
{
	if (dbg_buf[0] != '\0') {
		dbg_write(dbg_buf);
		dbg_buf[0] = '\0';
	}
}



