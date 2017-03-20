#define NOMINAL_POWER (93.21)
#define NOMINAL_VOLTAGE (230)
#define NOMINAL_FREQUENCY (60)
#define STARTUP_VOLTAGE (19)

#include <math.h>

double calculate_voltage(double input_power) {
	return NOMINAL_VOLTAGE * sqrt(input_power / NOMINAL_POWER);
}

double calculate_frequency(double voltage) {
	if (voltage < NOMINAL_VOLTAGE / 2) {
		double slope = (NOMINAL_FREQUENCY / 2) / (NOMINAL_VOLTAGE / 2 - STARTUP_VOLTAGE);
		return (voltage - STARTUP_VOLTAGE) * slope;
	} else {
		double slope = NOMINAL_FREQUENCY / NOMINAL_VOLTAGE;
		return (voltage - NOMINAL_VOLTAGE) * slope + NOMINAL_FREQUENCY;
	}
}

double calculate_converter_duty(double voltage) {
	return 0.001;
}
