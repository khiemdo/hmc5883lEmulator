#ifndef _cIMUOdometry_h
#define _cIMUOdometry_h
#include "mbed.h"
#include "cAssert.h"
#include "I2CSlave.h"
#include "BufferedSerial.h"
#include "cFrameMsgGetter.h"
#include "deviceConfig.h"

typedef struct _cIMUOdometry_ {
	I2C_HandleTypeDef* outCom1;
	uint8_t* memory;
	int32_t* i2cDataIndex;
	int32_t sizeMemory;
	int32_t* i2cState;

	BufferedSerial* inCom;
	cFrameMsgGetter *frameGetter;
	int8_t registerA;
	int8_t registerB;
	int8_t modeRegister;
	int8_t statusRegister;

	float_t wheelBase;
	float_t theta;
	float_t thetaDegree;
	float_t distance;
	float_t mmLeft;
	float_t mmRight;

	int32_t counter[2];
	float_t XOutput;
	float_t YOutput;
	float_t ZOutput;

} cIMUOdometry;

void cIMUOdometryInit(cIMUOdometry* me);
void cIMUOdometryConfig(cIMUOdometry* me, I2CSlave* outCom,
		BufferedSerial* inCom);
void Loop_cIMUOdometry(cIMUOdometry* me);
void HandleRBTSerial_cIMUOdometry(cIMUOdometry* me);
void HandleMasterMsg_cIMUOdometry(cIMUOdometry* me);
#endif
