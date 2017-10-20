#ifndef HALL_ARRAY_LIBRARY
#define HALL_ARRAY_LIBRARY

#include <stdint.h>
#include "stm32f072xb.h"
#include "stm32f0xx_hal.h"
#include "main.h"

#define NUM_SQUARES 64

void initialize_biases(void);
unsigned char** get_board_state(void);
void transmit_char_usart(char);
void transmit_voltage_usart(uint16_t);
void pseudo_main(void);
float adcval_tovolt(uint16_t);
uint16_t ** scan_array();
#endif
