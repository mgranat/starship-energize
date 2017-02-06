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
#define PWM_SWITCHING_FREQUENCY (20000)
#define PWM_PERIOD (250)
#define PWM_RANGE ((((double) PWM_PERIOD) - 3) / 2)
#define PWM_CENTER (PWM_RANGE + 3)
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
  GPIO_PORTB_AFSEL_R |= 0xD0;           // enable alt funct on PB4, PB6, PB7
  GPIO_PORTB_PCTL_R &= ~0xFF0F0000;     // configure PB4, PB6, PB7 as PWM0
  GPIO_PORTB_PCTL_R |= 0x44040000;
  GPIO_PORTB_AMSEL_R &= ~0xD0;          // disable analog functionality on PB4, PB6, PB7
  GPIO_PORTB_DEN_R |= 0xD0;             // enable digital I/O on PB7-6
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; // clear PWM divider field
  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_16; // configure for /16 divider

	// Initialize PB6 and PB7 on PWM0
  PWM0_0_CTL_R = 0;                     // disable PWM while initializing
  //PWM0, Generator A (PWM0/PB6) goes to 0 when count==reload and 1 when count==0
  PWM0_0_GENA_R = (PWM_0_GENA_ACTLOAD_ONE|PWM_0_GENA_ACTCMPAD_ZERO);
  //PWM0, Generator B (PWM1/PB7) goes to 0 when count==CMPA counting down and 1 when count==CMPA counting up
  PWM0_0_GENB_R = (PWM_0_GENB_ACTLOAD_ONE|PWM_0_GENB_ACTCMPBD_ZERO);

  PWM0_0_LOAD_R = period - 1;						// cycles needed to count down to 0
  PWM0_0_CMPA_R = (period - 1)/2;       // count value when PWM1/PB7 toggles
	PWM0_0_CMPB_R = (period - 1)/2;				// count value when PWM0/PB6 toggles

  PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;			// start PWM0 in count down mode
  PWM0_ENABLE_R |= (PWM_ENABLE_PWM1EN|PWM_ENABLE_PWM0EN); // enable PWM1-0
	
	// Initialize PB4 on PWM1
  PWM0_1_CTL_R = 0;                     // re-loading down-counting mode
	// high on LOAD, low on CMPA down
  PWM0_1_GENA_R = (PWM_0_GENA_ACTLOAD_ONE|PWM_0_GENA_ACTCMPAD_ZERO);

  PWM0_1_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_1_CMPA_R = (period - 1)/2;       // count value when output rises

  PWM0_1_CTL_R |= 0x00000001;           // start PWM0
  PWM0_ENABLE_R |= PWM_ENABLE_PWM2EN;		// enable PB4/M0PWM1A is PWM2
}

void set_duty_a(uint16_t duty) {
	PWM0_0_CMPB_R = PWM_PERIOD - (duty - 1);
}

void set_duty_b(uint16_t duty) {
	PWM0_0_CMPA_R = PWM_PERIOD - (duty - 1);
}

void set_duty_c(uint16_t duty){
  PWM0_1_CMPA_R = PWM_PERIOD - (duty - 1);
}

void SysTick_Handler(void){
	double frequency = 60;
	double divisor = TIMER_FREQUENCY / frequency;
	double phase = (((double) pwm_counter) / divisor) * 2 * PI;
	double sine = cos(phase);
	uint16_t duty_a = (uint16_t) (PWM_CENTER + (sine * PWM_RANGE));
	
	set_duty_a(duty_a);
	
	pwm_counter++;
	if (pwm_counter > TIMER_FREQUENCY) {
		pwm_counter = 0;
	}
}

int main() {
	set_duty_a(200);
	set_duty_b(100);
	set_duty_c(10);
	while(1) {}
}

int SystemInit() {
	PLL_Init();
	PWM_Init();
	SysTick_Init(TIMER_PERIOD);
	
	return 0;
}
