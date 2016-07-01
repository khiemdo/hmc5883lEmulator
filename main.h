#include "stm32f4xx_hal.h"

#define DEBUGUART_ID 3

#define USE_FULL_ASSERT
#define USE_MY_ASSERT
#define DEBUGON
#define DEBUGLV  LOG_TEST
#define ASSERTLV 2

#define CRC_CCITT



/**
 * \brief: help to index source files and make sure no duplication of index
 * \ref:http://www.barrgroup.com/Embedded-Systems/How-To/Define-Assert-Macro
 */
#define FILENUM(num) \
    enum { F_NUM=num }; \
    void _dummy ## num(void) {}

/*
 * main.c 					FILENUM(100);
 * stm32f4xx_it.c 			FILENUM(1000);
 * stm32f4xx_hal_msp.c 		FILENUM(1001);
 * project_api.c			FILENUM(1002);
 *
 * cSPIPort.c				FILENUM(1000000);
 * TestcSPIPort.c			FILENUM(1000001);
 */

