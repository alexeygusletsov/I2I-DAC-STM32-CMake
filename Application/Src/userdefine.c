#include "userdefine.h"
#include "board.h"
#include "string.h"

#define DEFAULT_DAC_BAUDRATE ((uint16_t)9600)

#define SCALE 100

#define CURRENT_mA_MAX (20*SCALE) // 20 mA
#define CURRENT_mA_MIN (4*SCALE) // 4 mA

#define DEFAULT_GREEN_LEVEL   (uint16_t)(4.0 * SCALE)
#define DEFAULT_ORANGE_LEVEL  (uint16_t)(9.3 * SCALE)
#define DEFAULT_RED_LEVEL     (uint16_t)(14.6 * SCALE)
#define DEFAULT_WARNING_LEVEL (uint16_t)(22.0 * SCALE)

#define ADC_MIN 0
#define ADC_MAX 0x3FF

#define DEFAULT_K ((float)1.0f)
#define DEFAULT_B ((int16_t)0)

const CS_TypeDef USER_DAC_TO_CS[USER_DAC_NUMBER] = {CS1, CS2, CS3};

const Parameters_t default_params = {
		.points = {
			{ { CURRENT_mA_MIN, ADC_MIN }, { CURRENT_mA_MAX, ADC_MAX } },
			{ { CURRENT_mA_MIN, ADC_MIN }, { CURRENT_mA_MAX, ADC_MAX } },
			{ { CURRENT_mA_MIN, ADC_MIN }, { CURRENT_mA_MAX, ADC_MAX } }
		},
		.levels = { DEFAULT_GREEN_LEVEL, DEFAULT_ORANGE_LEVEL, DEFAULT_RED_LEVEL, DEFAULT_WARNING_LEVEL },
		.dac_baudrate = DEFAULT_DAC_BAUDRATE,
		.k = {0, DEFAULT_K, 0},
		.b = {DEFAULT_B, DEFAULT_B, DEFAULT_B}
};

const Parameters_t params_in_flash __attribute__((__section__(".user_data"))) = {
		.points = {
			{ { CURRENT_mA_MIN, ADC_MIN }, { CURRENT_mA_MAX, ADC_MAX } },
			{ { CURRENT_mA_MIN, ADC_MIN }, { CURRENT_mA_MAX, ADC_MAX } },
			{ { CURRENT_mA_MIN, ADC_MIN }, { CURRENT_mA_MAX, ADC_MAX } }
		},
		.levels = { DEFAULT_GREEN_LEVEL, DEFAULT_ORANGE_LEVEL, DEFAULT_RED_LEVEL, DEFAULT_WARNING_LEVEL },
		.dac_baudrate = DEFAULT_DAC_BAUDRATE,
		.k = {0, DEFAULT_K, 0},
		.b = {DEFAULT_B, DEFAULT_B, DEFAULT_B}
};

const Coef_t coefs_in_flash __attribute__((__section__(".user_data"))) = {
	.a = 1,
	.b = 0
};

static DAC_t dacs[USER_DAC_NUMBER];

/*   Correction
 *   y = (x - ADC_4_mA) * k2 + b2
 */
typedef struct _coefficients_ {
	float k1;
	float b1;
	float k2;
	uint16_t b2;
} coeff_t;

const coeff_t default_coefficients = { 1., CURRENT_mA_MIN, 1., 0 };

static coeff_t coefficients[USER_DAC_NUMBER] = {
		{ 1., CURRENT_mA_MIN, 1., 0 },
		{ 1., CURRENT_mA_MIN, 1., 0 },
		{ 1., CURRENT_mA_MIN, 1., 0 }
};

float Init_K1(User_DAC_Num_t n)
{
	float ret = 1;
	if(dacs[n].measures[1].mA <= dacs[n].measures[0].mA) {
		// invalid operand
	} else if(dacs[n].measures[1].adc <= dacs[n].measures[0].adc) {
		// invalid operand
	} else {
		ret = dacs[n].measures[1].mA - dacs[n].measures[0].mA;
		ret /= SCALE;
		ret /= (dacs[n].measures[1].adc - dacs[n].measures[0].adc);
	}
	return ret;
}

float Init_B1(User_DAC_Num_t n,float k1)
{
	return (float) dacs[n].measures[1].mA / SCALE - k1 * dacs[n].measures[1].adc;
}

uint16_t Init_B2(float b1, float k1)
{
	return (float)( (float)CURRENT_mA_MIN / SCALE - b1) / k1;
}

float Init_K2(User_DAC_Num_t n)
{
	float ret = 1;
	if(dacs[n].measures[1].mA <= dacs[n].measures[0].mA) {
		// invalid operand
	} else {
		ret = (float) (CURRENT_mA_MAX - CURRENT_mA_MIN) / (dacs[n].measures[1].mA - dacs[n].measures[0].mA);
	}
	return ret;
}

