#include "TestcRBTRS232ProtocolWrapper.h"
#include "greatest.h"
#include "cAssert.h"
#include "cRBTRS232ProtocolWrapper.h"
#include "mbed.h"
#include "deviceConfig.h"

void Setup_TestcRBTRS232ProtocolWrapper(void* data) {
	BufferedSerial* pcPtr = (BufferedSerial*) greatest_info.setup_udata;
	pcPtr->baud(115200);
}
TEST SendMsg_TestcRBTRS232ProtocolWrapper() {
	SKIP();
}
TEST WaitAck_TestcRBTRS232ProtocolWrapper() {
	BufferedSerial* pcPtr = (BufferedSerial*) greatest_info.setup_udata;
	DEBUG(LOG_TEST, "input:'+'\r\n", 0);
	WaitAck_cRBTRS232ProtocolWrapper(pcPtr, 0, 0, 0);
	DEBUG(LOG_TEST, "input:'-'\r\n", 0);
	WaitAck_cRBTRS232ProtocolWrapper(pcPtr, 0, 0, 0);
	DEBUG(LOG_TEST, "input:'a=100'\r\n", 0);
	int32_t para1 = 0;
	WaitAck_cRBTRS232ProtocolWrapper(pcPtr, GET_CHANNEL_COUNTER_RBTProtocol,
			&para1, NULL);
	DEBUG(LOG_TEST, "input:'c=100'\r\n", 0);
	WaitAck_cRBTRS232ProtocolWrapper(pcPtr, GET_CHANNEL_COUNTER_RBTProtocol,
			&para1, NULL);
	SKIP();
}

TEST SendSerialUsingPutc_TestcRBTRS232ProtocolWrapper() {
	BufferedSerial* pcPtr = (BufferedSerial*) greatest_info.setup_udata;
	pcPtr->putc(33);
	wait_ms(100);
	pcPtr->putc(69);
	wait_ms(100);
	pcPtr->putc(88);
	wait_ms(100);
	pcPtr->putc(13);
	wait_ms(100);
	while (1) {
		if (pcPtr->readable()) {
char ch = pcPtr->getc();
						DEBUG(LOG_TEST, "%c", ch);
			if (ch == '\r') {
				break;
			}
		}
	}
	PASS();
}
SUITE(TestSuitecRBTRS232ProtocolWrapper)
{
	BufferedSerial pc_TestcRBTRS232ProtocolWrapper(SERIAL_RBT_TX,
			SERIAL_RBT_RX);
	SET_SETUP(Setup_TestcRBTRS232ProtocolWrapper,
			&pc_TestcRBTRS232ProtocolWrapper);
//	RUN_TEST(WaitAck_TestcRBTRS232ProtocolWrapper);
	RUN_TEST(SendSerialUsingPutc_TestcRBTRS232ProtocolWrapper);
}
