#include "TestcIMUOdometry.h"
#include "cIMUOdometry.h"
#include "I2CSlave1.h"
#include "greatest.h"
#include "cAssert.h"
#include "mbed.h"
#include "main.h"

extern I2C_HandleTypeDef I2CxHandle;
extern int32_t i2cDataIndex;
extern uint8_t memory[HMC5883L_TOTAL_REGISTER];
extern int32_t i2cState;

extern uint8_t memory[HMC5883L_TOTAL_REGISTER];

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

	me->wheelBase = WHEEL_BASE;

	me->outCom1 = &I2CxHandle;
	i2c1Config(me->outCom1);

	memset(memory, 0, HMC5883L_TOTAL_REGISTER);
	memory[10] = 0x48;
	memory[11] = 0x34;
	memory[12] = 0x33;
	i2cDataIndex = 0;
	me->memory = memory;
	me->i2cDataIndex = &i2cDataIndex;
	me->i2cState = &i2cState;
	me->sizeMemory = HMC5883L_TOTAL_REGISTER;
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
				(int32_t) me->distance, (int32_t)(me->XOutput/32),
				(int32_t)(me->YOutput/32));
		DEBUG(LOG_DEBUG, myBuff, 0);
//		DEBUG(LOG_DEBUG, "index=%d\r\n", i2cDataIndex);
		sprintf(myBuff, "m=%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,\r\n", (int32_t) memory[0],
				(int32_t) memory[1], (int32_t) memory[2], (int32_t) memory[3],
				(int32_t) memory[4], (int32_t) memory[5], (int32_t) memory[6],
				(int32_t) memory[7], (int32_t) memory[8], (int32_t) memory[9],
				(int32_t) memory[10], (int32_t) memory[11],
				(int32_t) memory[12]);
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
		if (deltaTime > 50) {
			me->inCom->printf("?C\r");
			lastTime = currentTime;
		}
		Loop_cIMUOdometry(me);
		DebugLog_cIMUOdometry(me);
	}
	PASS();
}
TEST TestHandleMasterMsg_cIMUOdometry();
TEST TestSendXYZ_cIMUOdometry();

SUITE(TestSuitecIMUOdometry)
{
	Setup_TestcIMUOdometry(NULL);
//	RUN_TEST(TestI2CPort_cIMUOdometry);
//	RUN_TEST(TestSerialPort_cIMUOdometry);
//	RUN_TEST(TestHandleRBTSerial_cIMUOdometry);
	RUN_TEST(TestConvertCounterToXYZ_cIMUOdometry);
}
