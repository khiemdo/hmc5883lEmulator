#include "main.h"
#include "mbed.h"
#include "greatest.h"
#include "BufferedSerial.h"
#include "deviceConfig.h"
#include "cIMUOdometry.h"
#include "TestcIMUOdometry.h"
#include "math.h"

I2C_HandleTypeDef I2CxHandle;
int32_t i2cDataIndex;
uint8_t memory[HMC5883L_TOTAL_REGISTER];
int32_t i2cState;
BufferedSerial rbtSerial(SERIAL_RBT_TX, SERIAL_RBT_RX, 512);
BufferedSerial debugSerial(USBTX, USBRX, 2048, 10, 0);
cIMUOdometry imuOdometry;
int8_t msgString_IMUOdometry[64];
int8_t outputFrame_IMUOdometry[64];
cFrameMsgGetter frameGetter_IMUOdometry;

SUITE_EXTERN(TestSuitecIMUOdometry);
extern "C" {
GREATEST_MAIN_DEFS()
;
}

int main(int argc, char **argv) {
	debugSerial.baud(115200);
	rbtSerial.baud(115200);

	GREATEST_MAIN_BEGIN();
	greatest_info.verbosity++;
	RUN_SUITE(TestSuitecIMUOdometry);
	GREATEST_MAIN_END();
}
