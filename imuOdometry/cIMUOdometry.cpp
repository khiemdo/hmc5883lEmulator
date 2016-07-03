#include "cIMUOdometry.h"
#include "cFrameMsgGetter.h"
#include "cRBTRS232ProtocolWrapper.h"
#include "math.h"
#include "generalUtility.h"

void cIMUOdometryInit(cIMUOdometry* me) {
	memset(me, 0, sizeof(cIMUOdometry));
}
void cIMUOdometryConfig(cIMUOdometry* me, I2CSlave* outCom,
		BufferedSerial* inCom) {
	me->inCom = inCom;
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

void HandleRBTSerial_cIMUOdometry(cIMUOdometry* me) {
	int8_t ch = 0;
	if (me->inCom->readable()) {
		ch = me->inCom->getc();
	}
	int32_t msgLength = FrameMsgGetter2(me->frameGetter, ch);
	if (msgLength != 0) {
		int ret = WaitAck_cRBTRS232ProtocolWrapper1(
				me->frameGetter->outputFrame, 0,
				GET_CHANNEL_COUNTER_RBTProtocol, me->counter);
		if (ret == 2) {
			DEBUG(LOG_INFO, "counter:%d;%d\r\n", me->counter[0],
					me->counter[1]);
		}
	}
}
void UpdateOdometry_cIMUOdometry(cIMUOdometry* me) {
	me->mmLeft = ConvertCounterToArcLength(me->counter[0], WHEEL_CIRCUMFERENCE,
			PPR_DRUM_ENCODER);
	me->mmRight = ConvertCounterToArcLength(me->counter[1], WHEEL_CIRCUMFERENCE,
			PPR_DRUM_ENCODER);
	ConvertCounterToXYZ_cIMUOdometry(me, me->mmLeft, me->mmRight, WHEEL_BASE);

	uint8_t* buffPtr = me->memory + HMC5883L_X_MSB;

	int16_t number16 = (int16_t)(me->XOutput / 32);
	int16_t swapNumber16 = SWAP_UINT16T(number16);
	memcpy(buffPtr, &swapNumber16, sizeof(int16_t));
	buffPtr += 2;

	number16 = 0;
	memcpy(buffPtr, &number16, sizeof(int16_t));
	buffPtr += 2;

	number16 = (int16_t)(me->YOutput / 32);
	swapNumber16 = SWAP_UINT16T(number16);
	memcpy(buffPtr, &swapNumber16, sizeof(int16_t));
}

void Loop_cIMUOdometry(cIMUOdometry* me) {
	HandleRBTSerial_cIMUOdometry(me);
	UpdateOdometry_cIMUOdometry(me);
}

