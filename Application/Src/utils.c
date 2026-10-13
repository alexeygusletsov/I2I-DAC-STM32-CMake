#include "utils.h"
#include "board.h" // BOARD_Send
#include <string.h> // memset
#include <stdio.h>

#define TX_BUFFER_SIZE 256
char tx_buffer[TX_BUFFER_SIZE];
#define TRACE_UART(...) { int len = snprintf(tx_buffer, TX_BUFFER_SIZE - 1, __VA_ARGS__); BOARD_Send(tx_buffer, len, 1000); }

void PrintAddress(uint32_t address)
{
    memset(tx_buffer, 0, sizeof(tx_buffer));
    TRACE_UART("Address: %08X\r\n", address);
}

void Dump(const void *address, size_t size)
{
    memset(tx_buffer, 0, sizeof(tx_buffer));
    int space = sizeof(tx_buffer);
    int len = snprintf(tx_buffer, space, "Dump %X (%lu):\r\n", (uint32_t)address, size);
    BOARD_Send((uint8_t*)tx_buffer, len, 1000);
    for(int i = 0; i < ((size + 3) / 4); i++) {
        len = snprintf(tx_buffer, space, "%08X ", *((const uint32_t*)address + i));
        BOARD_Send((uint8_t*)tx_buffer, len, 1000);
    }
    len = sniprintf(tx_buffer, space, "\r\n");
    BOARD_Send((uint8_t*)tx_buffer, len, 1000);
}

void PrintParam(const char *pre, const Parameters_t *param)
{
    const float k[] = {
        param->k[0] * 1000.,
        param->k[1] * 1000.,
        param->k[2] * 1000.
    };
    const float R[] = {
        (float)(param->points[0][1].mA - param->points[0][0].mA) / (param->points[0][1].adc - param->points[0][0].adc) * 1000,
        (float)(param->points[1][1].mA - param->points[1][0].mA) / (param->points[1][1].adc - param->points[1][0].adc) * 1000,
        (float)(param->points[2][1].mA - param->points[2][0].mA) / (param->points[2][1].adc - param->points[2][0].adc) * 1000
    };
    memset(tx_buffer, 0, sizeof(tx_buffer));
    TRACE_UART(
        "%s %X\r\n" \
        "Point[0][0] = %u, %x\r\n" \
        "Point[0][1] = %u, %x\r\n" \
        "Point[1][0] = %u, %x\r\n" \
        "Point[1][1] = %u, %x\r\n" \
        "Point[2][0] = %u, %x\r\n" \
        "Point[2][1] = %u, %x\r\n",
        pre, (uint32_t)param,
        param->points[0][0].mA, param->points[0][0].adc,
        param->points[0][1].mA, param->points[0][1].adc,
        param->points[1][0].mA, param->points[1][0].adc,
        param->points[1][1].mA, param->points[1][1].adc,
        param->points[2][0].mA, param->points[2][0].adc,
        param->points[2][1].mA, param->points[2][1].adc
    );
    memset(tx_buffer, 0, sizeof(tx_buffer));
    TRACE_UART(
        "Levels: %u, %u, %u, %u\r\n" \
        "Baudrate = %u\r\n" \
        "K[0] = %lu, B[0] = %hd\r\n" \
        "K[1] = %lu, B[1] = %hd\r\n" \
        "K[2] = %lu, B[2] = %hd\r\n" \
        "R[0] = %lu\r\n" "R[1] = %lu\r\n" "R[2] = %lu\r\n",
        param->levels[0], param->levels[1], param->levels[2], param->levels[3], 
        param->dac_baudrate,
        (uint32_t)k[0], param->b[0],
        (uint32_t)k[1], param->b[1],
        (uint32_t)k[2], param->b[2],
        (uint32_t)R[0], (uint32_t)R[1], (uint32_t)R[2] 
    );
}
