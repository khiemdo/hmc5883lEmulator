#include "TestcIMUOdometry.h"
#include "cIMUOdometry.h"
#include "I2CSlave1.h"
#include "greatest.h"
#include "cAssert.h"
#include "mbed.h"
#include "main.h"

extern I2C_HandleTypeDef I2CxHandle;
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

	me->outCom = &i2cCom;
	me->outCom->frequency(HMC5883L_I2C_FREQUENCY);
	me->outCom->address(HMC5883L_I2C_ADDRESS);

	me->wheelBase = WHEEL_BASE;

//	me->outCom1 = &I2CxHandle;
//	i2c1Config(me->outCom1);

}
TEST TestI2CPort_cIMUOdometry2() {
	cIMUOdometry* me = &imuOdometry;
	uint8_t data[10] = { 0 };
	HAL_I2C_Master_Transmit(me->outCom1, 0X1E, data, 1, 100);
}
TEST TestI2CPort_cIMUOdometry() {
	cIMUOdometry* me = &imuOdometry;
	int loopNumber = 100;

	while (1) {
		uint8_t msgBuff[30] = { 0 };
		int index = 0;
		int number = 0;
		I2CSlave* i2cPtr = me->outCom;
		int receiveFlag = i2cPtr->receive();
//		uint8_t data[10]={0};
//		i2cPtr->write((char*) data, 1);
		switch (receiveFlag) {
		case I2CSlave::ReadAddressed:
			DEBUG(LOG_TEST, "rqI2CRead\r\n", 0);
			index = 0;
			number = (int) me->XOutput;
			msgBuff[index++] = (uint8_t)(((number & 0xffff) >> 8) & 0xff);
			msgBuff[index++] = (uint8_t)(((number & 0xffff)) & 0xff);
			number = (int) me->ZOutput;
			msgBuff[index++] = (uint8_t)(((number & 0xffff) >> 8) & 0xff);
			msgBuff[index++] = (uint8_t)(((number & 0xffff)) & 0xff);
			number = (int) me->YOutput;
			msgBuff[index++] = (uint8_t)(((number & 0xffff) >> 8) & 0xff);
			msgBuff[index++] = (uint8_t)(((number & 0xffff)) & 0xff);
			i2cPtr->write((char*) msgBuff, index);
			break;
		case I2CSlave::WriteAddressed:
			i2cPtr->read((char*) msgBuff, 2);

//			wait_ms(10);
//			(I2C1->SR1) = I2C1->SR1 &(~(I2C_SR1_STOPF));
//			(I2C1->SR1) = 0;

//			DEBUG(LOG_TEST, "got msg:%d,%d,%d,%d,%d\r\n",
//					msgBuff[0], msgBuff[1], msgBuff[2], msgBuff[3], msgBuff[4], msgBuff[5]);
			break;
		case I2CSlave::NoData:
			(I2C1->SR1) = I2C1->SR1 &(~(I2C_SR1_RXNE));
//			i2cPtr->stop();
			break;
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

}
TEST TestSendXYZ_cIMUOdometry();

SUITE(TestSuitecIMUOdometry)
{
	Setup_TestcIMUOdometry(NULL);
	RUN_TEST(TestI2CPort_cIMUOdometry);
//	RUN_TEST(TestI2CPort_cIMUOdometry2);
//	RUN_TEST(TestSerialPort_cIMUOdometry);
//	RUN_TEST(TestHandleRBTSerial_cIMUOdometry);
//	RUN_TEST(TestConvertCounterToXYZ_cIMUOdometry);
}
