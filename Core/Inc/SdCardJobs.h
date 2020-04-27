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
#include "ProjDefs.h"

/* Exported functions prototypes ---------------------------------------------*/

/*
 * Initialize disk & mount FATFS on it
 * Returns TRUE if OK
 */
BOOL okFatfsOnSd_mount(void);


#ifdef __cplusplus
}
#endif

#endif /* INC_SDCARDJOBS_H_ */
