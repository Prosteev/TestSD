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
 * flags for main_event_flags
 */
#define MF_FATFS_ON_SD_INITED	(0x1) /* FATFS on SD-card initialized successfully */
#define MF_LOG_IS_READY			(0x2) /* Log object is ready logging */

/*
 * flags for jobs_event_flags
 */
#define JF_WRITE_LOG			(0x1) /* write test message to log */


#ifndef I_AM_IN_MAIN_C
#define MF_EXTERN extern
#define MF_ZERO
#else
#define MF_EXTERN
#define MF_ZERO = NULL
#endif


MF_EXTERN osEventFlagsId_t main_event_flags MF_ZERO;
//MF_EXTERN osEventFlagsId_t jobs_event_flags MF_ZERO;


#undef MF_EXTERN
#undef MF_ZERO

#endif /* INC_MAINEVENTFLAGS_H_ */
