#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

double calculate_motor_voltage(double pv_power);
double calculate_motor_frequency(double motor_voltage);
double calculate_converter_duty(double desired_converter_voltage, double pv_voltage);
double calculate_converter_voltage(double pv_voltage, double duty_cycle);

#endif
