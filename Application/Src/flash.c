#include "flash.h"
#include "userdefine.h"
#include "stm32f0xx.h"
#include <string.h>

#define USER_FLASH_PAGE_SIZE (FLASH_PAGE_SIZE)
#define FLASH_START_ADDRESS (0x08000000)
#define FLASH_PROGRAM_SIZE (60 * USER_FLASH_PAGE_SIZE)
#define USER_FLASH_START_ADDRESS (FLASH_START_ADDRESS + FLASH_PROGRAM_SIZE)

static int Erase(void);
static int Write(const uint8_t* src, uint32_t size);
static int Verify(const uint8_t* src, uint32_t size);

static uint8_t debug = 0;

flash_status_t UpdateFlash(const uint8_t* src, uint32_t size)
{
    if(Erase() != 0) {
        return FLASH_ERASE_ERROR;
    }
    if(Write(src, size) != 0) {
        return FLASH_WRITE_ERROR;
    }
    if(Verify(src, size) != 0) {
        return FLASH_VERIFY_ERROR;
    }
    return FLASH_OK;
}

static int Erase(void)
{
    int ret_val = 0;
    HAL_FLASH_Unlock(); // Открыть доступ к FLASH (она закрыта от случайной записи)
    
    FLASH_EraseInitTypeDef ef; // Объявляю структуру, необходимую для функции стирания страницы

    ef.TypeErase = FLASH_TYPEERASE_PAGES; // Стирать постранично
    ef.PageAddress = USER_FLASH_START_ADDRESS; // Адрес страницы для стирания
    ef.NbPages = 1; //Число страниц = 1
    uint32_t temp; // Временная переменная для результата стирания (не использую)
    if(HAL_FLASHEx_Erase(&ef, &temp) != HAL_OK) {  // Вызов функции стирания
        ret_val = -1;
    }
    HAL_FLASH_Lock(); // Закрыть флешку от случайной записи
    return ret_val;
}

static int Write(const uint8_t* src, uint32_t size)
{
    int ret_val = 0;
    HAL_FLASH_Unlock(); // Открыть доступ к FLASH (она закрыта от случайной записи)
    
    uint16_t* data = (uint16_t*) src;
    for (int i = 0; i < size; i += 2) { // Запись всех настроек
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, USER_FLASH_START_ADDRESS + i, *data++) != HAL_OK) {
            ret_val = -1;
            break; // Если что-то пошло не так - выскочить из цикла
        }
    }
    HAL_FLASH_Lock(); // Закрыть флешку от случайной записи
    return ret_val;
}

static int Verify(const uint8_t* src, uint32_t size)
{
    return memcmp(&params_in_flash, src, size);
}

void ReadFlash(uint8_t* dst, uint32_t size)
{
    memcpy(dst, &params_in_flash, size);
}
