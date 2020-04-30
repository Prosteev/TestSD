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
static SdFat_ErrorFlags_t error_flags = 0;
static SdFat_StateFlags_t state_flags = 0;

FATFS fs;

BOOL SdFat_okMount(void)
{
	DSTATUS dstat = RES_OK;
    FRESULT fres;
    uint32_t event_res;

    if(main_event_flags==NULL)
    	return FALSE; // flags not exist

    if(osEventFlagsGet(main_event_flags) & MF_FATFS_ON_SD_INITED)
    {	// already initialized
    	return TRUE;
    }

	dstat = disk_initialize(0);
	if(dstat != RES_OK)
	{
		error_flags = SdFat_EF_DISK_INIT;
		return FALSE;
	}

    fres = f_mount(&fs, "", 1);/* Mount the default drive */
    if(fres!=FR_OK)
    {
    	error_flags = SdFat_EF_FMOUNT;
    	return FALSE;
    }

    event_res = osEventFlagsSet(main_event_flags,MF_FATFS_ON_SD_INITED);
    if((int32_t)event_res < 0 || (event_res & MF_FATFS_ON_SD_INITED)==0)
    {
    	error_flags = SdFat_EF_SET_MAIN_EV_FLAG;
     	return FALSE;
    }

    state_flags = SdFat_SF_MOUNTED;
    error_flags = 0;

    return TRUE;
}

BOOL SdFat_isReady(void)
{
	return ((state_flags & SdFat_SF_MOUNTED) && error_flags == 0)
			? TRUE : FALSE;
}
