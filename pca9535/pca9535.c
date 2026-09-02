/**
 * *****************************************************************************
 * @file    pca9535_driver.c
 * @brief   16-bit I2C-bus and SMBus, low power I/O port with interrupt
 *
 * *****************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "pca9535.h"


#define PCA9535_I2C_TIMEOUT		10

pca9535_t pca9535_t_DD3_MB, pca9535_t_DD4_MB, pca9535_t_2, pca9535_t_3;
uint8_t pca_mb_init_config[2] = {};
/**
 * @brief       Get data from I/O port expander
 * @param[in]   dev_idx     Device index on the board from 0 to
 *              PCA9535_USED_NUM - 1
 * @param[in]   dev_addr    Target device address
 * @param[in]   reg_addr    Target register address
 * @param[out]  dest        Buffer for storing data received from the device
 * @param[in]   len         Data buffer length in bytes
 * @return
 *              - (-2): Parameter error
 *              - (-1): Bus error
 *              - 0: Success
 */
int pca9535_i2c_read(   I2C_HandleTypeDef *hi2c,
                        unsigned int dev_addr,
                        unsigned char reg_addr,
                        unsigned char* dest,
                        unsigned int len)
{
    /*------------------------------------------------------------------------*/
    /* Specify a custom I2C read function here -------------------------------*/
    /* The function must return a value (-1) in case of a hardware error -----*/
    /*------------------------------------------------------------------------*/
	if (HAL_I2C_Master_Transmit(hi2c, dev_addr, &reg_addr, 1, PCA9535_I2C_TIMEOUT) != HAL_OK) {
		return -1;
	}

    return HAL_I2C_Master_Receive(hi2c, dev_addr, dest, len, PCA9535_I2C_TIMEOUT) == HAL_OK ? 0 : -1;
}

/**
 * @brief       Send data to I/O port expander
 * @param[in]   dev_idx     Device index on the board from 0 to
 *              PCA9535_USED_NUM - 1
 * @param[in]   dev_addr    Target device address
 * @param[in]   reg_addr    Target register address
 * @param[in]   src         Buffer containing data to send
 * @param[in]   len         Data buffer length in bytes
 * @return
 *              - (-2): Parameter error
 *              - (-1): HW error
 *              - 0: Success
 */
int pca9535_i2c_write(  I2C_HandleTypeDef *hi2c,
                        unsigned int dev_addr,
                        unsigned char reg_addr,
                        unsigned char *src,
                        unsigned int len)
{
    /*------------------------------------------------------------------------*/
    /* Specify a custom I2C write function here ------------------------------*/
    /* The function must return a value (-1) in case of a hardware error -----*/
    /*------------------------------------------------------------------------*/

	unsigned char buf[255];
	int i;

	buf[0] = reg_addr;
	for (i = 0; i < len; ++i) buf[i + 1] = src[i];


	return HAL_I2C_Master_Transmit(hi2c, dev_addr, buf, 1 + len, PCA9535_I2C_TIMEOUT) == HAL_OK ?  0 : -1;;
}

/* Private functions prototypes ----------------------------------------------*/

/**
 * @brief       Get register address to write according to input parameters
 * @param[in]   reg_type    Register type
 * @param[in]   port        IO0, IO1 or both
 * @param[in]   len         Number of bytes to process
 * @param[out]  reg_addr    Pointer to a variable to store the register address
 * @return
 *              - (-1): Parameter error
 *              - 0: Success
 */
static int _pca9535_get_reg_addr(   pca9535_reg_type_t reg_type,
                                    pca9535_port_t port,
                                    int len,
                                    unsigned char *reg_addr);

/* Private functions ---------------------------------------------------------*/

/* Get register address to write according to input parameters */
static int _pca9535_get_reg_addr(pca9535_reg_type_t reg_type, pca9535_port_t port, int len, unsigned char* reg_addr) 
{
    if (port == PCA9535_PORT_BOTH && len != 2) {
        return -1;
    } else if ((port == PCA9535_PORT_0 || port == PCA9535_PORT_1) && len != 1) {
        return -1;
    } else if (port != PCA9535_PORT_0 && port != PCA9535_PORT_1 && port != PCA9535_PORT_BOTH) {
        return -1;
    }
    switch (reg_type) {
    case PCA9535_REG_TYPE_INPUT:    *reg_addr = PCA9535_REG_INPUT_0;    break;
    case PCA9535_REG_TYPE_OUTPUT:   *reg_addr = PCA9535_REG_OUTPUT_0;   break;
    case PCA9535_REG_TYPE_POL_INV:  *reg_addr = PCA9535_REG_POL_INV_0;  break;
    case PCA9535_REG_TYPE_CONFIG:   *reg_addr = PCA9535_REG_CONFIG_0;   break;
    default: return -1;
    }
    if (port == PCA9535_PORT_1) {
        (*reg_addr)++;
    }
    return 0;
}

/* Export functions ----------------------------------------------------------*/

/* Initialize the structure of the used device with the necessary values */
int pca9535_init(pca9535_t * pca9535, 	I2C_HandleTypeDef * idx, unsigned short addr)
{
    if (pca9535 == NULL ||
        idx == NULL) {
        return -2;
    }
    pca9535->idx = idx;
    pca9535->addr = (addr << 1);
    pca9535->busy = false;
    pca9535->err = false;
    return 0;
}


int pca9535_is_ready(pca9535_t * pca9535)
{
	 return HAL_I2C_IsDeviceReady(pca9535->idx, pca9535->addr, 3, 1000) == HAL_OK ? 0 : -3;
}


/* Get the value of one or a pair of IC registers */
int pca9535_get_reg(pca9535_t * pca9535, pca9535_reg_type_t reg_type, pca9535_port_t port, unsigned char *value, int len) {
    if (pca9535 == NULL ||
        value == NULL) {
        return -2;
    }
    int ret = 0;
    unsigned char reg_addr = 0;
    if (_pca9535_get_reg_addr(reg_type, port, len, &reg_addr) != 0) {
        return -2;
    }
    while (pca9535->busy == true);
    pca9535->busy = true;
    if ((ret = pca9535_i2c_read(pca9535->idx, pca9535->addr, reg_addr, value, len)) != 0) {
        if (ret == -1) {
            pca9535->err = true;
            ret = -1;
        } else if (ret == -2) {
            ret = -2;
        }
    }
    pca9535->busy = false;
    return (int)ret;
}

/* Set the value of one or a pair of IC registers */
int pca9535_set_reg(pca9535_t * pca9535, pca9535_reg_type_t reg_type, pca9535_port_t port, unsigned char *value, int len) {
    if (pca9535 == NULL ||
        value == NULL) {
        return -2;
    }
    int ret = 0;
    unsigned char reg_addr = 0;
    if (_pca9535_get_reg_addr(reg_type, port, len, &reg_addr) != 0) {
        return -2;
    }
    while (pca9535->busy == true);
    pca9535->busy = true;
    if ((ret = pca9535_i2c_write(pca9535->idx, pca9535->addr, reg_addr, value, len)) != 0) {
        if (ret == -1) {
            pca9535->err = true;
            ret = -1;
        } else if (ret == -2) {
            ret = -2;
        }
    }
    pca9535->busy = false;
    return (int)ret;
}
