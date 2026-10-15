/**
  ******************************************************************************
  * @file    board.c
  * @author  Alexey Gusletsov
  * @version V1.0.0
  * @date    12-May-2026
  * @brief   This file provides set of firmware functions to manage Leds and
  *          push-button available on Ito2I board.
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "board.h"

extern void Error_Handler(void);
/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup BOARD
  * @{
  */ 
    
/** @defgroup BOARD_LOW_LEVEL 
  * @brief This file provides firmware functions to manage Leds and push-buttons, 
  *        available on BOARD evaluation board from STMicroelectronics.
  * @{
  */ 

/** @defgroup BOARD_LOW_LEVEL_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup BOARD_LOW_LEVEL_Private_Defines
  * @{
  */ 
static ADC_HandleTypeDef hadc;

typedef enum {
    Board_DAC1 = 0,
    Board_DAC2,
    Board_DAC3,
    Board_DAC_Number
} Board_DAC_Num_t;

static UART_HandleTypeDef huart;

static SPI_HandleTypeDef hspi;

static pUSER_TxCallbackTypeDef User_Tx_CallBack = NULL;
static pUSER_RxCallbackTypeDef User_Rx_CallBack = NULL;
static pUSER_ADCCallbackTypeDef User_ADC_CallBack = NULL;

static pUSER_RxParamTypeDef User_Rx_Param = NULL;

/**
  * @}
  */ 


/** @defgroup BOARD_LOW_LEVEL_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup BOARD_LOW_LEVEL_Private_Variables
  * @{
  */ 
GPIO_TypeDef* LED_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT, LED4_GPIO_PORT};
const uint16_t LED_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};

GPIO_TypeDef* CS_PORT[CSn] = {CS1_GPIO_PORT, CS2_GPIO_PORT, CS3_GPIO_PORT};
const uint16_t CS_PIN[CSn] = {CS1_PIN, CS2_PIN, CS3_PIN};

GPIO_TypeDef* USART1_PORT[USART1n] = {USART1_RX_PORT, USART1_TX_PORT};
const uint16_t USART1_PIN[USART1n] = {USART1_RX_PIN, USART1_TX_PIN};

GPIO_TypeDef* SPI_PORT[SPIn] = {SPI_MOSI_PORT, SPI_SCK_PORT};
const uint16_t SPI_PIN[SPIn] = {SPI_MOSI_PIN, SPI_SCK_PIN};

/**
  * @}
  */ 


/** @defgroup BOARD_LOW_LEVEL_Private_FunctionPrototypes
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup BOARD_LOW_LEVEL_Private_Functions
  * @{
  */ 

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart);
}

void ADC1_COMP_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hadc);
}

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void BOARD_LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.Pin = LED_PIN[Led];
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStructure);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4  
  * @retval None
  */
void BOARD_LEDOn(Led_TypeDef Led)
{
    LED_PORT[Led]->BSRR = LED_PIN[Led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4 
  * @retval None
  */
void BOARD_LEDOff(Led_TypeDef Led)
{
    LED_PORT[Led]->BRR = LED_PIN[Led];
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4  
  * @retval None
  */
void BOARD_LEDToggle(Led_TypeDef Led)
{
  LED_PORT[Led]->ODR ^= LED_PIN[Led];
}

void BOARD_CSInit(CS_TypeDef CS)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    /* Enable the GPIO_CS Clock */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.Pin = CS_PIN[CS];
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CS_PORT[CS], &GPIO_InitStructure);
}

void BOARD_CSOn(CS_TypeDef CS)
{
    CS_PORT[CS]->BSRR = CS_PIN[CS];
}

void BOARD_CSOff(CS_TypeDef CS)
{
    CS_PORT[CS]->BRR = CS_PIN[CS];
}

static void BOARD_Tx_CallBack(UART_HandleTypeDef *husart) {
    if(User_Tx_CallBack) {
        User_Tx_CallBack();
    }
}

static void BOARD_Rx_CallBack(UART_HandleTypeDef *husart) {
    if(User_Rx_CallBack) {
        User_Rx_CallBack(User_Rx_Param);
    }
}

static void BOARD_ADC_CallBack(ADC_HandleTypeDef *hadc) {
    if(User_ADC_CallBack) {
        User_ADC_CallBack();
    }
}

void BOARD_UART1Init(void)
{
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable clock for USART1, AFIO and GPIOA */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    
    /* GPIOA PIN9 alternative function Tx */
    GPIO_InitStructure.Pin = USART1_PIN[TX];
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(USART1_PORT[TX], &GPIO_InitStructure);
    /* GPIOA PIN10 alternative function Rx */
    GPIO_InitStructure.Pin = USART1_PIN[RX];
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(USART1_PORT[RX], &GPIO_InitStructure);

    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    huart.Instance = BOARD_USART;
    huart.Init.BaudRate = BOARD_USART_BAUDRATE;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE ;
    huart.Init.Mode = UART_MODE_TX_RX;
    
    /* Configure UART1 */
    assert_param(HAL_UART_Init(&huart) == HAL_OK);
    assert_param(HAL_UART_RegisterCallback(&huart, HAL_UART_TX_COMPLETE_CB_ID, &BOARD_Tx_CallBack) == HAL_OK);
    assert_param(HAL_UART_RegisterCallback(&huart, HAL_UART_RX_COMPLETE_CB_ID, &BOARD_Rx_CallBack) == HAL_OK);

    __HAL_UART_ENABLE_IT(&huart, UART_IT_TC);
    __HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);
    __HAL_UART_ENABLE(&huart);
}

