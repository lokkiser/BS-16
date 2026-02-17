/*
 * softuart_util.c
 *
 *  Created on: 5 апр. 2024 г.
 *      Author: Rafael Boltachev
 */

#define SOFUART_DMA_HANDLE	hdma_memtomem_dma2_stream0

HAL_StatusTypeDef SOFT_UART_Transmit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,  uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint8_t_ * mem;
	uint32_t alloc_size;

	alloc_size = Size * 8;

	mem = (uint8_t_ *)malloc(alloc_size);

	HAL_DMA_Start(&SOFUART_DMA_HANDLE, (uint32_t)mem, (uint32_t)&GPIOA->ODR, alloc_size);
	HAL_DMA_PollForTransfer(&SOFUART_DMA_HANDLE0, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

	free(mem);
}
