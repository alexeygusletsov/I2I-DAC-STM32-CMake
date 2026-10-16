
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "main.h"
#include "userdefine.h"
#include "command_handler.h"
#include "flash.h"
#include "utils.h"
#include "ring_buffer.h"
#include <string.h>
#include <stdio.h>

#define USER_FLASH_PAGE_SIZE (FLASH_PAGE_SIZE)
#define FLASH_START_ADDRESS (0x08000000)
#define FLASH_PROGRAM_SIZE (60 * USER_FLASH_PAGE_SIZE)
#define USER_FLASH_START_ADDRESS (FLASH_START_ADDRESS + FLASH_PROGRAM_SIZE)

const char* promt = "trps$>";
const char* start = "\rTPRS has started.\r";

volatile flags_t flags = {
    .rx_flag = 0,
	  .overrun = 0,
	  .ADC_Ready = 0,
	  .SPI_Ready = 1,
	  .Timer100ms_Ready = 0,
	  .tx_ready = 1
};

Parameters_t params;

uint16_t DAC_input[USER_DAC_NUMBER];
uint16_t bufferADC = 0;
uint8_t counterADC = 0;

node_t nodes[] = {
		{ HELP_CMD,    NULL,              NULL,              (f_t)HANDLER_NAME(help) },
		{ VERSION_CMD, NULL,              NULL,              (f_t)HANDLER_NAME(version) },
		{ NAME_CMD,    NULL,              NULL,              (f_t)HANDLER_NAME(name) },
        { ADC_CMD,     (void*)&bufferADC, NULL,              (f_t)HANDLER_NAME(read_adc)},
		{ CURRENT_CMD, (void*)&bufferADC, NULL,              (f_t)HANDLER_NAME(read_current) },
		{ UPDATE_CMD,  (void*)&params,    NULL,              (f_t)HANDLER_NAME(update) },
		{ DAC_CMD,     (void*)DAC_input,  NULL,              (f_t)HANDLER_NAME(dac) },
		{ POINT_A_CMD, (void*)&params,    (void*)&bufferADC, (f_t)HANDLER_NAME(point) },
		{ POINT_B_CMD, (void*)&params,    (void*)&bufferADC, (f_t)HANDLER_NAME(point) },
		{ POINT_C_CMD, (void*)&params,    (void*)&bufferADC, (f_t)HANDLER_NAME(point) },
		{ POINT_D_CMD, (void*)&params,    (void*)&bufferADC, (f_t)HANDLER_NAME(point) },
		{ POINT_E_CMD, (void*)&params,    (void*)&bufferADC, (f_t)HANDLER_NAME(point) },
		{ POINT_F_CMD, (void*)&params,    (void*)&bufferADC, (f_t)HANDLER_NAME(point) },
		{ GREEN_CMD,   (void*)&params,    NULL,              (f_t)HANDLER_NAME(level) },
		{ ORANGE_CMD,  (void*)&params,    NULL,              (f_t)HANDLER_NAME(level) },
		{ RED_CMD,     (void*)&params,    NULL,              (f_t)HANDLER_NAME(level) },
		{ WARNING_CMD, (void*)&params,    NULL,              (f_t)HANDLER_NAME(level) },
		{ RESET_CMD,   (void*)&params,    NULL,              (f_t)HANDLER_NAME(reset) },
        { TEST_CMD,    (void*)&params,    NULL,              (f_t)HANDLER_NAME(test) },
		{ NULL_CMD,    NULL,              NULL,              NULL }
};

void SystemClock_Config(void);

void StartReceive(uint8_t *data, uint16_t size)
{
    assert_param(BOARD_Receive_IT(data, size) == 0);
}

void ReceiveData(uint8_t *data, uint16_t size)
{
    StartReceive(data, size);
}

void HandleReceive()
{
    flags.rx_flag = 1;
}

void HandleTransmit(void)
{
    flags.tx_ready = 1;
}

void HandleADCReady(void)
{
    flags.ADC_Ready = 1;
}

void process1HzTask(void)
{
}

int Parse(rbuf* buffer, uint8_t byte) {
    int ret_val = 0;
    if(byte == '\r') {
        byte = '\0';
        ret_val = 1;
    }
    rbuf_push(buffer, byte);
    return ret_val;
    
}

