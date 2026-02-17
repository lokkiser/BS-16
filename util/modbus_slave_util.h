#ifndef MODBUS_SLAVE_UTIL_H
#define MODBUS_SLAVE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "modbus_util.h"

#ifndef GETBIT
#define GETBIT(v, n)	((0u == (v & (1<<n))) ? 0u : 1u)
#endif

#ifndef SETBIT
#define SETBIT(v, n)	(v |= (1<<n))
#endif

#ifndef CLRBIT
#define CLRBIT(v, n)	(v &= (~(1<<n)))
#endif

#ifndef HTONS
#define HTONS(a)			((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#endif


//need to call this function when data arrives, (packet - rcv data, buf - buffer for operation)
void modbus_slave_recv(uint8_t * packet, uint16_t packet_len, uint8_t * buf, uint16_t buf_size);


//the functions located below must be written in your program
void modbus_slave_send(uint8_t * packet, uint16_t packet_len);
//functions below should return 0 on success, or a modbus error
uint8_t modbus_slave_write_register(uint8_t address, uint8_t function_code, uint16_t number, uint16_t value);
uint8_t modbus_slave_read_register(uint8_t address, uint8_t function_code, uint16_t number, uint16_t * value);
uint8_t modbus_slave_read_coil(uint8_t address, uint8_t function_code, uint16_t number, uint8_t * value);
uint8_t modbus_slave_write_coil(uint8_t address, uint8_t function_code, uint16_t number,  uint8_t value);




#ifdef __cplusplus
}
#endif

#endif
