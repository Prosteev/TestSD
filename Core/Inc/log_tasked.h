/*
 * log_tasked.h
 *
 * Pseudo-Object (Data & functions) for logging to text file on SD-card in RTOS
 *
 *  Created on: Apr 27, 2020
 *      Author: iva
 */

#ifndef INC_LOG_TASKED_H_
#define INC_LOG_TASKED_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "ProjDefs.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint32_t id;
	char mes[LOG_MES_LEN];
} Log_TMessage;

typedef uint8_t Log_ErrorFlags_t;
typedef uint8_t Log_StateFlags_t;

/* Exported constants --------------------------------------------------------*/
/* Log_ErrorFlags_t */
#define Log_EF_INIT_NO_SD_FATFS	(1<<0) /* SD & FATFS not initialized */
#define Log_EF_FOPEN_2WRITE		(1<<1) /* log file open to write error */
#define Log_EF_SET_MAIN_EV_FLAG	(1<<2) /* main_event_flags setting error */
#define Log_EF_MES_QUEUE_INIT	(1<<3) /* Message Queue initialization error */
#define Log_EF_MES_QUEUE_PUT	(1<<4) /* Put message into Message Queue error */
#define Log_EF_MES_QUEUE_GET	(1<<5) /* Get message from Message Queue error */
#define Log_EF_MES_QUEUE_RESET	(1<<6) /* Message Queue reset error */
//#define Log_ERR_MEM_POOL_INIT    (0x8) /* Memory Pool initialization error */
/* Log_StateFlags_t */
#define Log_SF_OPEN_2WRITE    (1<<1) /* log file opened to write */

/* Exported functions prototypes ---------------------------------------------*/

/*
 * Initialize Log for writing
 * Returns TRUE if OK
 */
BOOL Log_okInit(void);

/*
 * Put Message to Log Message Queue (not write to log file!)
 * Waits if Queue is full
 * Returns TRUE if OK
 */
BOOL Log_okPut(Log_TMessage * Message);

/*
 * Get next message from Log Message Queue
 * don't wait if Message Queue is empty
 * must be called from Log Task
 * Returns TRUE if next message was get from Queue
 */
BOOL Log_okGetFromQueue(Log_TMessage * Message);

/*
 * Write Message to log file
 * Returns TRUE if message was written
 */
BOOL Log_okWriteToLogFile(Log_TMessage * Message);
/*
 * Returns TRUE if Log pseudo-object is ready
 */
BOOL Log_isReady(void);

void Log_FileClose(void);

Log_ErrorFlags_t Log_GetErrorFlags(void);


#ifdef __cplusplus
}
#endif

#endif /* INC_LOG_TASKED_H_ */
