#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

double calculate_voltage(double input_power);
double calculate_frequency(double voltage);
double calculate_converter_duty(double voltage);

#endif
