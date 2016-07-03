#include "stm32f4xx.h"
#include "deviceConfig.h"
#include "deviceConfig.h"
#include "cAssert.h"
typedef enum {
	READY_I2C,
	MATCH_OWN_ADDRESS_I2C,
	READ_WRITE_I2C,
	SLAVE_WRITE_I2C,
	SLAVE_READ_I2C,
	NUMBER_OF_STATES_I2C
} I2cStateEnum;

extern I2C_HandleTypeDef I2CxHandle;
extern int32_t i2cDataIndex;
extern uint8_t memory[HMC5883L_TOTAL_REGISTER];
extern int32_t i2cState;
extern int32_t matchOwnReadAddr;
extern int32_t matchOwnWriteAddr;
int32_t HandlePointerBoundary(int32_t index, int32_t min, int32_t max) {
	if (index > max) {
		index -= (max - min + 1);
	}
	if (index < min) {
		index += (max - min + 1);
	}
	return index;
}
int32_t Limit(int32_t index, int32_t min, int32_t max) {
	if (index > max) {
		index = max;
	}
	if (index < min) {
		index = min;
	}
	return index;
}
int32_t debug1 = 0;
int32_t debug2 = 0;
int32_t debug3 = 0;
int32_t debug4 = 0;
void I2CEVIRQ(void) {
	I2C_HandleTypeDef * me = &I2CxHandle;
	int32_t tempIndex = 0;
	if (!__HAL_I2C_GET_FLAG(me,I2C_FLAG_TRA)) {
		switch (i2cState) {
		case READY_I2C:
			if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_ADDR)) { //match address
				matchOwnReadAddr = 1;
				__HAL_I2C_CLEAR_ADDRFLAG(me);
			}
			if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_RXNE) && matchOwnReadAddr) {
				tempIndex = I2C1->DR;
				REQUIRE(
						tempIndex >= 0 && tempIndex <= HMC5883L_TOTAL_REGISTER - 1);
				i2cDataIndex = tempIndex;
				i2cState = SLAVE_READ_I2C;
				matchOwnReadAddr = 0;
			}
			if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_STOPF)) { //end of transfer
				i2cState = READY_I2C;
				__HAL_I2C_CLEAR_STOPFLAG(me);
			}
			break;
		case SLAVE_READ_I2C:
			if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_RXNE)) {
				REQUIRE((i2cDataIndex >= 0 && i2cDataIndex <= HMC5883L_TOTAL_REGISTER - 1));
				memory[i2cDataIndex] = I2C1->DR;
				i2cDataIndex = HandlePointerBoundary(++i2cDataIndex, 0,
						HMC5883L_TOTAL_REGISTER);
			}
			if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_STOPF)) { //end of transfer
				i2cState = READY_I2C;
				__HAL_I2C_CLEAR_STOPFLAG(me);
			}
			break;
		}
	} else {
		if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_ADDR)) { //match address
			matchOwnWriteAddr = 1;
			__HAL_I2C_CLEAR_ADDRFLAG(me);
			//MUST clear bf read txe
		}
		if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_TXE)) {
			matchOwnWriteAddr = 0;
			I2C1->DR = memory[i2cDataIndex];
			i2cDataIndex = HandlePointerBoundary(++i2cDataIndex, 0,
					HMC5883L_TOTAL_REGISTER - 1);
			i2cState = READY_I2C;
		}
	}

}
void I2C1_EV_IRQHandler(void) {
	I2CEVIRQ();
}
void I2CErr_IRQ() {
	I2C_HandleTypeDef * me = &I2CxHandle;
	if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_AF)) {
		i2cDataIndex = HandlePointerBoundary(--i2cDataIndex, 0,
				HMC5883L_TOTAL_REGISTER - 1);
		__HAL_I2C_CLEAR_FLAG(me, I2C_FLAG_AF);
	} else if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_BERR)) {
		__HAL_I2C_CLEAR_FLAG(me, I2C_FLAG_BERR);
	} else if (__HAL_I2C_GET_FLAG(me,I2C_FLAG_BUSY)) {
		__HAL_I2C_CLEAR_FLAG(me, I2C_FLAG_BUSY);
	}
}
void I2Cx_ER_IRQHandler(void) {
	I2CErr_IRQ();
}