int Send_response(uint8_t* buf, size_t size) {
    assert_param(size - strlen((char*)buf) > 1);
    if(flags.tx_ready) {
        flags.tx_ready = 0;
        strncat((char*)buf, promt,
                size - strlen((char*)buf) - 1);
        assert_param(BOARD_Send_IT(buf, strlen((char*)buf)) == 0);
        return 0;
    } else {
        return -1;
    }
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    uint8_t recv_byte;
    uint8_t rx_data[RX_BUFFER_SIZE];
    rbuf* rx_buffer = rbuf_init(rx_data, sizeof(rx_data));
    assert_param(rx_buffer);

    HAL_Init();

    SystemClock_Config();
    SysTick_Config(SystemCoreClock/1000);

    BOARD_RegisterTxCallBack(HandleTransmit);
    BOARD_RegisterRxCallBack(HandleReceive);
    BOARD_RegisterADCCallBack(HandleADCReady);

    BOARD_Init();

    LED_Alarm(ON);
    HAL_Delay(2000);
    LED_Alarm(OFF);
    LED_Switch(RED);
    HAL_Delay(1000);
    LED_Switch(ORANGE);
    HAL_Delay(1000);
    LED_Switch(GREEN);
    HAL_Delay(1000);

    LED_Switch(NONE);
    LED_Alarm(OFF);

    // Init parameters
    ReadFlash((uint8_t*)&params, sizeof(Parameters_t));

    // Init DACs
    for(int i = 0; i < USER_DAC_NUMBER; i++) {
    	  InitDAC(i, &params.points[i][0]);
    }
    flash_status_t status = UpdateFlash((const uint8_t*)&params, sizeof(Parameters_t));
    if(status != FLASH_OK) {
        TRACE_UART("UpdateFlash failed (%d).\r\n", status);
        Error_Handler();
    }

    PrintParam("Parameters in RAM:", &params);
    PrintParam("Parameters in flash:", &params_in_flash);

    flags.tx_ready = 1;
    flags.rx_flag = 0;
    flags.overrun = 0;

    // Start to receive
    StartReceive(&recv_byte, sizeof(recv_byte));

    // Initiate start message
    TRACE_UART("%s", start);
   
    BOARD_ADC_Start();

    uint32_t tickstart = HAL_GetTick();
    const uint32_t Delay1s = 1000;

    // Main loop
    while (1) {
        if((HAL_GetTick() - tickstart) >= Delay1s) {
            tickstart = HAL_GetTick();
            process1HzTask();
        }
        // Serve ADC
        if(flags.ADC_Ready) {
            flags.ADC_Ready = 0;
            bufferADC = BOARD_ADC_Result();
            if((++counterADC % 2) == 0) {
                uint16_t input_value = ADC2mA(bufferADC);
                LED_Service(input_value, params.levels);
                for(int i = 0; i < USER_DAC_NUMBER; i++) {
                    DAC_input[i] = DAC_Tune(bufferADC, params.k[i], params.b[i]);
                    DAC_Write(i, DAC_input[i]);
                }
            }
            BOARD_ADC_Start();
        }
        // Receive request
        if(flags.rx_flag) {
            flags.rx_flag = 0;
            if(Parse(rx_buffer, recv_byte)) {
                Handle_command(&nodes, rx_data, Send_response);
                rbuf_reset(rx_buffer); // reset counter
            }
            // Continue receiving
            ReceiveData(&recv_byte, sizeof(recv_byte));
        }
    }
    // Shoul not get there
    assert_param(0);
    return 0;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI/2)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 8000000
  *            PREDIV                         = 1
  *            PLLMUL                         = 12
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* No HSE Oscillator on Nucleo, Activate PLL with HSI/2 as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    __disable_irq();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    const size_t size = 256;
    char buffer[size];
    snprintf(buffer, sizeof(buffer),
      "Assertion failed: file %s on line %lu\r\n", (char*)file, line);
    BOARD_Send((const uint8_t*)buffer, strlen((const char*)buffer), 1000);
    Error_Handler();
}
#endif
