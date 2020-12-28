/*
 * ads1115.c
 *
 *  Created on: Sep 24, 2020
 *      Author: SchofieChen
 */
#include "main.h"
#include "Delay.h"
#include "ads1115.h"
extern I2C_HandleTypeDef hi2c1;
extern unsigned char ADSwrite[6];
extern uint8_t ADS1115_ADDRESS;
extern int16_t reading;
extern const float voltageConv;


void Read_I2C(void)
{
	unsigned char TX_buffer[1];
	unsigned char RX_buffer[1];
	unsigned char i2c_addr = 0x90;

	TX_buffer[0] = 0x55;
	HAL_I2C_Master_Transmit(&hi2c1, i2c_addr, (uint8_t *)TX_buffer, 1, 100);
	HAL_I2C_Master_Receive(&hi2c1, i2c_addr+1, RX_buffer, 1, 100);
	delay_ms(1);
}

void Read_ads1115(ADS1115_BOARD * ADS1115_BOARD_SELECT,uint8_t ADDRESS)
{
	for( int i = 0; i < 2; i++) {
		ADSwrite[0] = 0x01;
		switch(i)
		{
			case(0):
				ADSwrite[1] = 0x81; // 10000001 CH0+ CH1
			break;
			case(1):
				ADSwrite[1] = 0xB1;// 10110001 CH2+ CH3
			break;
		}
		ADSwrite[2] = 0x83; // 10000011
		HAL_I2C_Master_Transmit(&hi2c1, ADDRESS<<1, ADSwrite, 3, 100);
		ADSwrite[0] = 0x00;
		HAL_I2C_Master_Transmit(&hi2c1, ADDRESS<<1, ADSwrite, 1, 100);
		HAL_Delay(20);
		HAL_I2C_Master_Receive(&hi2c1, ADDRESS<<1, ADSwrite, 2, 100);
		reading = (ADSwrite[0] << 8 | ADSwrite[1]);
		if(reading >= 0x8000 && reading <= 0xffff)
		{
			reading = 0xffff - reading;
		}
		 else if(reading >= 0xffff)
		 {
			 reading = 0;
		 }

		if(i == 0)
		{
			ADS1115_BOARD_SELECT->ADS1115_CH1.data = reading * voltageConv;
		}
		else if(i == 1)
		{
			ADS1115_BOARD_SELECT->ADS1115_CH2.data = reading * voltageConv;
		}
	}

}

int Convert2Modbus(float Data)
{
	int ConvertData = 0;

	ConvertData = ((Data+5)*65535)/10;

	return ConvertData;
}
