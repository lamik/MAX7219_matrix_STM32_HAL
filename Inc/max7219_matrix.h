/*
 * max7219_digits.h
 *
 *  Created on: 01.02.2019
 *      Author: Mateusz Salamon
 */

#ifndef MAX7219_MATRIX_H_
#define MAX7219_MATRIX_H_

//
//	Configuration
//
#define MAX7219_ROW_DEVICES		8
#define MAX7219_COLUMN_DEVICES	1
//#define SPI_CS_HARDWARE_CONTROL

#define MAX7219_MODULE_TYPE 0 // Description below
/*
 * 			MAX7219_MODULE_TYPE 0
 * 	   01234567 01234567 01234567 01234567
 *    0|--------|--------|--------|--------|
 *   -1|        |        |        |        |-
 * I -2|        |        |        |        |- O
 * N -3|        |        |        |        |- U
 *   -4|        |        |        |        |- T
 *   -5|        |        |        |        |-
 *   -6|        |        |        |        |-
 *    7|--------|--------|--------|--------|
 *
 *
 * 			MAX7219_MODULE_TYPE 1
 * 	   76543210 76543210 76543210 76543210
 *    7|--------|--------|--------|--------|
 *   -6|        |        |        |        |-
 * I -5|        |        |        |        |- O
 * N -4|        |        |        |        |- U
 *   -3|        |        |        |        |- T
 *   -2|        |        |        |        |-
 *   -1|        |        |        |        |-
 *    0|--------|--------|--------|--------|
 *
 */

//
//	Resolution
//
#define MAX7219_PIXELS_PER_DEVICE_ROW 8
#define MAX7219_PIXELS_PER_DEVICE_COLUMN 8
#define MAX7219_X_PIXELS	(MAX7219_PIXELS_PER_DEVICE_ROW*MAX7219_COLUMN_DEVICES)
#define MAX7219_Y_PIXELS	(MAX7219_PIXELS_PER_DEVICE_COLUMN*MAX7219_ROW_DEVICES)
#define MAX7219_DEVICES		(MAX7219_ROW_DEVICES * MAX7219_COLUMN_DEVICES)

//
//	Registers
//
#define MAX7219_NOOP_REGISTER   0x00
#define MAX7219_DIGIT0_REGISTER 0x01
#define MAX7219_DIGIT1_REGISTER 0x02
#define MAX7219_DIGIT2_REGISTER 0x03
#define MAX7219_DIGIT3_REGISTER 0x04
#define MAX7219_DIGIT4_REGISTER 0x05
#define MAX7219_DIGIT5_REGISTER 0x06
#define MAX7219_DIGIT6_REGISTER 0x07
#define MAX7219_DIGIT7_REGISTER 0x08
#define MAX7219_DECODE_MODE_REGISTER  0x09
#define MAX7219_INTENSITY_REGISTER   0x0A
#define MAX7219_SCAN_LIMIT_REGISTER   0x0B
#define MAX7219_SHUTDOWN_REGISTER    0x0C
#define MAX7219_DISPLAY_TEST_REGISTER 0x0F

typedef enum
{
	NoDecode 		= 0,
	DecodeFor0		= 1,
	DecodeFor3_0	= 2,
	DecodeFor7_0	= 3
}MAX7219_DecodeMode;

typedef enum
{
	SleepMode 		= 0,
	NormalOperation		= 1
}MAX7219_ShutdownMode;

typedef enum
{
	TestOff		= 0,
	TestMode	= 1
}MAX7219_TestMode;

typedef enum
{
	ScanDigit0		= 0,
	ScanDigit0_1	= 1,
	ScanDigit0_2	= 2,
	ScanDigit0_3	= 3,
	ScanDigit0_4	= 4,
	ScanDigit0_5	= 5,
	ScanDigit0_6	= 6,
	ScanDigit0_7	= 7,
}MAX7219_ScanLimit;

typedef enum
{
	MAX7219_OK 					= 0,
	MAX7219_ERROR				= 1,
	MAX7219_UNSUPPORTED_CHAR	= 2,
	MAX7219_OUT_OF_RANGE		= 3
}MAX7219_STATUS;

typedef enum
{
	MAX7219_BLACK = 0,
	MAX7219_WHITE = 1,
	MAX7219_INVERSE = 2
}MAX7219_Color;

MAX7219_STATUS MAX7219_Init(SPI_HandleTypeDef *hspi);

MAX7219_STATUS MAX7219_SetDecodeMode(uint8_t DeviceNumber, MAX7219_DecodeMode DecodeMode);
MAX7219_STATUS MAX7219_SetIntensity(uint8_t DeviceNumber, uint8_t Intensity);
MAX7219_STATUS MAX7219_SetScanLimit(uint8_t DeviceNumber, MAX7219_ScanLimit Limit);
MAX7219_STATUS MAX7219_Shutdown(uint8_t DeviceNumber, MAX7219_ShutdownMode Shutdown);
MAX7219_STATUS MAX7219_SetDisplayTest(uint8_t DeviceNumber, MAX7219_TestMode Enable);

MAX7219_STATUS MAX7219_SetPixel(int x, int y, MAX7219_Color Color);
MAX7219_STATUS MAX7219_Clear(MAX7219_Color Color);
MAX7219_STATUS MAX7219_Display(void);

#endif /* MAX7219_MATRIX_H_ */