void BOARD_RegisterTxCallBack(pUSER_TxCallbackTypeDef callback)
{
    User_Tx_CallBack = callback;
}

void BOARD_RegisterRxCallBack(pUSER_RxCallbackTypeDef callback, pUSER_RxParamTypeDef param)
{
    User_Rx_CallBack = callback;
    User_Rx_Param = param;
}

void BOARD_RegisterADCCallBack(pUSER_ADCCallbackTypeDef callback)
{
    User_ADC_CallBack = callback;
}

void BOARD_Init(void)
{
    BOARD_LEDInit(LED1);
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_LEDInit(LED4);

    BOARD_CSInit(CS1);
    BOARD_CSInit(CS2);
    BOARD_CSInit(CS3);

    BOARD_SPI_Init();

    BOARD_UART1Init();

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    BOARD_ADC_Init();

    HAL_NVIC_SetPriority(ADC1_COMP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);
}

void BOARD_Send(const uint8_t *buf, uint16_t size, uint32_t timeout)
{
    HAL_UART_Transmit(&huart, buf, size, timeout);
}

int BOARD_Send_IT(const uint8_t *buf, uint16_t size)
{
    int ret_val = 0;

    if(HAL_UART_Transmit_IT(&huart, buf, size) != HAL_OK) {
        ret_val = -1;
    }
    return ret_val;
}

int BOARD_Receive_IT(uint8_t *buf, uint16_t size)
{
    int ret_val = 0;

    if(HAL_UART_Receive_IT(&huart, buf, size) != HAL_OK) {
      ret_val = -1;
    }
    return ret_val;
}

uint16_t BOARD_ADC_Result(void)
{
    return (uint16_t)HAL_ADC_GetValue(&hadc);
}

void BOARD_ADC_Start(void)
{
    HAL_ADC_Start_IT(&hadc);
}

void BOARD_ADC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL ;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc.Init.Resolution = ADC_RESOLUTION_10B;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.SamplingTimeCommon = ADC_SAMPLETIME_1CYCLE_5;
    
    assert_param(HAL_ADC_Init(&hadc) == HAL_OK);

    assert_param(HAL_ADCEx_Calibration_Start(&hadc) == HAL_OK);
    
    ADC_ChannelConfTypeDef config;
    config.Channel = ADC_CHANNEL_0;
    config.Rank = ADC_RANK_CHANNEL_NUMBER;

    assert_param(HAL_ADC_ConfigChannel(&hadc, &config) == HAL_OK);
    assert_param(HAL_ADC_RegisterCallback(&hadc, HAL_ADC_CONVERSION_COMPLETE_CB_ID, BOARD_ADC_CallBack) == HAL_OK);
}

void BOARD_SPI_Init(void)
{
   /* Bit configuration structure for GPIOA PIN4 and PIN3 */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable clock for SPI1, AFIO and GPIOA */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();
    
    /* GPIOA PIN4 alternative function MOSI */
    GPIO_InitStructure.Pin = SPI_PIN[MOSI];
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(SPI_PORT[MOSI], &GPIO_InitStructure);
    /* GPIOA PIN3 alternative function SCK */
    GPIO_InitStructure.Pin = SPI_PIN[SCK];
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(SPI_PORT[SCK], &GPIO_InitStructure);

    /* Baud rate 1000000, 
     * 
     */
    hspi.Instance = BOARD_SPI;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    hspi.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi.Init.CLKPhase          = SPI_PHASE_2EDGE;
    hspi.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    hspi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi.Init.CRCPolynomial     = 7;
    hspi.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi.Init.NSS               = SPI_NSS_SOFT;
    hspi.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
    hspi.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
    hspi.Init.Mode              = SPI_MODE_MASTER;

    assert_param(HAL_SPI_Init(&hspi) == HAL_OK);
}

void BOARD_SPI_Send(uint8_t* data, uint16_t size)
{
    assert_param(HAL_SPI_Transmit(&hspi, data, size, 100) == HAL_OK);
}
/**
 * @}
 */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 
    
/*****************************END OF FILE****/
