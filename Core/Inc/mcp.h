/*
 * mcp.h
 *
 *  Created on: Sep 8, 2026
 *      Author: bimpk
 */
#include "main.h"
#include "stm32g4xx_hal_i2c.h"
#ifndef INC_MCP_H_
#define INC_MCP_H_

#define mcp_address 0b01011110
#define mcp_i2c hi2c3

extern uint8_t mcp_read_data;
extern uint8_t mcp_write_data;
#endif /* INC_MCP_H_ */
uint8_t mcp_init();
