/*
 * ADS1115.c
 *
 *  Created on: 9 lis 2021
 *      Author: Filip Rak
 */

#include "ADS1115.h"

struct ADS1115_Config_Tag{
	I2C_HandleTypeDef 	*hi2c;
	uint16_t 			address;
	ADS1115_Config_t 	config;
};

ADS1115_Handle_t* ADS1115_init(I2C_HandleTypeDef *hi2c, uint16_t Addr, ADS1115_Config_t config){
	ADS1115_Handle_t *pConfig = malloc(sizeof(ADS1115_Handle_t));
	pConfig->hi2c = hi2c;
	pConfig->address = Addr;
	pConfig->config = config;
	return pConfig;
}

void ADS1115_deinit(ADS1115_Handle_t* pConfig){
	free(pConfig);
}

void ADS1115_updateConfig(ADS1115_Handle_t *pConfig, ADS1115_Config_t config){
	pConfig->config = config;

	uint8_t bytes[3] = {0};
	bytes[0] = 0x01;
	bytes[1] |= (config.channel << 6) | (config.pgaConfig << 3)
					| (config.operatingMode << 0);
	bytes[2] |= (config.dataRate << 5) | (config.compareMode << 4) | (config.polarityMode << 3)
					| (config.latchingMode << 2) | (config.queueComparator << 1);

	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 3, 100);
}

void ADS1115_updateI2Chandler(ADS1115_Handle_t *pConfig, I2C_HandleTypeDef *hi2c){
	pConfig->hi2c = hi2c;
}

void ADS1115_updateAddress(ADS1115_Handle_t *pConfig, uint16_t address){
	pConfig->address = address;
}

int16_t ADS1115_oneShotMeasure(ADS1115_Handle_t *pConfig){
	uint8_t bytes[3] = {0};
	bytes[0] = 0x01;
	bytes[1] |= (1 << 7) | (pConfig->config.channel << 4) | (pConfig->config.pgaConfig << 1)
					| (pConfig->config.operatingMode << 0);

	bytes[2] |= (pConfig->config.dataRate << 5) | (pConfig->config.compareMode << 4) | (pConfig->config.polarityMode << 3)
					| (pConfig->config.latchingMode << 2) | (pConfig->config.queueComparator << 0);

	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 3, 100);

	return ADS1115_getData(pConfig);
}

int16_t ADS1115_getData(ADS1115_Handle_t *pConfig){
	uint8_t bytes[2] = {0};
	bytes[0] = 0x00;
	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 1, 50);

	if(HAL_I2C_Master_Receive(pConfig->hi2c, (pConfig->address << 1), bytes, 2, 50) != HAL_OK)
		return 0;

	int16_t readValue = ((bytes[0] << 8) | bytes[1]);
	if(readValue < 0)
		readValue = 0;

	return readValue;
}


void ADS1115_setThresholds(ADS1115_Handle_t *pConfig, int16_t lowValue, int16_t highValue){
	uint8_t ADSWrite[3] = { 0 };

	//hi threshold reg
	ADSWrite[0] = 0x03;
	ADSWrite[1] = (uint8_t)((highValue & 0xFF00) >> 8);
	ADSWrite[2] = (uint8_t)(highValue & 0x00FF);
	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), ADSWrite, 3, 100);

	//lo threshold reg
	ADSWrite[0] = 0x02;
	ADSWrite[1] = (uint8_t)((lowValue & 0xFF00) >> 8);
	ADSWrite[2] = (uint8_t)(lowValue & 0x00FF);
	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), ADSWrite, 3, 100);
}

void ADS1115_startContinousMode(ADS1115_Handle_t* pConfig){
	uint8_t bytes[3] = {0};
	bytes[0] = 0x01;
	bytes[1] |= (pConfig->config.channel << 6) | (pConfig->config.pgaConfig << 3)
					| (0 << 0);
	bytes[2] |= (pConfig->config.dataRate << 5) | (pConfig->config.compareMode << 4) | (pConfig->config.polarityMode << 3)
					| (pConfig->config.latchingMode << 2) | (pConfig->config.queueComparator << 1);

	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 3, 100);
}

void ADS1115_stopContinousMode(ADS1115_Handle_t* pConfig){
	uint8_t bytes[3] = {0};
	bytes[0] = 0x01;
	bytes[1] |= (pConfig->config.channel << 6) | (pConfig->config.pgaConfig << 3)
					| (1 << 0);
	bytes[2] |= (pConfig->config.dataRate << 5) | (pConfig->config.compareMode << 4) | (pConfig->config.polarityMode << 3)
					| (pConfig->config.latchingMode << 2) | (pConfig->config.queueComparator << 1);

	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 3, 100);
}

void ADS1115_flushData(ADS1115_Handle_t* pConfig){
	ADS1115_getData(pConfig);
}

void ADS1115_setConversionReadyPin(ADS1115_Handle_t* pConfig){
	ADS1115_setThresholds(pConfig, 0x0000, 0xFFFF);
}