void InitCoefs(User_DAC_Num_t n)
{
	memset(&coefficients[n], 0, sizeof(coeff_t));
	coefficients[n].k1 = Init_K1(n);
	coefficients[n].b1 = Init_B1(n, coefficients[n].k1);
	coefficients[n].b2 = Init_B2(coefficients[n].b1, coefficients[n].k1);
	coefficients[n].k2 = Init_K2(n);
}

void InitDAC(User_DAC_Num_t n, Pair_t *m)
{
	DisableDAC(n);
	// dacs[n].measures = m;
}

void EnableDAC(User_DAC_Num_t n)
{
	if(n < USER_DAC_NUMBER) {
		BOARD_CSOn(USER_DAC_TO_CS[n]);
	}
}

void DisableDAC(User_DAC_Num_t n)
{
	if(n < USER_DAC_NUMBER) {
		BOARD_CSOff(USER_DAC_TO_CS[n]);
	}
}

void LED_Service(uint16_t current, const uint16_t *levels)
{
	LED_Alarm(GetAlarmState(current, levels[WARN]));

	LED_Switch(GetLED(current, levels));
}

led_state_t GetAlarmState(uint16_t current, uint16_t level)
{
	if(current >= level) {
		return ON;
	} else {
		return OFF;
	}
}

led_t GetLED(uint16_t current, const uint16_t *levels)
{
	if(current >= levels[LEVEL3]) {
		return RED;
	} else if(current >= levels[LEVEL2] ) {
		return ORANGE;
	} else if(current >= levels[LEVEL1]) {
		return GREEN;
	}
	return NONE;
}

void LED_Alarm(led_state_t state)
{
	if(state == ON) {
        BOARD_LEDOn(LED3);
    } else {
        BOARD_LEDOff(LED3);
    }
}

void LED_Switch(led_t led) {
	switch(led) {
	case NONE:
	    BOARD_LEDOff(LED1);
	    BOARD_LEDOff(LED2);
		break;
	case RED:
	    BOARD_LEDOff(LED1);
	    BOARD_LEDOn(LED2);
		break;
	case GREEN:
		BOARD_LEDOn(LED1);
		BOARD_LEDOff(LED2);
		break;
	case ORANGE:
		BOARD_LEDOn(LED1);
		BOARD_LEDOn(LED2);
		break;
	default:
		break;
	}
}

uint16_t DAC_Write(User_DAC_Num_t n, uint16_t data)
{
	uint8_t buffer[2];
	uint16_t ret = data;

	data ^= -1; // inverse bits
	buffer[0] = 0xC0; // set normal mode
	buffer[0] |= data >> 4; // take high 6 bits because data has only 10 bits
	buffer[1] = (data << 4) | 0x0F; // take low 4 bits so that they take positions form 7 to 4
	EnableDAC(n);
	BOARD_SPI_Send(buffer, sizeof(buffer));
	DisableDAC(n);
	
	return ret;
}

uint16_t DAC_Tune(uint16_t in, float k, int16_t b)
{
	float ret = (float)in * k + b;
    return (uint16_t)ret;
}

uint16_t ADC2mA(uint16_t adc)
{
	int8_t sign = 0;
	int32_t res = 0;
	float k = (float)16 / ( dacs[0].measures[1].adc - dacs[0].measures[0].adc );
	float mA;
	if(adc == dacs[0].measures[0].adc)
		return (4*SCALE);
	if(adc < dacs[0].measures[0].adc) {
		sign = 1;
		mA = dacs[0].measures[0].adc - adc;
	}
	else {
		mA = adc - dacs[0].measures[0].adc;
	}
	mA *= k;
	if(sign) {
		mA = 4 - mA;
	}
	else {
		mA += 4;
	}
	mA *= SCALE;
	res = mA;
	return res > 0 ? (uint16_t)res : 0;
}

uint16_t UserWriteBuffer(volatile uint8_t *dst, uint16_t size, const char *src)
{
	uint16_t ret_val = 0;
	while(*src && (ret_val < size)) {
		*dst++ = *((uint8_t*)src++);
		ret_val++;
	}
	*dst = '\0';
	return ret_val;
}

/** @fn CalibrationReset()
 *
 *  Resets all the coefficients
 */
void CalibrationReset(Parameters_t *params)
{
	if(!params)
		return;

	for(int i = 0; i < USER_DAC_NUMBER; i++) {
		coefficients[i] = default_coefficients;
	}

	*params = default_params;
}


levels_t Get_Level(char ch)
{
	levels_t ret_val = WARN;

	switch(ch) {
	case GREEN_LEVEL_SYMBOL:
		ret_val = LEVEL1;
		break;
	case ORANGE_LEVEL_SYMBOL:
		ret_val = LEVEL2;
		break;
	case RED_LEVEL_SYMBOL:
		ret_val = LEVEL3;
		break;
	default:
		break;
	}
	return ret_val;
}

uint8_t Is_Sorted(const uint16_t *level, size_t size)
{
	uint8_t ret_val = 1;
	for(size_t i = 0; i < size - 1; i++) {
		if(level[i] >= level[i + 1]) {
			ret_val = 0;
			break;
		}
	}
	return ret_val;
}
