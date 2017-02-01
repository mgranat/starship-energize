// PWM.c
// Runs on TM4C123
// Use PWM0A/PB6 and PWM0B/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// January 13, 2017
// ***************************************************
// ************remove R9 and R10**********************
// ***************************************************
// PB7 A+  PWM 100 Hz, right motor, PWM positive logic
// PB6 A-  regular GPIO, right motor, 0 means forward
// PB5 B+  PWM 100 Hz, left motor, PWM negative logic
// PB4 B-  regular GPIO, left motor, 1 means forward
// PD0 is servo A, 20ms period, pulse time 0.5 to 2.5ms
// PD1 was servo B, but I needed the hole to fix regulator

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
#include "tm4c123gh6pm.h"
#define PWM_0_GENA_ACTCMPAD_ONE 0x000000C0  // Set the output signal to 1
#define PWM_0_GENA_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0
#define PWM_0_GENB_ACTCMPBD_ONE 0x00000C00  // Set the output signal to 1
#define PWM_0_GENB_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0

#define SYSCTL_RCC_USEPWMDIV    0x00100000  // Enable PWM Clock Divisor
#define SYSCTL_RCC_PWMDIV_M     0x000E0000  // PWM Unit Clock Divisor
#define SYSCTL_RCC_PWMDIV_2     0x00000000  // /2

#define PB7       (*((volatile uint32_t *)0x40005200))
#define PB5       (*((volatile uint32_t *)0x40005080))

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64 
//                = 3.2 MHz/64 = 50 kHz (in this example)

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/64
//                = 80 MHz/64 = 1.25 MHz (in this example)
// Inputs: period is in 800 ns units
//         duty is in 800 ns units
//         direction is value output to PB5 (0 or 1)
// Output on PB4/M0PWM1A
// GPIO on   PB5 controls direction
void PWM_Init() {
	// 20 kHz switching frequency
	uint16_t period = 100;
	uint16_t duty = 20;

	// Initialize PB6 and PB7 on PWM0
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
  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_64; // configure for /64 divider

  PWM0_0_CTL_R = 0;                     // disable PWM while initializing
  //PWM0, Generator A (PWM0/PB6) goes to 0 when count==reload and 1 when count==0
  PWM0_0_GENA_R = (PWM_0_GENA_ACTLOAD_ONE|PWM_0_GENA_ACTCMPAD_ZERO);
  //PWM0, Generator B (PWM1/PB7) goes to 0 when count==CMPA counting down and 1 when count==CMPA counting up
  PWM0_0_GENB_R = (PWM_0_GENB_ACTLOAD_ONE|PWM_0_GENB_ACTCMPBD_ZERO);

  PWM0_0_LOAD_R = period - 1;						// count from zero to this number and back to zero in (period - 1) cycles
  PWM0_0_CMPA_R = (period - 1)/4;       // count value when PWM1/PB7 toggles
	PWM0_0_CMPB_R = (period - 1)/8;				// count value when PWM0/PB6 toggles
  PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;			// start PWM0 in count down mode

  PWM0_ENABLE_R |= (PWM_ENABLE_PWM1EN|PWM_ENABLE_PWM0EN); // enable PWM1-0
	
	// Initialize PB4 on PWM1
  PWM0_1_CTL_R = 0;                     // re-loading down-counting mode
  PWM0_1_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down

  PWM0_1_LOAD_R = period - 1;           // cycles needed to count down to 0
  PWM0_1_CMPA_R = duty - 1;             // count value when output rises
  PWM0_1_CTL_R |= 0x00000001;           // start PWM0

  PWM0_ENABLE_R |= PWM_ENABLE_PWM2EN;		// enable PB4/M0PWM1A is PWM2
}

// change duty cycle of PB4
// Inputs: period was set in call to Init
//         duty is in 800 ns units
//         direction is value output to PB5 (0 or 1)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void Left_Duty(uint16_t duty, int direction){
  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
    if(direction){
    PB5 = 0x20;
  }else{
    PB5 = 0;
  }
}

int main() {
	while(1) {}
}

int SystemInit() {
	// PWM0Dual_Init(100);
	// Left_Init(100, 20, 1);
	
	PWM_Init();
	
	return 0;
}
