#ifndef _COMMAND_HANDLER_
#define _COMMAND_HANDLER_

#include "userdefine.h"

/** Commands handlers */
#define HANDLER_NAME(prefix) prefix##_##handler
#define HANDLER_PROTOTYPE(prefix) void HANDLER_NAME(prefix)(node_t *node, serial_data_t *serial)
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
void Handle_command(void *nodes, void *serial);

#endif
