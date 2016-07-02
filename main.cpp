#include "main.h"
#include "mbed.h"
#include "greatest.h"
#include "BufferedSerial.h"
#include "deviceConfig.h"
#include "cIMUOdometry.h"
#include "TestcIMUOdometry.h"
#include "math.h"

I2C_HandleTypeDef I2CxHandle;
volatile uint32_t i2cFlag;
uint8_t txI2cBuffer[10];
uint8_t rxI2cBuffer[10];
//I2CSlave i2cCom(I2C1_SDA, I2C1_SCL);
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

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		I2C1_SlaveTxCpltCallback_cIMUOdometry(&imuOdometry);
	}
}
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		I2C1_SlaveRxCpltCallback_cIMUOdometry(&imuOdometry);
	}
}
