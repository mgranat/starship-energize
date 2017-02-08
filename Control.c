#include <stdint.h>

#include "PLL.h"
#include "PWM.h"
#include "SysTickInts.h"

#define TIMER_PERIOD (10000)
#define TIMER_FREQUENCY (8000)

void SysTick_Handler(void){
	PWM_tick(TIMER_FREQUENCY, 60);
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
