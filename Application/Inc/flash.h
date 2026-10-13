#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>

typedef enum {
    FLASH_OK,
    FLASH_ERASE_ERROR = -1,
    FLASH_WRITE_ERROR = -2,
    FLASH_VERIFY_ERROR = -3,
    FLASH_YA_ERROR = -4
} flash_status_t;

flash_status_t UpdateFlash(const uint8_t* src, uint32_t size);
void ReadFlash(uint8_t* dst, uint32_t size);

#endif