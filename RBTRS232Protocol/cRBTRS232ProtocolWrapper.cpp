#include "cRBTRS232ProtocolWrapper.h"
#include "cAssert.h"
#include "mbed.h"
#include "BufferedSerial.h"
#include "string.h"
#include <ctype.h>
#include "generalUtility.h"

const char RBTProtocolEncode[][2][7] = { { "?C", '1' }, { "!C", '1' }, { "?S",
		'1' }, { "?SR", '2' }, { "!S", '2' }, { "!EX", '1' }, { "!MG", '0' }, {
		"?G", '1' }, { "!G", '2' }, { "~MMOD", '1' }, { "~ACTR", '1' }, {
		"~ADB", '1' }, { "~AINA", '1' }, { "~ALIN", '1' }, { "~AMAX", '1' }, {
		"~AMAXA", '1' }, { "~AMIN", '1' }, { "~AMINA", '1' }, { "~AMOD", '1' },
		{ "~APOL", '1' }, { "~PCTR", '1' }, { "~PDB", '1' }, { "~PINA", '1' }, {
				"~PLIN", '1' }, { "~PMAX", '1' }, { "~PMAXA", '1' }, { "~PMIN",
				'1' }, { "~PMINA", '1' }, { "~PMOD", '1' }, { "~PPOL", '1' }, {
				"^MMOD", '2' }, { "^ACTR", '2' }, { "^ADB", '2' }, { "^AINA",
				'2' }, { "^ALIN", '2' }, { "^AMAX", '2' }, { "^AMAXA", '2' }, {
				"^AMIN", '2' }, { "^AMINA", '2' }, { "^AMOD", '2' }, { "^APOL",
				'2' }, { "^PCTR", '2' }, { "^PDB", '2' }, { "^PINA", '2' }, {
				"^PLIN", '2' }, { "^PMAX", '2' }, { "^PMAXA", '2' }, { "^PMIN",
				'2' }, { "^PMINA", '2' }, { "^PMOD", '2' }, { "^PPOL", '2' }, {
				"~CPRI", '2' }, { "^CPRI", '2' }, };

