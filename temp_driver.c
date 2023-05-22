#include <stdio.h>

#include "platform.h"
#include "timer.h"
#include "gpio.h"

#define DHT_PIN PA_10


void end_start_signal();


int start_signal_ended = 0;

void start_signal(){
	
	start_signal_ended = 0;
	
	timer_init(CLK_FREQ * 18 / 1000); // Counts 18 ms.
	timer_set_callback(end_start_signal);
	
	gpio_set_mode(DHT_PIN, Output);
	gpio_set(DHT_PIN, 0);
	
	timer_enable();
	while(start_signal_ended); // Whait the time to end the start signal.
	
	gpio_set_mode(DHT_PIN, PullUp);	
	timer_disable();

}

void end_start_signal(){
	
	start_signal_ended = 1; // Set the flag to true.

}




int read_counter = 0;
int reading_bit = 0;
int counted_value = 0;
int new_bit_to_read = 0;

void increase_counter_isr(){
	
	if (gpio_get(DHT_PIN) != 0){
		
		reading_bit = 1;
		
		read_counter += 1;
	
	} else {
		
		if (reading_bit){
				
			counted_value = read_counter;
			
			reading_bit = 0;
			
			new_bit_to_read = 1;

			read_counter = 0;

		}
	}
}

uint64_t read_data(){
	
	uint64_t value = 0;
	
	start_signal();
	
	timer_init(CLK_FREQ / 1000000); // Counts 1 us.
	timer_set_callback(increase_counter_isr);

	int read_bits = 0;
	int bit_has_been_read = 1;
	
	while(gpio_get(DHT_PIN) == 0); // Wait for DHT to get ready.
	while(gpio_get(DHT_PIN) != 0); // Wait for DHT to get ready.
	
	timer_enable();
	
	while(read_bits < 40){
			
			
		while(!new_bit_to_read); // Wait to read new bit.
		
		if (counted_value > 45){
				
			value = (value << 1) | 1;
			
		} else {
			value = (value << 1);
		}
			
		new_bit_to_read = 0;
		
		read_bits += 1;
	}
	
	timer_disable();
	
	while(DHT_PIN == 0); // Wait the end signal.
	
	return value;
}

float get_temperature(){
	
	uint64_t data = read_data();
	
	uint8_t int_value = (data & 0xFF00) >> 8;
	uint8_t dec_value = (data & 0xFF);
	
	float temperature = (float)int_value + (float)dec_value;
	
	return temperature;
}

int main(void){
		
		__enable_irq();

}
