#include <stdio.h>
#include <timer.h>
#include <gpio.h>
#include <delay.h>
#include <uart.h>
#include <leds.h>

#define PINAKI PA_6 //put the right one here


void receive(void);
void first_call(void);
void timer_isr(void);


float temperature = 0.0;
int period = 2;
int counter = 0;


int timer_counter = 0;
int timer_mod = 2;

int new_value = 0;

int toggle_led = 0;

void timer_isr(void){
	
	timer_counter += 1;
	
	if( timer_counter % timer_mod == 0 ){
		first_call();
		new_value = 1;
	}
	
	if (toggle_led == 1){
		if(gpio_get(P_LED_R) != 0)
			leds_set(0, 0, 0);
		else
			leds_set(1, 0, 0);
	}
}


void first_call(void){
	gpio_set_mode(PINAKI,Output);
	gpio_set(PINAKI,0);
	delay_ms(18);
	gpio_set_mode(PINAKI,PullUp);  //
	
	delay_us(41);  // we could maybe delay for 41 and check if there is a response then call receive here
	
	while(gpio_get(PINAKI) == 0);
	delay_us(10);
	
	int count = 0;
	while(gpio_get(PINAKI)!= 0 && count < 8){
		delay_us(10);
		count += 1;
	}
	delay_us(10);
	
	receive();
	
}

void receive(void){
	int bits_read=0;
	uint64_t value = 0;
	
	while(bits_read < 40){
		
		while(gpio_get(PINAKI)==0); //check after 28ms if it is still high it means it is a 1 otherwise it is 0
		delay_us(35);
		
		if(gpio_get(PINAKI) != 0) { // this is 1
				value = (value << 1) | 1;
				bits_read++;
		}
		else {  // this is 0
			value = (value << 1);
			bits_read++;
		}
		
		int count = 0;
		while(gpio_get(PINAKI) != 0){
			delay_us(1);
			if (count > 60){
				break;
			}
			count += 1;
		}
	}
	
	uint8_t H_ck_int = (value & 0xFF00000000) >> 32;
	uint8_t H_ck_dec = (value & 0xFF000000) >> 24;
	uint8_t int_value = (value & 0xFF0000) >> 16;
	uint8_t dec_value = (value & 0xFF00) >> 8;
	uint8_t check_sum = (value & 0xFF);
	
	
	int temp_check = H_ck_dec + H_ck_int + int_value + dec_value;
	
	int denom = 1;
	while(denom < dec_value)
		denom = denom * 10;
	
	if(temp_check == check_sum)
		temperature = (float)int_value + ((float)dec_value / denom);
	else 
		temperature = -1.0;
}
	
int last_value = 0;
int second_value = 0;
int last_ac_value = 0;
int second_ac_value = 0;
int scan_has_ended = 0;

void uart_receive_isr(uint8_t rx){	
	
	uart_tx(rx);
	
	
	if(rx == 13){		
		
		uart_print("\r\n");
		last_ac_value = last_value - 48;
		second_ac_value = second_value - 48;
		scan_has_ended = 1;
		
	}
		
	else{
		second_value = last_value;
		last_value = rx;
	}
	
}

void button_pressed_isr(int src){
	
	if(src & GET_PIN_INDEX(P_SW)){
		counter++;
		
		if(counter==1){
			timer_mod = period;
		} else {
			if (counter % 2 == 1){
				timer_mod = 3;
			} 
			else {
				timer_mod = 4;
			}
		}
		
		char word[30];
		sprintf(word, "\r\ncounter times: %d", counter);
		uart_print(word);
	}
	
}


int main(void){

	
	uart_init(115200);
	uart_enable();
	
	leds_init();
	leds_set(0, 0, 0);
	
	uart_print("\r\n Give AEM: \n\r");
	uart_set_rx_callback(uart_receive_isr);
	while(scan_has_ended == 0);
	period = last_ac_value + second_ac_value;
	if (period == 2)
		period = 4;
	
	gpio_set_mode(P_SW, PullUp);
	gpio_set_trigger(P_SW, Rising);
	gpio_set_callback(P_SW, button_pressed_isr );	
	
	
	timer_init(1000000);
	timer_enable();
	timer_set_callback(timer_isr);
	__enable_irq();
	
	char word[40];
	while(1){
		__WFI();
		if(new_value == 1){
			sprintf(word, "\r\n temp is: %f, f is: %d", temperature, timer_mod);
			uart_print(word);		
			if(temperature > 30){
				leds_set(1, 0, 0);
				toggle_led = 0;
			}
			else if (temperature < 20){
				leds_set(0, 0, 0);
				toggle_led = 0;
			}
			else
				toggle_led = 1;
				
			new_value = 0;
		}
	}
	
	
	return 0;
}
