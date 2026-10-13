#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "command_handler.h"
#include "flash.h"
#include "version.h"

const char* version = GIT_DESCRIBE;
const char* name = DEVICE_NAME;
// const char* invalid = "Invalid command: %s\r";
const char* invalid = "Invalid command: ";
const char* help = "i - read current in [ mA x SCALE ]\r"
                   "x - read ADC output\r"
		           "z n - read DACn input\r"
		           "s n - set DAC speed\r"
                   "g - green level\r"
                   "o - orange level\r"
                   "r - red level\r"
                   "w - warning level\r"
                   "v - show version\r"
		           "n - show name\r"
                   "u - update flash\r"
                   "h - print help\r";

void Handle_command(void *p1, void *p2)
{
	node_t *nodes = (node_t*)p1;
	serial_data_t *serial = (serial_data_t*)p2;

	while(nodes->command_code != NULL_CMD) {
		if(serial->rx_buffer[0] == nodes->command_code) {
			if(nodes->handler) {
				nodes->handler(nodes, serial);
			}
			return;
		}
		++nodes;
	}
	// Command not found
	snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, invalid, serial->rx_buffer);
	serial->tx_count = strlen((char*)serial->tx_buffer);
}

void help_handler(node_t *node, serial_data_t *serial)
{
	snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%s\r\n", help);
	serial->tx_count = strlen((char*)serial->tx_buffer);
}

void version_handler(node_t *node, serial_data_t *serial)
{
	snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%s\r\n", version);
	serial->tx_count = strlen((char*)serial->tx_buffer);
}

void name_handler(node_t *node, serial_data_t *serial)
{
    snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%s\r\n", name);
	serial->tx_count = strlen((char*)serial->tx_buffer);
}

void read_adc_handler(node_t *node, serial_data_t *serial)
{
	uint16_t value = *((uint16_t*)node->param_1);
	snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%04X\r\n", value);
	serial->tx_count = strlen((char*)serial->tx_buffer);
}

void read_current_handler(node_t *node, serial_data_t *serial)
{
	uint16_t value = ADC2mA(*((uint16_t*)node->param_1));
	serial->tx_count = snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%hu\r", value);
}

void update_handler(node_t *node, serial_data_t *serial)
{
	uint16_t value; // channel number
	const uint8_t number_expexted = 2;
	if(sscanf((char*)serial->rx_buffer, "%c %hu", &serial->ch, &value) == number_expexted) {
		if(value < USER_DAC_NUMBER) {
			Parameters_t *params = (Parameters_t*)node->param_1;
			float R = (params->points[value][1].mA - params->points[value][0].mA) / (params->points[value][1].adc - params->points[value][0].adc);
			params->k[value] = (float)(1600) / (params->points[value][1].mA - params->points[value][0].mA);
			params->b[value] = (float)(400 / R) - params->points[value][0].adc * params->k[value];
			flash_status_t status = UpdateFlash((const uint8_t*)params, sizeof(Parameters_t));
			if(status != FLASH_OK) {
				snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "Writing data flash failed (%d)\r", status);
			} else {
				snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "Done\r");
			}
			serial->tx_count = strlen((char*)serial->tx_buffer);
		}
	}
}

void dac_handler(node_t *node, serial_data_t *serial)
{
	uint16_t value;
	const uint8_t number_expected = 2;
	if(sscanf((char*)serial->rx_buffer, "%c %hu", &serial->ch, &value) == number_expected) {
		if(value < USER_DAC_NUMBER) {
			value = ((uint16_t*)node->param_1)[value];
			serial->tx_count = snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%04X\r", value);
		}
	}
}

void point_handler(node_t *node, serial_data_t *serial)
{
	uint16_t value;
	const uint8_t number_expected = 2;
	Parameters_t *params = (Parameters_t*)node->param_1;
	uint16_t *bufferADC = (uint16_t*)node->param_2;
	int result = sscanf((char*)serial->rx_buffer, "%c %hu", &serial->ch, &value);
	uint8_t i = SYMBOL_TO_CHANNEL(serial->ch);
	uint8_t j = SYMBOL_TO_POINT(serial->ch);
	if(i >= USER_DAC_NUMBER || j >= MEASURES_NUMBER) {
		serial->tx_count = snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, invalid, serial->rx_buffer);
		return;
	}
	if(result == number_expected) {
		/*
		 *  Symbols map:
		 *  a - measures[0][0]
		 *  b - measures[0][1]
		 *  c - measures[1][0]
		 *  d - measures[1][1]
		 *  e - measures[2][0]
		 *  f - measures[2][1]
		 */
		params->points[i][j].mA = value;
		params->points[i][j].adc = *bufferADC;
		snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "Enter the command 'u' to update\r");
	} else {
		value = params->points[i][j].mA;
		snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%hu\r", value);
	}
	serial->tx_count = strlen((char*)serial->tx_buffer);
}

void level_handler(node_t *node, serial_data_t *serial)
{
	uint16_t value;
	Parameters_t *params = (Parameters_t*)node->param_1;
	levels_t level = Get_Level(serial->rx_buffer[0]);
	if(sscanf((char*)serial->rx_buffer, "%c %hu", &serial->ch, &value) == 2) {
		uint16_t levels[LEVELS_NUMBER];
		memcpy(&levels, &params->levels, sizeof(levels));
		levels[(int)level] = value;
		if(Is_Sorted(levels, sizeof(levels)/sizeof(uint16_t))) {
			params->levels[(int)level] = value;
			snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "Enter the command 'u' to update\r");
		} else {
			snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "Invalid argument\r");
		}
	} else {
		value = params->levels[(int)level];
		snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "%hu\r", value);
	}
	serial->tx_count = strlen((char*)serial->tx_buffer);
}

void reset_handler(node_t *node, serial_data_t *serial)
{
	Parameters_t *params = (Parameters_t*)node->param_1;
	CalibrationReset(params);
	for(int i = 0; i < USER_DAC_NUMBER; i++) {
		InitDAC(i, &(params->points[i][0]));
	}
	serial->tx_count = snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "Enter the command 'u' to update\r");
}

void test_handler(node_t *node, serial_data_t *serial)
{
	snprintf((char*)serial->tx_buffer, TX_BUFFER_SIZE, "Test\r");
	serial->tx_count = strlen((char*)serial->tx_buffer);
}
