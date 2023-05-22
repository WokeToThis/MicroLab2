#include <stdio.h>
#include <timer.h>
#include <gpio.h>
#include <delay.h>

#define PINAKI PC_10 //put the right one here

uint64_t receive(void);
uint64_t first_call(void);
void timer_isr(void);

void timer_isr(void){
	
	//basically this should start communicating with the sensor
	//this assumes the status is at high level vcc
	
	//first pull down voltage at least 18 ms
	
	//now pull back again for 20-40us-> this should tell the sensor to reply 
	
	//sensor should pull down for 80us and and pull up for 80us and then the data should start
	
	//sensor will pull down for 50 us to announce that will send 1-bit data
	
	//0 is a 26-28us pulse
	//1 is a 70us pulse 
	
	//at the end it will pull down for 50 us
	
	first_call();
	
	
}


uint64_t first_call(void){
	gpio_set_mode(PINAKI,Output);
	gpio_set(PINAKI,0);
	delay_ms(18);
	gpio_set_mode(PINAKI,PullUp);  //
	
	delay_us(41);  // we could maybe delay for 41 and check if there is a response then call receive here
	
	while(gpio_get(PINAKI)==0);
	delay_us(10);
	
	while(gpio_get(PINAKI)==1);
	delay_us(10);
	
	uint64_t data = receive();
	return data;
//	if(!gpio_get(PINAKI)){
//		delay_us(80);
//		receive();
//	}
	
}

uint64_t receive(void){
	int bits_read=0;
	uint64_t value = 0;
	while(bits_read < 40){
		
		while(gpio_get(PINAKI)==0); //check after 28ms if it is still high it means it is a 1 otherwise it is 0
		delay_us(35);
		
		if(gpio_get(PINAKI) == 1) { // this is 1
				value = (value << 1) | 1;
				bits_read++;
		}
		else {  // this is 0
			value = (value << 1);
			bits_read++;
		}
		while(gpio_get(PINAKI)==1); //wait for next 
	}
}
	
	



int main(void){

	timer_init(1000000);
	timer_enable();
	
	timer_set_callback(timer_isr);
	__enable_irq();
	
	while(1){
		__WFI();
	}
	
	
	return 0;
}
