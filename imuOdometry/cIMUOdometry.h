#ifndef _cIMUOdometry_h
#define _cIMUOdometry_h
#include "mbed.h"
#include "cAssert.h"
#include "I2CSlave.h"
#include "BufferedSerial.h"
#include "cFrameMsgGetter.h"

#define WHEEL_BASE 			(630)
#define WHEEL_CIRCUMFERENCE	(1320)
#define PPR_DRUM_ENCODER	(8192)

#define HMC5883L_I2C_ADDRESS 		(0x1E) //7-bit address. 0x3C write, 0x3D read.
#define HMC5883L_I2C_FREQUENCY		(100000)
#define HMC5883L_I2C_WRITE   (0x3C)
#define HMC5883L_I2C_READ    (0x3D)

#define HMC5883L_CONFIG_A     0x00
#define HMC5883L_CONFIG_B     0x01
#define HMC5883L_MODE         0x02
#define HMC5883L_X_MSB        0x03
#define HMC5883L_X_LSB        0x04
#define HMC5883L_Z_MSB        0x05
#define HMC5883L_Z_LSB        0x06
#define HMC5883L_Y_MSB        0x07
#define HMC5883L_Y_LSB        0x08
#define HMC5883L_STATUS       0x09
#define HMC5883L_IDENT_A      0x0A
#define HMC5883L_IDENT_B      0x0B
#define HMC5883L_IDENT_C      0x0C

typedef struct _cIMUOdometry_ {
	I2CSlave * outCom;
	I2C_HandleTypeDef* outCom1;
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
#endif
