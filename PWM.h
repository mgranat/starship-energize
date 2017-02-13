#ifndef __PWM_H__
#define __PWM_H__

#include <stdint.h>

void PWM_Init(void);
void PWM_tick(uint32_t timer_frequency, double motor_frequency);
void set_range_factor(double new_factor);

#endif
