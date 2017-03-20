#include <stdint.h>

#include "PLL.h"
#include "PWM.h"
#include "MotorControl.h"
#include "SysTickInts.h"

#define TIMER_PERIOD (10000)
#define TIMER_FREQUENCY (8000)

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
	set_converter_duty(.0001);
	while(1) {}
}

int SystemInit() {
	PLL_Init();
	PWM_Init();
	SysTick_Init(TIMER_PERIOD);
	
	return 0;
}
