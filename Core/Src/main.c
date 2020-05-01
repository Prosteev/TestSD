/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#define I_AM_IN_MAIN_C

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "SdCardJobs.h"
#include "MainEventFlags.h"
#include "log_tasked.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUTTON_AMOUNT 2
/* event flags for spi_event_flags */
//#define EF_SPI_SEND_TEST_CMD 0x1 /* SPI: send test command */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

PCD_HandleTypeDef hpcd_USB_FS;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for spiTask */
osThreadId_t spiTaskHandle;
const osThreadAttr_t spiTask_attributes = {
  .name = "spiTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 450 * 4
};
/* Definitions for buttonTask */
osThreadId_t buttonTaskHandle;
const osThreadAttr_t buttonTask_attributes = {
  .name = "buttonTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 180 * 4
};
/* Definitions for sdFatfsTask */
osThreadId_t sdFatfsTaskHandle;
const osThreadAttr_t sdFatfsTask_attributes = {
  .name = "sdFatfsTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 420 * 4
};
/* Definitions for logTask */
osThreadId_t logTaskHandle;
const osThreadAttr_t logTask_attributes = {
  .name = "logTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1000 * 4
};
/* Definitions for syscallCountingSem */
osSemaphoreId_t syscallCountingSemHandle;
const osSemaphoreAttr_t syscallCountingSem_attributes = {
  .name = "syscallCountingSem"
};
/* USER CODE BEGIN PV */

//osEventFlagsId_t spi_event_flags = NULL;
static BOOL sd_card_init_needed = TRUE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USB_PCD_Init(void);
void StartDefaultTask(void *argument);
void SpiTask(void *argument);
void ButtonTask(void *argument);
void SdFatfsTask(void *argument);
void LogTask(void *argument);

/* USER CODE BEGIN PFP */
BOOL MakeSdCardJob(void);