int32_t SendMsg_cRBTRS232ProtocolWrapper(BufferedSerial* port,
		int32_t protocolIndex, int32_t para1, int32_t para2) {
	if (!port->writeable()) {
		return -1;
	}
	switch (RBTProtocolEncode[protocolIndex][1][0]) {
	case '1':
		port->printf("%s %d\r", RBTProtocolEncode[protocolIndex][0], para1);
		break;
	case '2':
		port->printf("%s %d %d\r", RBTProtocolEncode[protocolIndex][0], para1,
				para2);
		break;
	default:
		port->printf("%s\r", RBTProtocolEncode[protocolIndex][0]);
		break;
	}
	return 0;
}
int32_t HandleRBTMsg_cRBTRS232ProtocolWrapper(BufferedSerial* port,
		int32_t protocolIndex, int32_t* para1, int8_t * chBuff) {
	int32_t ret = 0; //0=ack,-1=nack
	switch (chBuff[0]) {
	case '+':
		ret = 0;
		DEBUG(LOG_TEST, "ACK\r\n", chBuff);
		break;
	case '-':
		ret = -1;
		DEBUG(LOG_TEST, "NACK\r\n", chBuff);
		break;
	default:
		//reply msg example: C=10
		ret = 0;
		DEBUG(LOG_TEST, "got:%s\r\n", chBuff);
		int8_t cmdName[7];
		memcpy(cmdName, &RBTProtocolEncode[protocolIndex][0][1],
				strlen(&RBTProtocolEncode[protocolIndex][0][1]) + 1);
		int8_t msgDup[20];
		memcpy((char*) msgDup, (char*) chBuff, strlen((char*) chBuff) + 1); //to protect origin string
		int8_t * cmdNamePtr = cmdName; //'P'
		int8_t *running = msgDup;
		ConvertStringToLowerCase((int8_t*) cmdNamePtr,
				(int32_t) (strlen((char*) cmdNamePtr)));
		ConvertStringToLowerCase((int8_t*) msgDup,
				(int32_t) (strlen((char*) msgDup)));
		int8_t * retString = (int8_t*) strstr((char*) running,
				(char*) cmdNamePtr); //reString=0;running='=10'
		if (retString != running) { //ret!=null
			ret = -1;
			return ret;
		}
		int8_t delimiters[] = "=:";
		memcpy((char*) msgDup, (char*) chBuff, strlen((char*) chBuff) + 1); //to protect origin string
		running = msgDup;
		retString = (int8_t*) strsep((char**) &running, (char*) delimiters); //reString = 'c',running='10...'
		retString = (int8_t*) strsep((char**) &running, (char*) delimiters); //reString = '10',running=0
		int8_t* para1Input = retString;
		if (!running) { //make sure the end of reply msg
			DEBUG(LOG_TEST, "%s\r\n", para1Input);
			REQUIRE(para1!=0);
			*para1 = atoi((char*) para1Input);
		} else {
			DEBUG(LOG_TEST, "%s\r\n", retString);
		}
		break;
	}
	return ret;
}
int32_t WaitAck_cRBTRS232ProtocolWrapper(BufferedSerial* port,
		int32_t protocolIndex, int32_t* para1, int8_t * buff) {
	int32_t ret = 0; //0=ack,-1=nack
	int8_t ch = 0;
	int8_t chBuff[20];
	int32_t index = 0;
	while (1) { //todo:loop to clear echo msg
		if (port->readable()) {
			ch = port->getc();
			if (ch == '\r') {
				chBuff[index] = '\0';
				break;
			}
			chBuff[index++] = ch;
		}
	}
	index = 0;
	while (1) {
		if (port->readable()) {
			ch = port->getc();
			if (ch == '\r') {
				chBuff[index] = '\0';
				break;
			}
			chBuff[index++] = ch;
		}
	}
	DEBUG(LOG_TEST, "%s\r\n", chBuff);

	switch (chBuff[0]) {
	case '+':
		ret = 0;
		DEBUG(LOG_TEST, "ACK\r\n", chBuff);
		break;
	case '-':
		ret = -1;
		DEBUG(LOG_TEST, "NACK\r\n", chBuff);
		break;
	default:
		//reply msg example: C=10
		index = 0;
		ret = 0;
		DEBUG(LOG_TEST, "got:%s\r\n", chBuff);
		int8_t cmdName[7];
		memcpy(cmdName, &RBTProtocolEncode[protocolIndex][0][1],
				strlen(&RBTProtocolEncode[protocolIndex][0][1]) + 1);
		int8_t msgDup[20];
		memcpy((char*) msgDup, (char*) chBuff, strlen((char*) chBuff) + 1); //to protect origin string
		int8_t * cmdNamePtr = cmdName; //'P'
		int8_t *running = msgDup;
		ConvertStringToLowerCase(cmdNamePtr, strlen((char*) cmdNamePtr));
		ConvertStringToLowerCase(msgDup, strlen((char*) msgDup));
		int8_t * retString = (int8_t*) strstr((char*) running,
				(char*) cmdNamePtr); //reString=0;running='=10'
		if (retString != running) { //ret!=null
			ret = -1;
			return ret;
		}
		int8_t delimiters[] = "=:";
		memcpy((char*) msgDup, (char*) chBuff, strlen((char*) chBuff) + 1); //to protect origin string
		running = msgDup;
		retString = (int8_t*) strsep((char**) &running, (char*) delimiters); //reString = 'c',running='10...'
		retString = (int8_t*) strsep((char**) &running, (char*) delimiters); //reString = '10',running=0
		int8_t* para1Input = retString;
		if (!running) { //make sure the end of reply msg
			DEBUG(LOG_TEST, "%s\r\n", para1Input);
			REQUIRE(para1!=0);
			*para1 = atoi((char*) para1Input);
		} else {
			DEBUG(LOG_TEST, "%s\r\n", retString);
		}
		break;
	}
	return ret;
}
int32_t WaitAck_cRBTRS232ProtocolWrapper1(int8_t * buff, int32_t buffLength,
		int32_t protocolIndex, int32_t* para1) {
	int32_t ret = 0; //0=ack,-1=nack,-2=clearEcho,1=got 1 para1, 2=got 2 para

	DEBUG(LOG_TEST, "%s\r\n", buff);

	switch (buff[0]) {
	case '+':
		ret = 0;
		DEBUG(LOG_TEST, "ACK\r\n", buff);
		break;
	case '-':
		ret = -1;
		DEBUG(LOG_TEST, "NACK\r\n", buff);
		break;
	default:
		//reply msg example: C=10
		int8_t cmdName[7];
		memcpy(cmdName, &RBTProtocolEncode[protocolIndex][0][1],
				strlen(&RBTProtocolEncode[protocolIndex][0][1]) + 1);
		int8_t msgDup[20];
		memcpy((char*) msgDup, (char*) buff, strlen((char*) buff) + 1); //to protect origin string
		int8_t * cmdNamePtr = cmdName; //'C'
		int8_t *running = msgDup;
		ConvertStringToLowerCase(cmdNamePtr, strlen((char*) cmdNamePtr));
		ConvertStringToLowerCase(msgDup, strlen((char*) msgDup));
		int8_t * retString = (int8_t*) strstr((char*) running,
				(char*) cmdNamePtr); //reString=0;running='=10'
		if (retString != running) { //ret!=null
			if (strlen((char*) retString) + 1 == strlen((char*) msgDup)) {
				ret = -2;
			} else
				ret = -1;
			return ret;
		}
		int8_t delimiters[] = "=:";
		memcpy((char*) msgDup, (char*) buff, strlen((char*) buff) + 1); //to protect origin string
		running = msgDup;
		retString = (int8_t*) strsep((char**) &running, (char*) delimiters); //reString = 'c',running='10...'

		int32_t indexInput = 0;
		REQUIRE(para1!=0);
		while (running != 0) {
			retString = (int8_t*) strsep((char**) &running, (char*) delimiters); //reString = '10',running=0
			int8_t* paraInput = retString;
			DEBUG(LOG_TEST, "input%d=%s\r\n", indexInput, paraInput);
			para1[indexInput++] = atoi((char*) paraInput);
			if (indexInput >= 2) {
				break;
			}
		}
		if (indexInput == 1)
			ret = 1;
		else if (indexInput == 2)
			ret = 2;
		break;
	}

	return ret;
}
