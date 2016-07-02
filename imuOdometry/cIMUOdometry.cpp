#include "cIMUOdometry.h"
#include "cFrameMsgGetter.h"
#include "cRBTRS232ProtocolWrapper.h"
#include "math.h"

void cIMUOdometryInit(cIMUOdometry* me) {
	memset(me, 0, sizeof(cIMUOdometry));
}
void cIMUOdometryConfig(cIMUOdometry* me, I2CSlave* outCom,
		BufferedSerial* inCom) {
	me->inCom = inCom;
//	me->outCom = outCom;
}

void SetMode_cIMUOdometry(cIMUOdometry* me);

float_t GetAngle_cIMUOdometry(float_t mmLeft, float_t mmRight,
		float_t mmWheelBase) {
	return (mmLeft - mmRight) / mmWheelBase;
}
float_t GetDistance_cIMUOdometry(float_t mmLeft, float_t mmRight,
		float_t mmWheelBase) {
	return (mmLeft + mmRight) / 2;
}
float_t ConvertCounterToArcLength(float_t counter, float_t wheelCircumference,
		float_t ppr) {
//	float_t intPart = 0;
//	float_t fractPart = 0;
//	fractPart = modff(counter * wheelCircumference / ppr, &intPart);
//	return fractPart;
	return counter * wheelCircumference * 1.0 / ppr;
}
void ConvertCounterToXYZ_cIMUOdometry(cIMUOdometry* me, float_t mmLeft,
		float_t mmRight, float_t mmWheelBase) {
	me->theta = (mmLeft - mmRight) / mmWheelBase;
	me->thetaDegree = me->theta / M_PI * 180;
	me->distance = (mmLeft + mmRight) / 2;
	me->XOutput = -1000 * cosf(me->theta);
	me->YOutput = 1000 * sinf(me->theta);
}

void SendXYZ_cIMUOdometry(cIMUOdometry* me);
void HandleRBTSerial_cIMUOdometry(cIMUOdometry* me) {
	int8_t ch = 0;
	if (me->inCom->readable()) {
ch	= me->inCom->getc();
}
	int32_t msgLength = FrameMsgGetter2(me->frameGetter, ch);
	if (msgLength != 0) {
		int ret = WaitAck_cRBTRS232ProtocolWrapper1(
				me->frameGetter->outputFrame, 0,
				GET_CHANNEL_COUNTER_RBTProtocol, me->counter);
		if (ret == 2) {
			DEBUG(LOG_INFO, "counter:%d;%d\r\n",
					me->counter[0], me->counter[1]);
		}
	}
}
void UpdateOdometry_cIMUOdometry(cIMUOdometry* me) {
	me->mmLeft = ConvertCounterToArcLength(me->counter[0], WHEEL_CIRCUMFERENCE,
			PPR_DRUM_ENCODER);
	me->mmRight = ConvertCounterToArcLength(me->counter[1], WHEEL_CIRCUMFERENCE,
			PPR_DRUM_ENCODER);
	ConvertCounterToXYZ_cIMUOdometry(me, me->mmLeft, me->mmRight, WHEEL_BASE);
}
void HandleMasterMsg_cIMUOdometry(cIMUOdometry* me) {
	uint8_t msgBuff[30] = { 0 };
	int index = 0;
	int number = 0;
	I2CSlave* i2cPtr = 0; //todo
	int receiveFlag = i2cPtr->receive();
	switch (receiveFlag) {
	case I2CSlave::ReadAddressed:
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
		i2cPtr->read((char*) msgBuff, 30);
		DEBUG(LOG_TEST, "got msg:%d,%d,%d,%d,%d\r\n",
				msgBuff[0], msgBuff[1], msgBuff[2], msgBuff[3], msgBuff[4], msgBuff[5]);
		break;
	}
}

void Loop_cIMUOdometry(cIMUOdometry* me) {
	HandleRBTSerial_cIMUOdometry(me);
	UpdateOdometry_cIMUOdometry(me);
	HandleMasterMsg_cIMUOdometry(me);
}
#ifdef __cplusplus
extern "C" {
#endif
void I2C1_SlaveTxCpltCallback_cIMUOdometry(cIMUOdometry* me) {
	me->transmitCmplt = 1;
}
void I2C1_SlaveRxCpltCallback_cIMUOdometry(cIMUOdometry* me) {
	me->rxCmplt = 1;
}
#ifdef __cplusplus
}
#endif
