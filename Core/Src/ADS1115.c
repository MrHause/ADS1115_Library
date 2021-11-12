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

//static ADS1115_ConfigReg_t hConfigReg;

//extern I2C_HandleTypeDef hi2c1;

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
	bytes[2] |= (config.dataRate << 7) | (config.compareMode << 4) | (config.polarityMode << 3)
					| (config.latchingMode << 2) | (config.queueComparator << 1);

	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 3, 100);
}

int16_t ADS1115_oneShotMeasure(ADS1115_Handle_t *pConfig){
	uint8_t bytes[3] = {0};
	bytes[0] = 0x01;
	bytes[1] |= (1 << 7) | (pConfig->config.channel << 4) | (pConfig->config.pgaConfig << 1)
					| (pConfig->config.operatingMode << 0);
	uint8_t value = (uint8_t)pConfig->config.dataRate;
	bytes[2] |= (pConfig->config.dataRate << 5) | (pConfig->config.compareMode << 4) | (pConfig->config.polarityMode << 3)
					| (pConfig->config.latchingMode << 2) | (pConfig->config.queueComparator << 0);

	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 3, 100);

	return ADS1115_getData(pConfig);
}

int16_t ADS1115_getData(ADS1115_Handle_t *pConfig){
	uint8_t bytes[2] = {0};
	bytes[0] = 0x00;
	HAL_I2C_Master_Transmit(pConfig->hi2c, (pConfig->address << 1), bytes, 1, 100);
	HAL_Delay(20);
	HAL_I2C_Master_Receive(pConfig->hi2c, (pConfig->address << 1), bytes, 2, 100);
	int16_t readValue = ((bytes[0] << 8) | bytes[1]);
	if(readValue < 0)
		readValue = 0;

	return readValue;
}

