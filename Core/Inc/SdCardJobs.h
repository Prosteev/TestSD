/*
 * SdCardJobs.h
 *
 * Data & functions for working with FATFS on SD-card in RTOS
 *
 *  Created on: Apr 27, 2020
 *      Author: iva
 */

#ifndef INC_SDCARDJOBS_H_
#define INC_SDCARDJOBS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "ProjDefs.h"
/* Exported types ------------------------------------------------------------*/
typedef uint8_t SdFat_ErrorFlags_t;
typedef uint8_t SdFat_StateFlags_t;
/* Exported constants --------------------------------------------------------*/
/* SdFat_ErrorFlags_t */
#define SdFat_EF_DISK_INIT			(1<<0) /* disk_initialize() error */
#define SdFat_EF_FMOUNT				(1<<1) /* f_mount error */
#define SdFat_EF_SET_MAIN_EV_FLAG	(1<<2) /* main_event_flags setting error */
//#define Log_EF_MES_QUEUE_INIT	(1<<3) /* Message Queue initialization error */
//#define Log_EF_MES_QUEUE_PUT	(1<<4) /* Put message into Message Queue error */
//#define Log_EF_MES_QUEUE_GET	(1<<4) /* Get message from Message Queue error */

/* SdF_StateFlags_t */
#define SdFat_SF_MOUNTED		    (1<<1) /* log file opened to write */
/* Exported functions prototypes ---------------------------------------------*/

/*
 * Initialize SD-card & mount FATFS on it
 * Returns TRUE if OK
 */
BOOL SdFat_okMount(void);

BOOL SdFat_isReady(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SDCARDJOBS_H_ */
