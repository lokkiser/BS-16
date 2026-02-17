/**
 * *****************************************************************************
 * @file    pca9535_driver.h
 * @brief   16-bit I2C-bus and SMBus, low power I/O port with interrupt
 *
 * *****************************************************************************
 */

/* Define to prevent recursive inclusion */
#ifndef PCA9535_DRV_H__
#define PCA9535_DRV_H__

/* Includes ------------------------------------------------------------------*/

#include "main.h"


//Выход прерывания с открытым стоком микросхем PCA9535 и PCA9535C активируется,
//когда любое состояние входа отличается от соответствующего состояния регистра входного порта,
//и используется для индикации системному ведущему устройству об изменении состояния входа.
//Сброс при включении питания устанавливает регистры в значения по умолчанию и инициализирует конечный автомат устройства.



//Registers 6 and 7: Configuration registers
//Этот регистр настраивает направления выводов ввода/вывода.
//Если бит в этом регистре установлен (записывается как «1»),
//соответствующий вывод порта включается как вход с высокоимпедансным выходным драйвером.
//Если бит в этом регистре сброшен (записывается как «0»),
//соответствующий вывод порта включается как выход.
//После сброса порты устройства являются входами.

/* Export constants ----------------------------------------------------------*/

/* PCA9535 registers */
#define PCA9535_REG_INPUT_0     0x00U
#define PCA9535_REG_INPUT_1     0x01U
#define PCA9535_REG_OUTPUT_0    0x02U
#define PCA9535_REG_OUTPUT_1    0x03U
#define PCA9535_REG_POL_INV_0   0x04U
#define PCA9535_REG_POL_INV_1   0x05U
#define PCA9535_REG_CONFIG_0    0x06U
#define PCA9535_REG_CONFIG_1    0x07U

/* Export typedef ------------------------------------------------------------*/

//typedef struct pca9535_s pca9535_t;
//typedef struct pca9535_s *pca9535_t *;

/**
 * @brief   Register types enumeration
 */
typedef enum 
{
    PCA9535_REG_TYPE_INPUT = 0,
    PCA9535_REG_TYPE_OUTPUT,
    PCA9535_REG_TYPE_POL_INV,
    PCA9535_REG_TYPE_CONFIG,
} pca9535_reg_type_t;

/**
 * @brief   Device port enumeration
 */
typedef enum 
{
    PCA9535_PORT_0 = 0,
    PCA9535_PORT_1,
    PCA9535_PORT_BOTH,
} pca9535_port_t;

typedef struct pca9535_s 
{
	I2C_HandleTypeDef* idx;
    unsigned short addr;
    bool busy;
    bool err;
} pca9535_t;

/* Export functions prototypes -----------------------------------------------*/

/**
 * @brief       Initialize the structure of the used device with the necessary
 *              values
 * @param[out]  pca9535 Pointer to a structure describing the device
 * @param[in]   cfg     Pointer to a structure containing initial values
 * @return
 *              - (-2): Parameter error
 *              - 0: Success
 */
int pca9535_init(pca9535_t* pca9535, 	I2C_HandleTypeDef* idx, unsigned short addr);

/**
 * @brief       Get the value of one or a pair of IC registers
 * @param[in]   pca9535     Pointer to a structure describing the device
 * @param[in]   reg_type    Register type
 * @param[in]   port        IO0, IO1 or both
 * @param[out]  value       Variable to store the result
 * @param[in]   len         Number of bytes to receive
 * @return
 *              - (-2): Parameter error
 *              - (-1): Bus error
 *              - 0: Success
 */
int pca9535_get_reg(pca9535_t* pca9535,
                    pca9535_reg_type_t reg_type,
                    pca9535_port_t port,
                    unsigned char* value,
                    int len);

/**
 * @brief       Set the value of one or a pair of IC registers
 * @param[in]   pca9535     Pointer to a structure describing the device
 * @param[in]   reg_type    Register type
 * @param[in]   port        IO0, IO1 or both
 * @param[in]   value       Variable holding the data to be sent
 * @param[in]   len         Number of bytes to send
 * @return
 *              - (-2): Parameter error
 *              - (-1): Bus error
 *              - 0: Success
 */
int pca9535_set_reg(pca9535_t* pca9535,
                    pca9535_reg_type_t reg_type,
                    pca9535_port_t port,
                    unsigned char* value,
                    int len);


int pca9535_is_ready(pca9535_t* pca9535);

#endif  /* PCA9535_DRV_H__ */
