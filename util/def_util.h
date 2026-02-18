#ifndef DEF_H
#define DEF_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>



#ifndef NULL
#define NULL	0
#endif
/*
#ifndef BOOL
#define BOOL	unsigned char
#endif

#ifndef bool
#define bool    BOOL
#endif
*/
#ifndef TRUE
#define TRUE	true //!!1
#endif

#ifndef FALSE
#define FALSE	false//!TRUE
#endif

#ifndef ON
#define ON		1
#endif

#ifndef OFF
#define OFF		0
#endif

#ifndef GETBIT
//#define GETBIT(v, n)	((0u == (v & (1<<n))) ? 0u : 1u)
#define GETBIT(val, number)  (((unsigned char*)&val)[(number) / 8]  & (1 << (number % 8)) ? 1u : 0u)
#endif

#ifndef SETBIT
//#define SETBIT(v, n)	(v |= (1<<n))
#define SETBIT(val, number)   ((unsigned char*)&val)[(number) / 8] |= (1 << ((number) % 8) )
#endif

#ifndef CLRBIT
//#define CLRBIT(v, n)	(v &= (~(1<<n)))
#define CLRBIT(val, number)   ((unsigned char*)&val)[(number) / 8] &= ( ~(unsigned char)(1 << ((number) % 8)) )
#endif

#ifndef INVBIT
//#define INVBIT(v, n)	(v ^= (1<<n))
#define INVBIT(val, number)   ((unsigned char*)&val)[(number) / 8] ^= (1 << ((number) % 8) )
#endif

#ifndef RANDOM
#define RANDOM(a)	(rand() % (a))
#endif

#ifndef MIN
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef BOUND
#define BOUND(low, high, value)	MAX(MIN(high, value), low)
#endif

#ifndef BOUNDSMALL
#define BOUNDSMALL(low, high, value)	((value) < (low) ? (low) : ((value) > (high) ? (high) : (value)))
#endif


#ifndef NOTINGATE
#define NOTINGATE(low, high, value)	(((value) < (low)) || ((value) > (high)))
#endif

#ifndef INGATE
#define INGATE(low, high, value)	(((value) >= (low)) && ((value) <= (high)))
#endif


#ifndef LF
#define LF	"\n"
#endif


#ifndef CRLF
#define CRLF	"\r\n"
#endif

#ifndef RN
#define RN	CRLF
#endif


#ifndef PERCENT
#define PERCENT(val, pct) (val * pct / 100)
#endif

#ifndef PERCENT_LOW
#define PERCENT_LOW(val, pct) (val - PERCENT(val, pct))
#endif

#ifndef PERCENT_HI
#define PERCENT_HI(val, pct) (val + PERCENT(val, pct))
#endif

#ifndef PERCENT_INGATE
#define PERCENT_INGATE(val, cons, pct) ((val >= PERCENT_LOW(cons, pct)) && (val <= PERCENT_HI(cons, pct)))
#endif


//нечетный
#ifndef ODD
#define ODD(x) ((x) % 2)
#endif

//четный
#ifndef EVEN
#define EVEN(x) (!ODD(x))
#endif

#ifndef MODULE
#define MODULE(x) ((x) < 0 ? -(x) : (x))
#endif

#ifndef MIDDLE
#define MIDDLE(l, h) ((l) + ((h) - (l)) / 2)
#endif


#ifndef INVALID_BYTE
#define INVALID_BYTE	0xFF
#endif

#ifndef INVALID_WORD
#define INVALID_WORD	0xFFFF
#endif

#ifndef INVALID_DWORD
#define INVALID_DWORD	0xFFFFFFFF
#endif

#ifndef INVALID_QWORD
#define INVALID_QWORD	0xFFFFFFFFFFFFFFFF
#endif

#ifndef INVALID_CHAR
#define INVALID_CHAR	–128
#endif

#ifndef INVALID_SHORT
#define INVALID_SHORT	–32768
#endif

#ifndef INVALID_INT
#define INVALID_INT		−2147483648
#endif

#ifndef INVALID_LONGLONG
#define INVALID_LONGLONG	-9223372036854775808
#endif

#ifndef INVALID_FLOAT
#define INVALID_FLOAT	-3.4E-38
#endif

#ifndef INVALID_DOUBLE
#define INVALID_DOUBLE	-1.7E-308
#endif

#ifndef MAX_STR_SIZE
#define MAX_STR_SIZE	1024
#endif

#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE	128
#endif

#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE	1600
#endif

#ifndef TMP_FILE_EXT
#define TMP_FILE_EXT	"tmp"
#endif

typedef unsigned int f_stream_read_t(void * afile, void * data, unsigned int data_len);
typedef unsigned int f_stream_write_t(void * afile, void * data, unsigned int data_len);


#endif /* DEF_H */
