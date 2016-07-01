#include "main.h"
#include "mbed.h"
#include "greatest.h"
#include "BufferedSerial.h"
#include "deviceConfig.h"
#include "cIMUOdometry.h"
#include "TestcIMUOdometry.h"
#include "math.h"
#include "I2CSlave.h"

I2C_HandleTypeDef I2CxHandle;
I2C i2cMaster;
I2CSlave i2cCom(I2C1_SDA, I2C1_SCL);
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

//I2C i2c(I2C1_SDA, I2C1_SCL);
//
//const int addr = 0x10;
//
//int main() {
//    char cmd[2];
//    i2c.frequency(400000);
//    while (1) {
//        cmd[0] = 0x01;
//        cmd[1] = 0xee;
//        i2c.write(addr, cmd, 2);
//
//        wait(0.5);
//
//        cmd[0] = 0xaa;
//        i2c.write(addr, cmd, 1);
//        i2c.read(addr, cmd, 2);
//
//        float tmp = (float((cmd[0]<<8)|cmd[1]) / 256.0);
//        printf("Temp = %.2f\n", tmp);
//    }
//}
