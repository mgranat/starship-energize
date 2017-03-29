#include <stdint.h>

#include "PLL.h"
#include "PWM.h"
#include "MotorControl.h"
#include "SysTickInts.h"

#define TIMER_PERIOD_K (10)
#define TIMER_PERIOD (TIMER_PERIOD_K * 1000)
#define TIMER_FREQUENCY (80000 / TIMER_PERIOD_K)

// Motor control loop
void SysTick_Handler(void){
	// Read data from sensing
	double pv_voltage = 20;
	double pv_power = 9;
	// double meas_converter_voltage = 70; // Use to close converter voltage control loop

	// Calculate motor voltage from P-V relationship
	double motor_voltage = calculate_motor_voltage(pv_power);
	// Calculate converter voltage necessary to produce desired motor voltage
	double desired_converter_voltage = 4 * motor_voltage / 3.141592;
	// Calculate converter duty cycle necessary to produce desired converter voltage
	double converter_duty = calculate_converter_duty(desired_converter_voltage, pv_voltage);
	// Calculate actual converter voltage from given duty cycle
	double actual_converter_voltage = calculate_converter_voltage(pv_voltage, converter_duty);
	// Make adjustments if system is unable to reach desired converter voltage
	double buck_factor = 1;
	// If converter voltage is too high, buck output at three phase inverter
	if (actual_converter_voltage > desired_converter_voltage) {
		buck_factor = desired_converter_voltage / actual_converter_voltage;
	// If converter voltage is not high enough, adjust motor voltage
	} else if (actual_converter_voltage < desired_converter_voltage) {
		motor_voltage = 3.141592 * actual_converter_voltage / 4;
	}
	// Calculate motor frequency from V-f relationship
	double frequency = calculate_motor_frequency(motor_voltage);
	
	// Set output parameters
	// Set bucking of three-phase inverter if necessary
	set_range_factor(buck_factor);
	// Set converter duty cycle
	set_converter_duty(converter_duty);
	// Advance PWM phases
	PWM_tick(TIMER_FREQUENCY, frequency);
}

int main() {
	while(1) {}
}

int SystemInit() {
	PLL_Init();
	PWM_Init();
	SysTick_Init(TIMER_PERIOD);
	
	return 0;
}
