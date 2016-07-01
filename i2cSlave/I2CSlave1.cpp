#include "I2CSlave1.h"
#include "main.h"
#include "cAssert.h"
#include "deviceConfig.h"

void i2c1Config(I2C_HandleTypeDef* I2CxHandle) {
	GPIO_InitTypeDef GPIO_InitStruct;
	I2Cx_SCL_GPIO_CLK_ENABLE();
	I2Cx_SDA_GPIO_CLK_ENABLE();
	I2Cx_CLK_ENABLE();

	/* I2C TX GPIO pin configuration  */
	GPIO_InitStruct.Pin = I2Cx_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = I2Cx_SCL_AF;

	HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

	/* I2C RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
	GPIO_InitStruct.Alternate = I2Cx_SDA_AF;

	HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

	I2CxHandle->Instance = I2Cx;
	I2CxHandle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2CxHandle->Init.ClockSpeed = 400000;
	I2CxHandle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2CxHandle->Init.DutyCycle = I2C_DUTYCYCLE_16_9;
	I2CxHandle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2CxHandle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	I2CxHandle->Init.OwnAddress1 = I2C_ADDRESS;
	I2CxHandle->Init.OwnAddress2 = 0;
	REQUIRE(HAL_I2C_Init(I2CxHandle) != HAL_OK);

	__HAL_I2C_ENABLE_IT(I2CxHandle, I2C_IT_BUF);
	__HAL_I2C_ENABLE_IT(I2CxHandle, I2C_IT_EVT);
	__HAL_I2C_ENABLE_IT(I2CxHandle, I2C_IT_ERR);

	/*##-4- Configure the NVIC for I2C #########################################*/
	/* NVIC for I2C1 */
	HAL_NVIC_SetPriority(I2Cx_ER_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(I2Cx_ER_IRQn);
	HAL_NVIC_SetPriority(I2Cx_EV_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(I2Cx_EV_IRQn);
}
