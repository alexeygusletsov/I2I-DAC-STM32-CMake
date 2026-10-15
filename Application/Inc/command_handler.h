#ifndef _COMMAND_HANDLER_
#define _COMMAND_HANDLER_

#include "userdefine.h"

typedef int(*sender_t)(uint8_t*, size_t);

int Parse(uint8_t* buf, size_t pos);

/** Commands handlers */
#define HANDLER_NAME(prefix) prefix##_##handler
#define HANDLER_PROTOTYPE(prefix) void HANDLER_NAME(prefix)(node_t *node, const uint8_t* rx_buffer, uint8_t* tx_buffer)
HANDLER_PROTOTYPE(help);
HANDLER_PROTOTYPE(version);
HANDLER_PROTOTYPE(name);
HANDLER_PROTOTYPE(read_adc);
HANDLER_PROTOTYPE(read_current);
HANDLER_PROTOTYPE(update);
HANDLER_PROTOTYPE(dac);
HANDLER_PROTOTYPE(point);
HANDLER_PROTOTYPE(level);
HANDLER_PROTOTYPE(reset);
HANDLER_PROTOTYPE(speed);
HANDLER_PROTOTYPE(test);
void Handle_command(void *nodes, const uint8_t* rx_buffer, sender_t sender);

#endif
