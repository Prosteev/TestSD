/*
 * log_tasked.c
 *
 *  Created on: Apr 27, 2020
 *      Author: iva
 */

/* Includes ------------------------------------------------------------------*/
#include "log_tasked.h"

#include <string.h>
#include "cmsis_os.h"
#include "fatfs.h"
#include "MainEventFlags.h"
/* Private defines -----------------------------------------------------------*/
#define QUEUE_MES_COUNT      5
//#define MP_BLOCK_SIZE      LOG_MES_LEN
//#define MP_BLOCK_COUNT     5
/* Private variables ---------------------------------------------------------*/
static Log_ErrorFlags_t log_error_flags = 0;
static Log_StateFlags_t log_state_flags = 0;

FIL log_file;        /* File object */
FRESULT fres;   /* FatFs return code */

osMessageQueueId_t MsgQueue = NULL;
//osMemoryPoolId_t mem_pool; /* Memory Pool for messages to be written to log file */

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

BOOL Log_okInit(void)
{
	uint32_t ev_flag_set_res;

	// check preconditions
	log_state_flags=0;
    if(main_event_flags==NULL)
    	return FALSE; // flags not exist

    if(osEventFlagsGet(main_event_flags) & MF_LOG_IS_READY)
    	{return TRUE;} // already initialized

    if( (osEventFlagsGet(main_event_flags) & MF_FATFS_ON_SD_INITED)==0 )
    {	// SD & FATFS not initialized
    	log_error_flags = Log_EF_INIT_NO_SD_FATFS;
    	return FALSE;
    }

 	// basic work

   if( (log_state_flags & Log_SF_OPEN_2WRITE)==0 )
   {	// is not opened

	   /* Open a text file */
	   fres = f_open(&log_file, "a.log", FA_CREATE_ALWAYS | FA_WRITE);
	   if (fres)
	   {
		   log_error_flags = Log_EF_FOPEN_2WRITE;
		   return FALSE;
	   }

	   if(MsgQueue==NULL)
	   {
		   MsgQueue = osMessageQueueNew(QUEUE_MES_COUNT, sizeof(Log_TMessage), NULL);
		   if(MsgQueue == NULL)
		   {	// Message Queue object is not created
			   log_error_flags = Log_EF_MES_QUEUE_INIT;
			   return FALSE;
		   }
	   }
	   else
	   {
		   if(osMessageQueueReset(MsgQueue)!=osOK)
		   {	// Message Queue object is not reset
			   log_error_flags = Log_EF_MES_QUEUE_RESET;
			   return FALSE;
		   }
	   }
//	   mem_pool = osMemoryPoolNew( MP_BLOCK_COUNT, MP_BLOCK_SIZE, NULL);
//	   if(mem_pool==0)
//	   {
//		   error_flags = Log_ERR_MEM_POOL_INIT;
//		   return FALSE;
//	   }

	   log_state_flags |= Log_SF_OPEN_2WRITE;
   }

   ev_flag_set_res=
		   osEventFlagsSet(main_event_flags,MF_LOG_IS_READY);
   if( (int32_t)ev_flag_set_res < 0 || (ev_flag_set_res & MF_LOG_IS_READY)==0 )
   {
	   log_error_flags |= Log_EF_SET_MAIN_EV_FLAG;
	   return FALSE;
   }

   return TRUE;
}

BOOL Log_okPut(Log_TMessage * Message)
{
	osStatus_t status;

	// check preconditions
	if(Message==NULL)
		return FALSE;
	if( !Log_isReady() )
		return FALSE;

	// basic work
	status = osMessageQueuePut( MsgQueue, Message, 0, osWaitForever );
	if(status!=osOK)
	{
		log_state_flags |= Log_EF_MES_QUEUE_PUT;
		return FALSE;
	}
	log_state_flags &= ~Log_EF_MES_QUEUE_PUT;
	return TRUE;
}

BOOL Log_okGetFromQueue(Log_TMessage * Message)
{
	osStatus_t status;

	// check preconditions
	if(Message==NULL)
		return FALSE;
	if( !Log_isReady() )
		return FALSE;

	// basic work
    status = osMessageQueueGet(MsgQueue, Message, NULL, 0U);   // don't wait for message (try semantics)
    switch (status)
    {
		case osOK:
			// the message has been retrieved from the queue OR
			log_state_flags &= ~Log_EF_MES_QUEUE_GET;
			return TRUE;
			break;

		case osErrorResource:
			//nothing to get from the queue (try semantics)
			log_state_flags &= ~Log_EF_MES_QUEUE_GET;
			return FALSE;
			break;

		default:
			log_state_flags |= Log_EF_MES_QUEUE_GET;
	    	return FALSE;
			break;
	}
}

BOOL Log_okWriteToLogFile(Log_TMessage * Message)
{
	UINT len;
	UINT bw;

	// check preconditions
	if(Message==NULL)
		return FALSE;
	if( !Log_isReady() )
		return FALSE;
	if(f_error(&log_file))
		return FALSE;

	// basic work
//	if( f_printf(&log_file, "%ld: %s", Message->id,Message->mes) == EOF )
//	if( f_puts(Message->mes, &log_file) == EOF )
//		return FALSE;
	len = strlen(Message->mes);
	fres = f_write( &log_file, Message->mes, len, &bw );
	if( fres == FR_OK && bw == len)
		return TRUE; // write message - ok
	// write message error
	if( fres == FR_DISK_ERR || fres == FR_INT_ERR )
		Log_FileClose();
	return FALSE;
}

Log_ErrorFlags_t Log_GetErrorFlags(void)
{
	return log_error_flags;
}

BOOL Log_isReady(void)
{
	if( (log_state_flags & Log_SF_OPEN_2WRITE)==0 )
		return FALSE; // log is not opened
	if(main_event_flags==NULL)
		return FALSE; // flags not exist
	if( (osEventFlagsGet(main_event_flags) & MF_LOG_IS_READY)==0 )
		return FALSE; // log is not initialized

	return TRUE;
}

void Log_FileClose(void)
{
	// check preconditions
	if( (log_state_flags & Log_SF_OPEN_2WRITE)==0 )
		return; // log is not opened

	// basic work
	fres = f_close(&log_file);
	log_state_flags &= ~Log_SF_OPEN_2WRITE;
	osEventFlagsClear(main_event_flags, MF_LOG_IS_READY);
}
