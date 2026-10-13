/**
  ******************************************************************************
  * @file    board.h
  * @author  Alexey Gusletsov
  * @version V1.0.0
  * @date    12-May-2026
  * @brief   This file contains definitions for Ito2I Leds, push-
  *          buttons hardware resources.
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup BOARD
  * @{
  */
      
/** @addtogroup BOARD_LOW_LEVEL
  * @{
  */ 

/** @defgroup BOARD_LOW_LEVEL_Exported_Types
  * @{
  */
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;

typedef enum
{
  CS1 = 0,
  CS2 = 1,
  CS3 = 2
} CS_TypeDef;

typedef void (*pUSER_TxCallbackTypeDef)(void);  /*!< pointer to an USART callback function */
typedef void (*pUSER_RxCallbackTypeDef)(void);  /*!< pointer to an USART callback function */
typedef void (*pUSER_ADCCallbackTypeDef)(void);

/**
  * @}
  */ 

/** @defgroup BOARD_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** @addtogroup BOARD_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4
  
#define LED1_PIN                         GPIO_PIN_7
#define LED1_GPIO_PORT                   GPIOB
  
#define LED2_PIN                         GPIO_PIN_6
#define LED2_GPIO_PORT                   GPIOB

#define LED3_PIN                         GPIO_PIN_5
#define LED3_GPIO_PORT                   GPIOB

#define LED4_PIN                         GPIO_PIN_4
#define LED4_GPIO_PORT                   GPIOB

/**
  * @}
  */   

/** @addtogroup BOARD_LOW_LEVEL_CS
  * @{
  */
#define CSn                             3
  
#define CS1_PIN                         GPIO_PIN_1
#define CS1_GPIO_PORT                   GPIOB
  
#define CS2_PIN                         GPIO_PIN_0
#define CS2_GPIO_PORT                   GPIOB

#define CS3_PIN                         GPIO_PIN_3
#define CS3_GPIO_PORT                   GPIOB

/**
  * @}
  */   

/** @addtogroup BOARD_LOW_LEVEL_USART
  * @{
  */

typedef enum {
    RX = 0,
    TX = 1
} USART_Typedef;

#define BOARD_USART                      USART1
#define BOARD_USART_BAUDRATE             9600

#define USART1n                          2
  
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_PORT                   GPIOA
  
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_PORT                   GPIOA

/**
  * @}
  */   

  /** @addtogroup BOARD_LOW_LEVEL_SPI
  * @{
  */

typedef enum {
    MOSI = 0,
    SCK = 1
} SPI_Typedef;

#define BOARD_SPI                       SPI1
#define BOARD_SPI_BAUDRATE              1000000

#define SPIn                            2
  
#define SPI_MOSI_PIN                    GPIO_PIN_7
#define SPI_MOSI_PORT                   GPIOA
  
#define SPI_SCK_PIN                     GPIO_PIN_5
#define SPI_SCK_PORT                    GPIOA

/**
  * @}
  */   

/** @defgroup BOARD_LOW_LEVEL_Exported_Macros
  * @{
  */  
/**
  * @}
  */ 

/** @defgroup BOARD_LOW_LEVEL_Exported_Functions
  * @{
  */
void BOARD_LEDInit(Led_TypeDef Led);
void BOARD_LEDOn(Led_TypeDef Led);
void BOARD_LEDOff(Led_TypeDef Led);
void BOARD_LEDToggle(Led_TypeDef Led);
void BOARD_CSInit(CS_TypeDef CS);
void BOARD_CSOn(CS_TypeDef CS);
void BOARD_CSOff(CS_TypeDef CS);
void BOARD_UART1Init(void);
void BOARD_RegisterTxCallBack(pUSER_TxCallbackTypeDef callback);
void BOARD_RegisterRxCallBack(pUSER_RxCallbackTypeDef callback);
void BOARD_Init(void);

void BOARD_Send(const uint8_t *buf, uint16_t size, uint32_t timeout);
int BOARD_Send_IT(const uint8_t *buf, uint16_t size);
int BOARD_Receive_IT(uint8_t *buf, uint16_t size);

uint16_t BOARD_ADC_Result(void);
void BOARD_ADC_Init(void);
void BOARD_ADC_Start(void);
void BOARD_RegisterADCCallBack(pUSER_ADCCallbackTypeDef callback);

void BOARD_SPI_Init(void);
void BOARD_SPI_Send(uint8_t* data, uint16_t size);
/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
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
