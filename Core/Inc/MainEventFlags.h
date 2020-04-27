/*
 * MainEventFlags.h
 *
 * Defines this project main osEventFlags
 *
 *  Created on: Apr 27, 2020
 *      Author: iva
 */

#ifndef INC_MAINEVENTFLAGS_H_
#define INC_MAINEVENTFLAGS_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

/* Exported macro ------------------------------------------------------------*/
/*
 * project main event flags for spi_event_flags
 */
#define MF_FATFS_ON_SD_INITED (0x1) /* FATFS on SD-card initialized successfully */

#ifndef I_AM_IN_MAIN_C
extern
#endif
osEventFlagsId_t main_event_flags
#ifdef I_AM_IN_MAIN_C
= NULL
#endif
;

#endif /* INC_MAINEVENTFLAGS_H_ */
