#include "hall_array_library.h"

uint16_t biases[8][8];
uint16_t board_state[8][8];

void swap(char* a, char* b)
{
 int temp = *a;
 *a = *b;
 *b = temp;
}
/* A utility function to reverse a string  */
void reverse(char str[], int length){
    int start = 0;
    int end = length -1;
    while (start < end){
        swap((str+start), (str+end));
        start++;
        end--;
    }
}
// Implementation of itoa()
char* itoa(int num, char* str, int base){
    int i = 0;
    char isNegative = 0;
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0){
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10){
        isNegative = 1;
        num = -num;
    }
    // Process individual digits
    while (num != 0){
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
    str[i] = '\0'; // Append string terminator
    // Reverse the string
    reverse(str, i);
    return str;
}

unsigned char** get_board_state(){
	return 0;
}

void initialize_biases(){
	int x = 0;
	//zero out biases
	for(unsigned i = 0; i < NUM_SQUARES; i++){
		for(unsigned j = 0; j < NUM_SQUARES; j++){
			biases[i][j] = 0;
		}
	}
}

void transmit_string_usart(char* str){
	char c = str[0];
	unsigned i = 0;
	while(c != '\0'){
		transmit_char_usart(c);
		i ++;
		c = str[i];
	}
	return;
}

void transmit_char_usart(char c){
	while((USART1->ISR & USART_ISR_TXE) == 0){
		//enable timeout
	}
	USART1->TDR = c;
}
float adcval_tovolt(uint16_t adc_val){
	float val = 0;
	float temp = 3.3/4096;
	val =((float)adc_val)*temp;
	return temp;
}
void transmit_voltage_usart(uint16_t val){
	
	uint16_t mask = 0xff00;
	uint16_t tx = 0;
	uint8_t transmit = 0;
	for(unsigned i = 0; i < 2; i ++){
		tx = mask & val;
		mask = mask >> 8;
		tx  = tx >> (8 * (1 - i));
		transmit = (uint8_t) tx;
		transmit_char_usart((char)(transmit + '0'));
	}
	transmit_char_usart('\r');
	transmit_char_usart('\n');
}

uint16_t** scan_array(uint16_t board_buffer[8][8]){
	//uint16_t array_values[64][64];
	for(unsigned i = 0; i < 4; i++){
		switch (i){
			case 0:
				turn_off_all();
			break;
			case 1:
				turn_off_all();
				turn_on_a();			
			break;
			case 2:
				turn_off_all();
				turn_on_b();
			break;
			case 3:
				turn_off_all();
				turn_on_a();
				turn_on_b();
			break;
			default:
			break;
		}
		for(unsigned j = 0; j < 16; j++){
			if(j < 8){
				turn_off_f();
			}else{
				turn_on_f();
			}
			unsigned mod = j % 8;
			if(mod < 4){
				turn_off_e();
			}else{
				turn_on_e();
			}
			mod = j % 4;
			if(mod < 2){
				turn_off_d();
			}else{
				turn_on_d();
			}
			mod = j % 2;
			if(mod == 0){
				turn_off_c();
			}else{
				turn_on_d();
			}
			HAL_Delay(80);
			//uint16_t reading = take_reading();
			while((ADC1->ISR & ADC_ISR_EOC) == 0){
			//implement time-out
			}
			uint16_t reading = ADC1->DR;
			int string_int = reading & 0x0fff;
			char buffer [sizeof(uint16_t)*8+1];
			itoa(string_int,buffer,10);
			transmit_string_usart(buffer);
			transmit_char_usart(' ');
			if(j == 7){
				transmit_char_usart('\r');
				transmit_char_usart('\n');
			}
		}
		transmit_char_usart('\r');
		transmit_char_usart('\n');
	}
}
uint16_t take_reading(void){
	uint16_t result = 0;
}
void pseudo_main(void){
	
	//uint16_t biases[64][64];
//	for(unsigned i = 0; i < NUM_SQUARES; i++){
//		for(unsigned j = 0; j < NUM_SQUARES; j++){
//			biases[i][j] = 0;
//		}
//	}
	//initialize_biases();
	char str[7];
	str[0] = 'c';
	str[1] = 'h';
	str[2] = 'e';
	str[3] = 's';
	str[4] = 's';
	str[5] = '\r';
	str[6] = '\n';
	//str[7] = '\0';
	for(unsigned i = 0; i < 7; i++){
		transmit_char_usart(str[i]);
	}
	uint16_t reading = 0;
	//look at mux 0,0,0,0,0,0 - A1
	GPIOB->BSRR |= GPIO_BSRR_BR_10;
	GPIOB->BSRR |= GPIO_BSRR_BR_11;
	GPIOB->BSRR |= GPIO_BSRR_BR_12;
	GPIOB->BSRR |= GPIO_BSRR_BR_13;
	GPIOB->BSRR |= GPIO_BSRR_BR_14;
	GPIOB->BSRR |= GPIO_BSRR_BR_15;
	
	//GPIOB->BSRR |= GPIO_BSRR_BS_12;

	ADC1->CR |= ADC_CR_ADSTART;//turn on adc
  while (1)
  {
		while((ADC1->ISR & ADC_ISR_EOC) == 0){
			//implement time-out
		}
		reading = ADC1->DR;
		int string_int = reading & 0x0fff;
		char buffer [sizeof(uint16_t)*8+1];
		itoa(string_int,buffer,10);
		//utoa(reading,buffer,10);
		//transmit_string_usart(buffer);
		//transmit_char_usart('\r');
		//transmit_char_usart('\n');
		scan_array(biases);
		transmit_char_usart('\r');
		transmit_char_usart('\n');
		HAL_Delay(2000);
  }
  /* USER CODE END 3 */
}

void turn_on_a(void){
	GPIOB->BSRR |= GPIO_BSRR_BS_10;
}
void turn_on_b(void){
	GPIOB->BSRR |= GPIO_BSRR_BS_11;
}
void turn_on_c(void){
	GPIOB->BSRR |= GPIO_BSRR_BS_12;
}
void turn_on_d(void){
	GPIOB->BSRR |= GPIO_BSRR_BS_13;
}
void turn_on_e(void){
	GPIOB->BSRR |= GPIO_BSRR_BS_14;
}
void turn_on_f(void){
	GPIOB->BSRR |= GPIO_BSRR_BS_15;
}

void turn_off_a(void){
	GPIOB->BSRR |= GPIO_BSRR_BR_10;
}
void turn_off_b(void){
	GPIOB->BSRR |= GPIO_BSRR_BR_11;
}
void turn_off_c(void){
	GPIOB->BSRR |= GPIO_BSRR_BR_12;
}
void turn_off_d(void){
	GPIOB->BSRR |= GPIO_BSRR_BR_13;
}
void turn_off_e(void){
	GPIOB->BSRR |= GPIO_BSRR_BR_14;
}
void turn_off_f(void){
	GPIOB->BSRR |= GPIO_BSRR_BR_15;
}

void turn_off_all(void){
	turn_off_a();
	turn_off_b();
	turn_off_c();
	turn_off_d();
	turn_off_e();
	turn_off_f();
}