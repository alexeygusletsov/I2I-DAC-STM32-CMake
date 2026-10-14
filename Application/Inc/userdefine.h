#ifndef _USERDEFINE_H_
#define _USERDEFINE_H_

#include <stdint.h>
#include <stddef.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 128
typedef struct _serial_data_
{
	uint8_t tx_buffer[TX_BUFFER_SIZE];
	uint8_t rx_buffer[RX_BUFFER_SIZE];
	uint16_t rx_count;
	uint16_t tx_count;
	char ch;
} serial_data_t;

typedef struct _flags_
{
	volatile uint8_t rx_flag;
	volatile uint8_t overrun;
	volatile uint8_t ADC_Ready;
	volatile uint8_t SPI_Ready;
	volatile uint8_t Timer100ms_Ready;
	volatile uint8_t tx_ready;
} flags_t;

typedef enum {
	NONE = 0, RED, GREEN, ORANGE
} led_t;

typedef enum {
	OFF = 0, ON
} led_state_t;

typedef struct _pair_ {
	uint16_t mA; // measured current in SCALE*milli-amp
	uint16_t adc; // measured adc value
} __attribute__((__aligned__(4))) Pair_t;

#define MEASURES_NUMBER 2
typedef struct _dac_ {
	const Pair_t *measures;
} DAC_t;

typedef enum {
	USER_DAC1 = 0, USER_DAC2, USER_DAC3, USER_DAC_NUMBER
} User_DAC_Num_t;

typedef enum {
	LEVEL1 = 0, LEVEL2, LEVEL3, WARN, LEVELS_NUMBER
} levels_t;

typedef struct _params_ {
	Pair_t points[USER_DAC_NUMBER][MEASURES_NUMBER];
	uint16_t levels[LEVELS_NUMBER];
	uint16_t dac_baudrate;
	float k[USER_DAC_NUMBER];
	int16_t b[USER_DAC_NUMBER];
} __attribute__((__aligned__(4))) Parameters_t;

extern const Parameters_t params_in_flash __attribute__((__section__(".user_data")));

typedef struct _coefs_ {
	uint32_t a;
	uint32_t b;
} __attribute__((__aligned__(4))) Coef_t;

extern const Coef_t coefs_in_flash __attribute__((__section__(".user_data")));

/**
 *  Commands
 */
#define HELP_CMD    'h' // print help
#define VERSION_CMD 'v' // show version
#define NAME_CMD    'n' // show name
#define ADC_CMD     'x' // read ADC output
#define CURRENT_CMD 'i' // read current
#define DAC_CMD     'z' // read DAC input
#define SPEED_CMD   's' // set DAC speed
#define GREEN_CMD   'g' // green level
#define ORANGE_CMD  'o' // orange level
#define RED_CMD     'r' // red level
#define WARNING_CMD 'w' // warning level
#define UPDATE_CMD  'u' // update flash
#define POINT_A_CMD 'a' // point a
#define POINT_B_CMD 'b' // point b
#define POINT_C_CMD 'c' // point c
#define POINT_D_CMD 'd' // point d
#define POINT_E_CMD 'e' // point e
#define POINT_F_CMD 'f' // point f
#define RESET_CMD   'q' // reset calibration
#define TEST_CMD    't' // test
#define NULL_CMD    '\0' // end of nodes list

#define GREEN_LEVEL_SYMBOL  'g'
#define ORANGE_LEVEL_SYMBOL 'o'
#define RED_LEVEL_SYMBOL    'r'
#define WARN_LEVEL_SYMBOL   'w'
#define IS_LEVEL_SYMBOL(ch) ( ch == GREEN_LEVEL_SYMBOL  ? 1 : \
                            ( ch == ORANGE_LEVEL_SYMBOL ? 1 : \
                            ( ch == RED_LEVEL_SYMBOL    ? 1 : \
                            ( ch == WARN_LEVEL_SYMBOL   ? 1 : 0 ) ) ) )

#define IS_POINT_SYMBOL(ch) ((ch >= 'a') && (ch <= 'f'))
#define SYMBOL_TO_CHANNEL(ch) ((ch - 'a') / 2)
#define SYMBOL_TO_POINT(ch)   ((ch - 'a') % 2)

typedef void(*f_t)(void*, const uint8_t*, uint8_t*);
typedef void* param_t;
typedef struct _node_ {
	uint8_t command_code;
	void *param_1;
	void *param_2;
	f_t handler;
} node_t;

void LED_Switch(led_t led);
void LED_Alarm(led_state_t state);
void LED_Service(uint16_t data, const uint16_t *levels);
uint16_t DAC_Write(User_DAC_Num_t n, uint16_t data);
uint16_t DAC_Tune(uint16_t in, float k, int16_t b);
void InitCoefs(User_DAC_Num_t n);
void EnableDAC(User_DAC_Num_t dac);
void DisableDAC(User_DAC_Num_t dac);
typedef Pair_t pair_arr_t[USER_DAC_NUMBER][MEASURES_NUMBER];
void InitDACs(pair_arr_t *m);
void InitDAC(User_DAC_Num_t n, Pair_t *m);
uint16_t ADC2mA(uint16_t adc);
void CalibrationReset(Parameters_t *params);
levels_t Get_Level(char ch);
uint8_t Is_Sorted(const uint16_t *level, size_t size);
uint16_t UserWriteBuffer(volatile uint8_t *dst, uint16_t size, const char *src);
led_state_t GetAlarmState(uint16_t current, uint16_t level);
led_t GetLED(uint16_t current, const uint16_t *levels);

#endif
