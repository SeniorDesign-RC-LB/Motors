#include "tm4c123gh6pm.h"
#include "MotorSetup.h"
#include "MotorControl.h"
#include "PLL.h"
#include "LED.h"
#include "Switches.h"
#include "SPI.h"

#define LEFTPOWER	0.50*PERIOD
#define RIGHTPOWER	0.50*PERIOD

// basic functions defined at end of startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void WaitForInterrupt(void);  // low power mode
extern void EnableInterrupts(void);  // Enable interrupts

void GPIOPortF_Handler(void);
void delay_1ms(void);
void system_init();
unsigned char receivedData;
unsigned char transmitData;

int main(void) {
	DisableInterrupts();
	system_init();
	EnableInterrupts();
	while(1) {
		SPI_receive(&receivedData);
		switch(receivedData){
			case 'W':
				pwm_duty(LEFTPOWER, RIGHTPOWER);
				move_forward();
				transmitData = 'W';
				break;
			case 'S':
				pwm_duty(LEFTPOWER, RIGHTPOWER);
				move_backward();
				transmitData = 'S';
				break;
			case 'A':
				pwm_duty(LEFTPOWER, RIGHTPOWER);
				turn_left();
				transmitData = 'A';
				break;
			case 'D':
				pwm_duty(LEFTPOWER, RIGHTPOWER);
				turn_right();
				transmitData = 'D';
				break;
			case 'P':
				pwm_duty(LEFTPOWER, RIGHTPOWER);
				pivot_right();
				transmitData = 'P';
				break;
			case 'O':
				pwm_duty(LEFTPOWER, RIGHTPOWER);
				pivot_left();
				transmitData = 'O';
				break;			
			case 'Q':
				stop_motors();
				transmitData = 'Q';
				break;
			default:
				stop_motors();
				break;
		}
		SPI_transmit(transmitData);
	}
}

//System Initializations
void system_init() {
	motors_init();
	PLL_Init();
	led_init();
	SPI_slave_init();
	switch_init();
}

// Switch Interrupt Handler
void GPIOPortF_Handler(void) { 
	if(GPIO_PORTF_RIS_R&SW2) {	// SW2 pressed
    	GPIO_PORTF_ICR_R = SW2;	// acknowledge flag
		pwm_duty(LEFTPOWER, RIGHTPOWER);
		stop_motors();			// Emergency stop
		delay_1ms();
	}

	if(GPIO_PORTF_RIS_R&SW1) {	// SW1 pressed
    	GPIO_PORTF_ICR_R = SW1;	// acknowledge flag
		pwm_duty(LEFTPOWER, RIGHTPOWER);
		move_forward();
		delay_1ms();
	}
}

// 1ms Delay
void delay_1ms(void){
	unsigned long volatile time;
	time = 727240*100/91;  // 0.01sec
	while(time){
		time--;
	}
}

