/*
 * SdCardJobs.c
 *
 *  Created on: Apr 27, 2020
 *      Author: iva
 */

/* Includes ------------------------------------------------------------------*/
#include "SdCardJobs.h"

#include "cmsis_os.h"
#include "fatfs.h"
#include "MainEventFlags.h"

/* Private variables ---------------------------------------------------------*/
FATFS fs;

BOOL okFatfsOnSd_mount(void)
{
	DSTATUS dstat = RES_OK;
    FRESULT fres;
    uint32_t event_res;

    if(main_event_flags==NULL)
    	return FALSE; // flags not exist

    if(osEventFlagsGet(main_event_flags) & MF_FATFS_ON_SD_INITED)
    	{return TRUE;} // already initialized

	dstat = disk_initialize(0);
	if(dstat != RES_OK)
		return FALSE;

    fres = f_mount(&fs, "", 1);/* Mount the default drive */
    if(fres!=FR_OK)
    	return FALSE;

    event_res = osEventFlagsSet(main_event_flags,MF_FATFS_ON_SD_INITED);
    if((int32_t)event_res < 0 || (event_res & MF_FATFS_ON_SD_INITED)==0)
     	return FALSE;

   return TRUE;
}
