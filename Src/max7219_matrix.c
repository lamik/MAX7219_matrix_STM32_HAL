/*
 * max7219_digits.c
 *
 *  Created on: 01.02.2019
 *      Author: Mateusz Salamon
 */

#include "main.h"
#include "spi.h"
#ifndef SPI_CS_HARDWARE_CONTROL
#include "gpio.h"
#endif
#include <string.h>

#include <max7219_matrix.h>

SPI_HandleTypeDef *max7219_spi;
uint8_t Max7219PixelsBuffer[(MAX7219_X_PIXELS * MAX7219_Y_PIXELS) / 8];
uint8_t Max7219SpiBuffer[MAX7219_DEVICES * 2];

MAX7219_STATUS MAX7219_SendToDevice(uint8_t DeviceNumber, uint8_t Register, uint8_t Data)
{
	uint8_t Offset = DeviceNumber * 2;

	memset(Max7219SpiBuffer, 0x00, (MAX7219_DEVICES * 2));
	Max7219SpiBuffer[(MAX7219_DEVICES * 2)-Offset-2] = Register;
	Max7219SpiBuffer[(MAX7219_DEVICES * 2)-Offset-1] = Data;

#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);
#endif

	if(HAL_OK != HAL_SPI_Transmit(max7219_spi, Max7219SpiBuffer, (MAX7219_DEVICES * 2), 10))
	{
		return MAX7219_ERROR;
	}

#ifndef SPI_CS_HARDWARE_CONTROL
	HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
#endif

	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_SetDecodeMode(uint8_t DeviceNumber, MAX7219_DecodeMode DecodeMode)
{
	if(DeviceNumber >= MAX7219_DEVICES || DecodeMode > 4)
	{
		return MAX7219_ERROR;
	}

	if(MAX7219_OK != MAX7219_SendToDevice(DeviceNumber, MAX7219_DECODE_MODE_REGISTER, DecodeMode))
	{
		return MAX7219_ERROR;
	}

	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_SetIntensity(uint8_t DeviceNumber, MAX7219_ScanLimit Intensity)
{
	if(DeviceNumber >= MAX7219_DEVICES || Intensity > 16)
	{
		return MAX7219_ERROR;
	}

	if(MAX7219_OK != MAX7219_SendToDevice(DeviceNumber, MAX7219_INTENSITY_REGISTER, Intensity))
	{
		return MAX7219_ERROR;
	}

	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_SetScanLimit(uint8_t DeviceNumber, uint8_t Limit)
{
	if(DeviceNumber >= MAX7219_DEVICES || Limit > 8)
	{
		return MAX7219_ERROR;
	}

	if(MAX7219_OK != MAX7219_SendToDevice(DeviceNumber, MAX7219_SCAN_LIMIT_REGISTER, Limit))
	{
		return MAX7219_ERROR;
	}

	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_Shutdown(uint8_t DeviceNumber, MAX7219_ShutdownMode Shutdown)
{
	if(DeviceNumber >= MAX7219_DEVICES)
	{
		return MAX7219_ERROR;
	}

	if(MAX7219_OK != MAX7219_SendToDevice(DeviceNumber, MAX7219_SHUTDOWN_REGISTER, Shutdown?1:0))
	{
		return MAX7219_ERROR;
	}

	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_SetDisplayTest(uint8_t DeviceNumber, MAX7219_TestMode Enable)
{
	if(DeviceNumber >= MAX7219_DEVICES)
	{
		return MAX7219_ERROR;
	}

	if(MAX7219_OK != MAX7219_SendToDevice(DeviceNumber, MAX7219_DISPLAY_TEST_REGISTER, Enable?1:0))
	{
		return MAX7219_ERROR;
	}

	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_SetPixel(int x, int y, MAX7219_Color Color)
{
	 if ((x < 0) || (x >= MAX7219_X_PIXELS) || (y < 0) || (y >= MAX7219_Y_PIXELS))
		 return MAX7219_OUT_OF_RANGE;

	 switch(Color)
	 {
#if(MAX7219_MODULE_TYPE == 0)
		 case MAX7219_WHITE:   Max7219PixelsBuffer[(x/8) + (y*MAX7219_COLUMNS)] |=  (0x80 >> (x&7)); break;
		 case MAX7219_BLACK:   Max7219PixelsBuffer[(x/8) + (y*MAX7219_COLUMNS)] &= ~(0x80 >> (x&7)); break;
		 case MAX7219_INVERSE: Max7219PixelsBuffer[(x/8) + (y*MAX7219_COLUMNS)] ^=  (0x80 >> (x&7)); break;
#elif(MAX7219_MODULE_TYPE == 1)
		 case MAX7219_WHITE:   Max7219PixelsBuffer[(x/8) + ((MAX7219_PIXELS_PER_DEVICE_ROW-1) - ((y%8)) + ((y/8)*MAX7219_PIXELS_PER_DEVICE_ROW))*MAX7219_COLUMNS] |=  (1 << (x&7)); break;
		 case MAX7219_BLACK:   Max7219PixelsBuffer[(x/8) + ((MAX7219_PIXELS_PER_DEVICE_ROW-1) - ((y%8)) + ((y/8)*MAX7219_PIXELS_PER_DEVICE_ROW))*MAX7219_COLUMNS] &= ~(1 << (x&7)); break;
		 case MAX7219_INVERSE: Max7219PixelsBuffer[(x/8) + ((MAX7219_PIXELS_PER_DEVICE_ROW-1) - ((y%8)) + ((y/8)*MAX7219_PIXELS_PER_DEVICE_ROW))*MAX7219_COLUMNS] ^=  (1 << (x&7)); break;
#endif
		 default: return MAX7219_ERROR;
	 }

	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_Clear(MAX7219_Color Color)
{
	switch (Color)
	{
		case MAX7219_WHITE:
			memset(Max7219PixelsBuffer, 0xFF, (MAX7219_Y_PIXELS * MAX7219_X_PIXELS / 8));
			break;
		case MAX7219_BLACK:
			memset(Max7219PixelsBuffer, 0x00, (MAX7219_Y_PIXELS * MAX7219_X_PIXELS / 8));
			break;
		default:
			return MAX7219_ERROR;
	}
	return MAX7219_OK;
}

MAX7219_STATUS MAX7219_Display(void)
{
	uint32_t i, j;

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < MAX7219_DEVICES; j++)
		{
			Max7219SpiBuffer[(MAX7219_DEVICES * 2) - (2*j) - 2] = MAX7219_DIGIT0_REGISTER + i;
			Max7219SpiBuffer[(MAX7219_DEVICES * 2) - (2*j) - 1] = Max7219PixelsBuffer[j + (i*MAX7219_COLUMNS)];
		}

	#ifndef SPI_CS_HARDWARE_CONTROL
		HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);
	#endif

		if(HAL_OK != HAL_SPI_Transmit(max7219_spi, Max7219SpiBuffer, (MAX7219_DEVICES * 2), 10))
		{
			return MAX7219_ERROR;
		}

	#ifndef SPI_CS_HARDWARE_CONTROL
		HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
	#endif
	}

	return MAX7219_OK;
}


MAX7219_STATUS MAX7219_Init(SPI_HandleTypeDef *hspi)
{
	uint8_t i;
	max7219_spi = hspi;

	for(i = 0; i < MAX7219_DEVICES; i++)
	{
		if(MAX7219_OK != MAX7219_SetDecodeMode(i, NoDecode)) return MAX7219_ERROR;
		if(MAX7219_OK != MAX7219_SetIntensity(i, 1)) return MAX7219_ERROR;
		if(MAX7219_OK != MAX7219_SetScanLimit(i, ScanDigit0_7)) return MAX7219_ERROR;
		if(MAX7219_OK != MAX7219_SetDisplayTest(i, TestOff)) return MAX7219_ERROR;
		if(MAX7219_OK != MAX7219_Shutdown(i, NormalOperation)) return MAX7219_ERROR;
		if(MAX7219_OK != MAX7219_Clear(MAX7219_BLACK)) return MAX7219_ERROR;
	}

	return MAX7219_OK;
}
