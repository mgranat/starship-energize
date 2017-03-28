#include <stdint.h>
#include <stdio.h>

#include "PLL.h"
#include "PWM.h"
#include "MotorControl.h"
#include "SysTickInts.h"
#include "ST7735.h"
#include "Sensing.h"

#define TIMER_PERIOD (10000)
#define TIMER_FREQUENCY (8000)  

void Delay10ms(uint32_t count){
	uint32_t volatile time;
  while(count>0){
    time = 72724;  // 0.01sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

// Called at TIMER_FREQUENCY
void SysTick_Handler(void){
	// Motor Control
	//double input_power = 20;																		// Read input power from sensing
	//double voltage = calculate_voltage(input_power);						// Calculate V from P-V relationship
	//double frequency = calculate_frequency(voltage);						// Calculate V
	//double converter_duty = calculate_converter_duty(voltage);	// Calculate converter duty cycle
	//set_converter_duty(converter_duty);													// Set converter duty cycle
	PWM_tick(TIMER_FREQUENCY, 19.09);												// Advance PWM phases
}

int main() {
	int ADCdata[9] = {0} ;
	//set_converter_duty(.0001);
	ADC_Print(ADCdata, 1); //setup the LCD for ADC raw data print
	
	while(1) {
		ADC_In(ADCdata);
		ADC_Print(ADCdata, 0);
		Delay10ms(25);
	}
}

int SystemInit() {
	PLL_Init();
	ADC_Init();
	ST7735_InitR(INITR_REDTAB);
	
	//PWM_Init();
	//SysTick_Init(TIMER_PERIOD); //-- this breaks my LCD printing out ever. Cant output more than 10 pixels before it crashes. Doesn't ever return. Possible Hard Fault
								//LCD seems to work otherwise
	
	
	return 0;
}

