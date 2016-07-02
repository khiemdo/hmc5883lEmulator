#include "stm32f4xx.h"
#include "deviceConfig.h"

extern I2C_HandleTypeDef I2CxHandle;
extern volatile int32_t i2cFlag;
void I2C1_EV_IRQHandler(void) {
//	if (__HAL_I2C_GET_FLAG(&I2CxHandle,I2C_FLAG_ADDR)
//			&& __HAL_I2C_GET_FLAG(&I2CxHandle,I2C_FLAG_RXNE)) {
//		i2cFlag = 1; //read same addrez
//	} else if (__HAL_I2C_GET_FLAG(&I2CxHandle,I2C_FLAG_RXNE)) {
//		i2cFlag = 2; //general read
//	} else {
//		i2cFlag = 3; //in transmission
//	}

	HAL_I2C_EV_IRQHandler(&I2CxHandle);
}
void I2Cx_ER_IRQHandler(void) {
	HAL_I2C_ER_IRQHandler(&I2CxHandle);
}
