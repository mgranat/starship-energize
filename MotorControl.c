#include <stdint.h>
#include <math.h>

#define NOMINAL_POWER (93.21)
#define NOMINAL_VOLTAGE (230.0)
#define NOMINAL_FREQUENCY (60.0)
#define STARTUP_VOLTAGE (19.0)
#define DUTY_CYCLE_MIN (0.1)
#define DUTY_CYCLE_MAX (0.85)

double calculate_motor_voltage(double pv_power) {
	return NOMINAL_VOLTAGE * sqrt(pv_power / NOMINAL_POWER);	
}

double calculate_motor_frequency(double motor_voltage) {
	double slope = 0;
	double frequency = 0;
	if (motor_voltage < NOMINAL_VOLTAGE / 2) {
		slope = (NOMINAL_FREQUENCY / 2) / (NOMINAL_VOLTAGE / 2 - STARTUP_VOLTAGE);
		frequency = (motor_voltage - STARTUP_VOLTAGE) * slope;
	} else {
		slope = NOMINAL_FREQUENCY / NOMINAL_VOLTAGE;
		frequency = (motor_voltage - NOMINAL_VOLTAGE) * slope + NOMINAL_FREQUENCY;
	}
	
	if (frequency < 0)
		return 0;
	else
		return frequency;
}

double calculate_converter_duty(double desired_converter_voltage, double pv_voltage) {
	// V_out = V_in / (1 - D)
	// D = 1 - V_in / V_out
	// Min 0.1, max 0.85
	double duty_cycle = pv_voltage / desired_converter_voltage - 1;
	if (duty_cycle < DUTY_CYCLE_MIN)
		return DUTY_CYCLE_MIN;
	else if (duty_cycle > DUTY_CYCLE_MAX)
		return DUTY_CYCLE_MAX;
	else
		return duty_cycle;
}

double calculate_converter_voltage(double pv_voltage, double duty_cycle) {
	return pv_voltage / (1 - duty_cycle);
}
