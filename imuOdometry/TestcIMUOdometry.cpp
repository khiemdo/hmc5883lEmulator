#include "TestcIMUOdometry.h"
#include "cIMUOdometry.h"
#include "I2CSlave1.h"
#include "greatest.h"
#include "cAssert.h"
#include "mbed.h"
#include "main.h"

extern I2C_HandleTypeDef I2CxHandle;
extern volatile int32_t i2cFlag;
extern uint8_t txI2cBuffer[10];
extern uint8_t rxI2cBuffer[10];
extern I2CSlave i2cCom;
extern BufferedSerial rbtSerial;
extern BufferedSerial debugSerial;
extern cIMUOdometry imuOdometry;
extern int8_t msgString_IMUOdometry[64];
extern int8_t outputFrame_IMUOdometry[64];
extern cFrameMsgGetter frameGetter_IMUOdometry;

void Setup_TestcIMUOdometry(void* data) {
	cIMUOdometry* me = &imuOdometry;
	FrameMsgGetterInitialize(&frameGetter_IMUOdometry, 64,
			msgString_IMUOdometry, outputFrame_IMUOdometry);
	me->frameGetter = &frameGetter_IMUOdometry;
	me->inCom = &rbtSerial;
	me->inCom->baud(115200);

//	me->outCom = &i2cCom;
//	me->outCom->frequency(HMC5883L_I2C_FREQUENCY);
//	me->outCom->address(HMC5883L_I2C_ADDRESS);

	me->wheelBase = WHEEL_BASE;

	me->outCom1 = &I2CxHandle;
	i2c1Config(me->outCom1);

}

TEST TestI2CPort_cIMUOdometry() {
	cIMUOdometry* me = &imuOdometry;
	int loopNumber = 100;
	uint8_t data[10];
	while (1) {
		if (__HAL_I2C_GET_FLAG(me->outCom1,I2C_FLAG_ADDR)) {
			DEBUG(LOG_TEST, "got msg\r\n", 0);
		}
		HAL_StatusTypeDef ret = HAL_I2C_Slave_Receive_IT(me->outCom1, data, 2);
		if (ret == HAL_OK) {
			DEBUG(LOG_TEST, "got msg\r\n", 0);
		}
	}

}
TEST TestSerialPort_cIMUOdometry() {
	int test = 0;
	printf("helloworld\r\n");
	cIMUOdometry* me = &imuOdometry;
	me->inCom->printf("?C\r");
	wait_us(100);
	while (me->inCom->readable()) {
		char ch = me->inCom->getc();
		printf("%c", ch);
		test = 1;
	}
	if (test == 1)
		PASS();
	else if (test == 0)
		FAIL();
	else
		FAIL();
}
TEST TestHandleRBTSerial_cIMUOdometry() {
	cIMUOdometry* me = &imuOdometry;
	int32_t numberOfLoops = 0;
	uint32_t lastTime = HAL_GetTick();

	while (1) {
		uint32_t currentTime = HAL_GetTick();
		int deltaTime = currentTime - lastTime;
		if (deltaTime > 50) {
			me->inCom->printf("?C\r");
			if (numberOfLoops++ > 10)
				break;
			lastTime = currentTime;
		}
		HandleRBTSerial_cIMUOdometry(me);
	}
	PASS();
}

void DebugLog_cIMUOdometry(cIMUOdometry* me) {
	static uint32_t lastTime = 0;
	uint32_t currentTime = HAL_GetTick();
	uint32_t deltaTime = currentTime - lastTime;
	if (deltaTime > 1000) {
		DEBUG(LOG_DEBUG, "c=%d:%d\r\n", me->counter[0], me->counter[1]);
		char myBuff[100];
		sprintf(myBuff, "o=%d,%d,%d,%d\r\n", (int32_t) me->thetaDegree,
				(int32_t) me->distance, (int32_t) (me->XOutput),
				(int32_t) (me->YOutput));
		DEBUG(LOG_DEBUG, myBuff, 0);
		lastTime = currentTime;
	}
}
TEST TestConvertCounterToXYZ_cIMUOdometry() {
	cIMUOdometry* me = &imuOdometry;
	int32_t numberOfLoops = 0;
	uint32_t lastTime = HAL_GetTick();

	while (1) {
		uint32_t currentTime = HAL_GetTick();
		int deltaTime = currentTime - lastTime;
		if (deltaTime > 1000) {
			me->inCom->printf("?C\r");
			if (numberOfLoops++ > 100)
				break;
			lastTime = currentTime;
		}
		Loop_cIMUOdometry(me);
		DebugLog_cIMUOdometry(me);
	}
	PASS();
}
TEST TestHandleMasterMsg_cIMUOdometry() {
	cIMUOdometry* me = &imuOdometry;
	HAL_StatusTypeDef ret = HAL_ERROR;
	int loopNumber = 100;
//	txI2cBuffer[0] = 0x48;
//	HAL_StatusTypeDef ret = HAL_I2C_Slave_Transmit_IT(me->outCom1, txI2cBuffer,
//			1);
	while (1) {
		if (HAL_I2C_GetState(&I2CxHandle) == HAL_I2C_STATE_READY) {
			ret = HAL_I2C_Slave_Receive_IT(me->outCom1,
					rxI2cBuffer, 1);
			if (ret = HAL_BUSY) {
				//request read from master
				if (__HAL_I2C_GET_FLAG(me->outCom1,I2C_FLAG_TRA) == 1) {
					switch (rxI2cBuffer[0]) {
					case HMC5883L_IDENT_A:
						if (HAL_I2C_GetState(me->outCom1)
								== HAL_I2C_STATE_BUSY_TX) { //got read request
							txI2cBuffer[0] = 0x48;
							ret = HAL_I2C_Slave_Transmit_IT(me->outCom1,
									txI2cBuffer, 1);
						} //todo: else write to config
					case HMC5883L_IDENT_B:
						if (HAL_I2C_GetState(me->outCom1)
								== HAL_I2C_STATE_BUSY_TX) { //got read request
							txI2cBuffer[0] = 0x34;
							ret = HAL_I2C_Slave_Transmit_IT(me->outCom1,
									txI2cBuffer, 1);
						}
						break;
					case HMC5883L_IDENT_C:
						if (HAL_I2C_GetState(me->outCom1)
								== HAL_I2C_STATE_BUSY_TX) { //got read request
							txI2cBuffer[0] = 0x33;
							ret = HAL_I2C_Slave_Transmit_IT(me->outCom1,
									txI2cBuffer, 1);
						}
						break;
					}
				}
			} else if (ret == HAL_OK) { //request write
				if (__HAL_I2C_GET_FLAG(me->outCom1,I2C_FLAG_ADDR) == 1) {
					DEBUG(LOG_TEST, "rqWriteAddr:%d\r\n", rxI2cBuffer[0]);
				}
			}

			break;
		}
	}
}
TEST TestSendXYZ_cIMUOdometry();

SUITE(TestSuitecIMUOdometry)
{
	Setup_TestcIMUOdometry(NULL);
//	RUN_TEST(TestI2CPort_cIMUOdometry);
//	RUN_TEST(TestSerialPort_cIMUOdometry);
//	RUN_TEST(TestHandleRBTSerial_cIMUOdometry);
//	RUN_TEST(TestConvertCounterToXYZ_cIMUOdometry);
	RUN_TEST(TestHandleMasterMsg_cIMUOdometry);
}