void Button1_Pressed(void);
void Button2_Pressed(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * защита от применения где-то malloc & free
 */
extern void nonexistentfunction( void );

void *malloc( size_t size )
{
    ( void ) size;
    nonexistentfunction();
    return NULL;
}

void free( void *pvMemory )
{
    ( void ) pvMemory;
    nonexistentfunction();
}

/*
 * additional 64bit tick counter function
 *
 * https://www.keil.com/pack/doc/cmsis/RTOS2/html/group__CMSIS__RTOS__KernelCtrl.html#ga84bcdbf2fb76b10c8df4e439f0c7e11b
 *
 * Due to the limited value range u
 * sed for the tick count it may overflow during runtime,
 * i.e. after 232 ticks which are roughly 49days @ 1ms.
 * Typically one has not to take special care of this
 * unless a monotonic counter is needed. For such a case
 * an additional 64bit tick counter can be implemented as follows.
 * The given example needs GetTick() called
 * at least twice per tick overflow to work properly.
 */
uint64_t GetTick(void)
{
  static uint32_t tick_h = 0U;
  static uint32_t tick_l = 0U;
         uint32_t tick;
  tick = osKernelGetTickCount();
  if (tick < tick_l) {
    tick_h++;
  }
  tick_l = tick;
  return (((uint64_t)tick_h << 32) | tick_l);
}


BOOL MakeSdCardJob(void)
{
	return SdFat_okMount();
}

void Button1_Pressed(void)
{
	static Log_TMessage mes;

	mes.id = osKernelGetTickCount();
	strncpy(mes.mes,"qwerty",sizeof(mes.mes));
	if(Log_okPut(&mes))
		HAL_GPIO_WritePin(LD5_GPIO_Port,LD5_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LD6_GPIO_Port,LD6_Pin,GPIO_PIN_SET);
//				ev_flag_set_ret_value=
//				osEventFlagsSet(jobs_event_flags,JF_WRITE_LOG);
}
void Button2_Pressed(void)
{

}


//{
//	DSTATUS dstat = RES_OK;
//
////    FATFS *fs;     /* Pointer to the filesystem object */
//	FATFS ofs;
//    FRESULT fres;
//
//    FIL fil;        /* File object */
//    char line[20]; /* Line buffer */
//    FRESULT fr;     /* FatFs return code */
//
//	dstat = disk_initialize(0);
//	if(dstat != RES_OK)
//		return FALSE;
//
////    fs = malloc(sizeof (FATFS));/* Get work area for the volume */
////    if(fs==NULL)
////    {
////    	int e = errno;
////    	return FALSE;
////    }
//
//    fres=f_mount(&ofs, "", 1);/* Mount the default drive */
//    if(fres!=FR_OK)
//    	return FALSE;
//
//    /* Open a text file */
//    fr = f_open(&fil, "folder.ini", FA_READ);
//    if (fr)
//    	return FALSE;
////    return (int)fr;
//
//    /* Read every line and display it */
//    f_gets(line, sizeof line, &fil);
//
//    /* Close the file */
//    f_close(&fil);
//
//    fres=f_mount(0, "", 0);/* Unmount the default drive */
//     if(fres!=FR_OK)
//    	return FALSE;
//
////   free(fs);/* Here the work area can be discarded */
//
//   return TRUE;
//}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USB_PCD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of syscallCountingSem */
  syscallCountingSemHandle = osSemaphoreNew(1, 1, &syscallCountingSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of spiTask */
  spiTaskHandle = osThreadNew(SpiTask, NULL, &spiTask_attributes);

  /* creation of buttonTask */
  buttonTaskHandle = osThreadNew(ButtonTask, NULL, &buttonTask_attributes);

  /* creation of sdFatfsTask */
  sdFatfsTaskHandle = osThreadNew(SdFatfsTask, NULL, &sdFatfsTask_attributes);

  /* creation of logTask */
  logTaskHandle = osThreadNew(LogTask, NULL, &logTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

//  spi_event_flags = osEventFlagsNew(NULL);
  main_event_flags = osEventFlagsNew(NULL);
//  jobs_event_flags = osEventFlagsNew(NULL);

  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
 
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin|LD4_Pin|LD3_Pin|LD5_Pin 
                          |LD7_Pin|LD9_Pin|LD10_Pin|LD8_Pin 
                          |LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : DRDY_Pin MEMS_INT3_Pin MEMS_INT4_Pin MEMS_INT1_Pin 
                           MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = DRDY_Pin|MEMS_INT3_Pin|MEMS_INT4_Pin|MEMS_INT1_Pin 
                          |MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_I2C_SPI_Pin SD_CS_Pin LD4_Pin LD3_Pin 
                           LD5_Pin LD7_Pin LD9_Pin LD10_Pin 
                           LD8_Pin LD6_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin|SD_CS_Pin|LD4_Pin|LD3_Pin 
                          |LD5_Pin|LD7_Pin|LD9_Pin|LD10_Pin 
                          |LD8_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B2_Pin */
  GPIO_InitStruct.Pin = B2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(B2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  if(/*jobs_event_flags==NULL ||*/ main_event_flags==NULL || sd_card_init_needed)
		  osDelay(100);
	  else
		  osDelay(500);

	  HAL_GPIO_TogglePin(LD4_GPIO_Port,LD4_Pin);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_SpiTask */
/**
* @brief Function implementing the spiTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SpiTask */
void SpiTask(void *argument)
{
  /* USER CODE BEGIN SpiTask */

	/* Infinite loop */
	for(;;)
	{
//		if(osEventFlagsWait(
//				spi_event_flags,
//				EF_SPI_SEND_TEST_CMD,
//				osFlagsWaitAny,
//				0) == EF_SPI_SEND_TEST_CMD)
//		{	// SPI send test command received
//			HAL_GPIO_TogglePin(LD5_GPIO_Port,LD5_Pin);/* indicate start command */

//			HAL_GPIO_WritePin(
//					LD7_GPIO_Port,
//					LD7_Pin,
//					MakeSdCardJob() ? GPIO_PIN_SET : GPIO_PIN_RESET);
//					HAL_GPIO_WritePin(LD10_GPIO_Port,LD10_Pin,GPIO_PIN_SET);
//		}
		osDelay(1);
	}
  /* USER CODE END SpiTask */
}

/* USER CODE BEGIN Header_ButtonTask */
/**
* @brief Function implementing the buttonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ButtonTask */
void ButtonTask(void *argument)
{
  /* USER CODE BEGIN ButtonTask */
	typedef struct
	{
		GPIO_TypeDef * port;
		uint16_t pin;
		GPIO_TypeDef * led_port;
		uint16_t led_pin;
		char prev_state_is_pressed;
		char state_is_now_pressed;
		GPIO_PinState current_state;
		void (*ButtonPressedFunc)(void);
	} TButtonData;

	static TButtonData button[BUTTON_AMOUNT] =
	{
			{ B1_GPIO_Port, B1_Pin, LD3_GPIO_Port, LD3_Pin, 0, 0, GPIO_PIN_RESET, Button1_Pressed },
			{ B2_GPIO_Port, B2_Pin, LD10_GPIO_Port, LD10_Pin, 0, 0, GPIO_PIN_RESET, Button2_Pressed }
	};
//	static char button_prev_state_is_pressed=0;
//	char button_state_is_now_pressed;
//	GPIO_PinState button_current_state;

	/* Infinite loop */
	for(;;)
	{
		osDelay(100); // некая защита от дребезга кнопки
		for(int i=0; i < BUTTON_AMOUNT; ++i)
		{
			button[i].current_state = HAL_GPIO_ReadPin( button[i].port, button[i].pin );
			HAL_GPIO_WritePin( button[i].led_port, button[i].led_pin, button[i].current_state );
			button[i].state_is_now_pressed = (button[i].current_state == GPIO_PIN_SET);
			if(button[i].prev_state_is_pressed != button[i].state_is_now_pressed)
			{	// button state changed
				if(button[i].current_state == GPIO_PIN_SET)
				{	// button just pressed
					button[i].prev_state_is_pressed=1;
				}
				else
				{	// button just released
					button[i].prev_state_is_pressed=0;
					button[i].ButtonPressedFunc();
				}
			}
		}
//		button_current_state=HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin);
//		HAL_GPIO_WritePin(LD3_GPIO_Port,LD3_Pin,button_current_state);
//		button_state_is_now_pressed = (button_current_state == GPIO_PIN_SET);

//		if(button_prev_state_is_pressed != button_state_is_now_pressed)
//		{	// button state changed
//			if(button_current_state == GPIO_PIN_SET)
//			{	// button just pressed
//				button_prev_state_is_pressed=1;
////				HAL_GPIO_WritePin(LD7_GPIO_Port,LD7_Pin,GPIO_PIN_RESET);
////				HAL_GPIO_WritePin(LD8_GPIO_Port,LD8_Pin,GPIO_PIN_RESET);
////				HAL_GPIO_WritePin(LD9_GPIO_Port,LD9_Pin,GPIO_PIN_RESET);
////				HAL_GPIO_WritePin(LD10_GPIO_Port,LD10_Pin,GPIO_PIN_RESET);
//			}
//			else
//			{	// button just released
//				button_prev_state_is_pressed=0;
//				mes.id = osKernelGetTickCount();
//				strncpy(mes.mes,"qwerty",sizeof(mes.mes));
//				if(Log_okPut(&mes))
//					HAL_GPIO_WritePin(LD5_GPIO_Port,LD5_Pin,GPIO_PIN_SET);
//				else
//					HAL_GPIO_WritePin(LD6_GPIO_Port,LD6_Pin,GPIO_PIN_SET);
////				ev_flag_set_ret_value=
////				osEventFlagsSet(jobs_event_flags,JF_WRITE_LOG);
//			}
//		}
	}
  /* USER CODE END ButtonTask */
}

/* USER CODE BEGIN Header_SdFatfsTask */
/**
* @brief Function implementing the sdFatfsTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SdFatfsTask */
void SdFatfsTask(void *argument)
{
  /* USER CODE BEGIN SdFatfsTask */
  /* Infinite loop */
  for(;;)
  {
	  if(sd_card_init_needed)
	  {
		  if(SdFat_okMount())
			  sd_card_init_needed=FALSE;
	  }

	  HAL_GPIO_WritePin(
			  LD7_GPIO_Port,
			  LD7_Pin,
			  SdFat_isReady() ? GPIO_PIN_SET : GPIO_PIN_RESET);

	  osDelay(1);
  }
  /* USER CODE END SdFatfsTask */
}

/* USER CODE BEGIN Header_LogTask */
/**
* @brief Function implementing the logTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LogTask */
void LogTask(void *argument)
{
  /* USER CODE BEGIN LogTask */
 	static Log_TMessage Message;
 	static uint32_t log_not_ready_counter=0;
 	static uint32_t no_or_errGetFromQueue_counter=0;
 	static uint32_t errWriteToLogFile_counter=0;

 /* Infinite loop */
  for(;;)
  {
    osDelay(1);
    if(Log_isReady())
    {
    	HAL_GPIO_WritePin(LD9_GPIO_Port,LD9_Pin,GPIO_PIN_SET);

    	if(Log_okGetFromQueue(&Message))
    	{
    		if(Log_okWriteToLogFile(&Message))
    		{
				HAL_GPIO_WritePin(LD5_GPIO_Port,LD5_Pin,GPIO_PIN_RESET);
				Log_FileClose();
    		}
    		else
    			++errWriteToLogFile_counter;
    	}
    	else
    		++no_or_errGetFromQueue_counter;
    }
    else
    {
    	if( ! Log_okInit() )
    	{
    		HAL_GPIO_WritePin(LD9_GPIO_Port,LD9_Pin,GPIO_PIN_RESET);
    		++log_not_ready_counter;
    	}
    }
  }
  /* USER CODE END LogTask */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

	for(;;)
		;

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
