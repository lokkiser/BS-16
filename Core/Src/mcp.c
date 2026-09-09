#include "mcp.h"

uint8_t mcp_read_data = 0;
uint8_t mcp_write_data = 0x7F;
uint8_t mcp_init()
{
    if (HAL_I2C_Master_Receive(&mcp_i2c, mcp_address, &mcp_read_data, 1, 100) != HAL_OK)
    {
        return 0;
    }
    HAL_I2C_Master_Transmit(&mcp_i2c, mcp_address, &mcp_write_data, 1, 100);
    return 1;
}
