// PWM.c
// Runs on TM4C123
// Use PB4, PB6, and PB7 to generate pulse-width modulated outputs.
// Max Granat

// PB6 PWM output A
// PB7 PWM output B
// PB4 PWM output C

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2016
   Program 6.8, section 6.3.2

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2017
   Program 8.4, Section 8.3

 Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include <math.h>
#include "PLL.h"
#include "SysTickInts.h"
#include "tm4c123gh6pm.h"

#define PI (3.141592)
#define PWM_DEAD_BAND (4)
#define PWM_PERIOD (500) // 250 is 20 kHz switching frequency
#define PWM_PADDING (PWM_DEAD_BAND + 3)
#define PWM_RANGE ((((double) PWM_PERIOD) - PWM_PADDING) / 2)
#define PWM_CENTER (PWM_RANGE + PWM_PADDING)
#define TIMER_PERIOD (10000)
#define TIMER_FREQUENCY (8000)

uint32_t pwm_counter = 0;

void PWM_Init() {
	uint16_t period = PWM_PERIOD;

	// Initialize PWM
	volatile uint32_t delay;
  SYSCTL_RCGCPWM_R |= 0x01;             // activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // activate port B
  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
  GPIO_PORTB_AFSEL_R |= 0xF0;           // enable alt funct on PB4-7
  GPIO_PORTB_PCTL_R &= ~0xFFFF0000;     // configure PB4-7 as PWM0
  GPIO_PORTB_PCTL_R |= 0x44440000;
  GPIO_PORTB_AMSEL_R &= ~0xF0;          // disable analog functionality on PB4-7
  GPIO_PORTB_DEN_R |= 0xF0;             // enable digital I/O on PB4-7

  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; // clear PWM divider field
  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16; // configure for /16 divider

	// Initialize PB6-7 on PWM0
  PWM0_0_CTL_R = 0;                     // disable PWM while initializing
  // PWM0, Generator A (PWM0/PB6) goes to 1 when count==reload and 0 when count==CMPA
  PWM0_0_GENA_R = (PWM_0_GENA_ACTLOAD_ONE|PWM_0_GENA_ACTCMPAD_ZERO);
  // PWM0, Generator B (PWM1/PB7) goes to 0 when count==reload and 1 when count==CMPA
  PWM0_0_GENB_R = (PWM_0_GENB_ACTLOAD_ZERO|PWM_0_GENB_ACTCMPAD_ONE);
	
	PWM0_0_DBCTL_R = 1;										// enable dead-band generator for PWM0
	PWM0_0_DBRISE_R = PWM_DEAD_BAND;			// configure rising edge dead-band for PWM0
	PWM0_0_DBFALL_R = PWM_DEAD_BAND;			// configure falling edge dead-band for PWM0

  PWM0_0_LOAD_R = period - 1;						// cycles needed to count down to 0
  PWM0_0_CMPA_R = (period - 1)/2;       // count value when PWM0 toggles

  PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;			// start PWM0 in count down mode
  PWM0_ENABLE_R |= (PWM_ENABLE_PWM0EN|PWM_ENABLE_PWM1EN);		// enable PWM0
	
	// Initialize PB4-5 on PWM1
  PWM0_1_CTL_R = 0;                     // disable PWM while initializing
	// PWM1, Generator A (PWM2/PB4) goes to 1 when count==reload and 0 when count==CMPA
  PWM0_1_GENA_R = (PWM_1_GENA_ACTLOAD_ONE|PWM_1_GENA_ACTCMPAD_ZERO);
	// PWM1, Generator B (PWM3/PB5) goes to 0 when count==reload and 1 when count==CMPA
	PWM0_1_GENB_R = (PWM_1_GENB_ACTLOAD_ZERO|PWM_1_GENB_ACTCMPAD_ONE);
	
	PWM0_1_DBCTL_R = 1;										// enable dead-band generator for PWM1
	PWM0_1_DBRISE_R = PWM_DEAD_BAND;			// configure rising edge dead-band for PWM1
	PWM0_1_DBFALL_R = PWM_DEAD_BAND;			// configure falling edge dead-band for PWM1

  PWM0_1_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_1_CMPA_R = (period - 1)/2;       // count value when output rises

  PWM0_1_CTL_R |= PWM_1_CTL_ENABLE;     // start PWM1
  PWM0_ENABLE_R |= (PWM_ENABLE_PWM2EN|PWM_ENABLE_PWM3EN);		// enable PWM1
	
	SYSCTL_RCGCGPIO_R |= 0x10;            // activate port E
  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
  GPIO_PORTE_AFSEL_R |= 0x30;           // enable alt funct on PE4-5
  GPIO_PORTE_PCTL_R &= ~0x00FF0000;     // configure PE4-5 as PWM0
  GPIO_PORTE_PCTL_R |= 0x00440000;
  GPIO_PORTE_AMSEL_R &= ~0x30;          // disable analog functionality on PE4-5
  GPIO_PORTE_DEN_R |= 0x30;             // enable digital I/O on PE4-5
	
	// Initialize PE4-5 on PWM2
  PWM0_2_CTL_R = 0;                     // disable PWM while initializing
	// PWM2, Generator A (PWM4/PE4) goes to 1 when count==reload and 0 when count==CMPA
  PWM0_2_GENA_R = (PWM_2_GENA_ACTLOAD_ONE|PWM_2_GENA_ACTCMPAD_ZERO);
	// PWM2, Generator B (PWM5/PE5) goes to 0 when count==reload and 1 when count==CMPA
	PWM0_2_GENB_R = (PWM_2_GENB_ACTLOAD_ZERO|PWM_2_GENB_ACTCMPAD_ONE);
	
	PWM0_2_DBCTL_R = 1;										// enable dead-band generator for PWM2
	PWM0_2_DBRISE_R = PWM_DEAD_BAND;			// configure rising edge dead-band for PWM2
	PWM0_2_DBFALL_R = PWM_DEAD_BAND;			// configure falling edge dead-band for PWM2

  PWM0_2_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_2_CMPA_R = (period - 1)/2;       // count value when output rises

  PWM0_2_CTL_R |= PWM_2_CTL_ENABLE;     // start PWM2
  PWM0_ENABLE_R |= (PWM_ENABLE_PWM4EN|PWM_ENABLE_PWM5EN);	// enable PWM2
}

void set_duty_a(uint16_t duty) {
	PWM0_0_CMPA_R = PWM_PERIOD - (duty - 1);
}

void set_duty_b(uint16_t duty){
  PWM0_1_CMPA_R = PWM_PERIOD - (duty - 1);
}

void set_duty_c(uint16_t duty){
  PWM0_2_CMPA_R = PWM_PERIOD - (duty - 1);
}

void SysTick_Handler(void){
	double frequency = 1;
	double divisor = TIMER_FREQUENCY / frequency;
	double phase = (((double) pwm_counter) / divisor) * 2 * PI;
	double offset = 2 * PI / 3;
	double sine_a = cos(phase);
	double sine_b = cos(phase + offset);
	double sine_c = cos(phase - offset);
	uint16_t duty_a = (uint16_t) (PWM_CENTER + (sine_a * PWM_RANGE));
	uint16_t duty_b = (uint16_t) (PWM_CENTER + (sine_b * PWM_RANGE));
	uint16_t duty_c = (uint16_t) (PWM_CENTER + (sine_c * PWM_RANGE));
	
	set_duty_a(duty_a);
	set_duty_b(duty_b);
	set_duty_c(duty_c);
	
	pwm_counter++;
	if (pwm_counter > TIMER_FREQUENCY) {
		pwm_counter = 0;
	}
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
