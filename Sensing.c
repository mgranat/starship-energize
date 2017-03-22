//Sensing.c 
	
/*	
	Pin assignments-                     
		PE0: PV-DC voltage pin (AIN3)
			
		PE1: PV-DC current pin (AIN2)
			
		PE2: DC-DC voltage pin (AIN1)
			
		PE3: a-leg AC voltage pin (AIN0)
		PE4: b-leg AC voltage pin (AIN9)
		PE5: c-leg AC voltage pin (AIN8)
			
		PD0: a-leg AC current pin (AIN7)
		PD1: b-leg AC current pin (AIN6)
		PD2: c-leg AC current pin (AIN5)
		*/
		
#include <stdint.h>
#include "tm4c123gh6pm.h"

/***********ADC_Init()************
	Init Pins PE0-5, PD 0-2 for ADC functionality
	Use ADC0 module to handle all inputs
	Input: None
	Output: None
	*/
void ADC_Init(void){
	volatile int delay =0;
	SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E 
	while((SYSCTL_PRGPIO_R&0x10) == 0){};
	
	/*
	GPIO_PORTE_DIR_R &= ~0x3F;      // 2) make PE0-5 input
	GPIO_PORTE_AFSEL_R |= 0x3F;     // 3) enable alternate fun on PE0-5
	GPIO_PORTE_DEN_R &= ~0x3F;      // 4) disable digital I/O on PE0-5
	GPIO_PORTE_AMSEL_R |= 0x3F;     // 5) enable analog fun on PE0-5
  
	SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D 
	while((SYSCTL_PRGPIO_R&0x08) == 0){};
	GPIO_PORTE_DIR_R &= ~0x07;      // 2) make PD0-2 input
	GPIO_PORTE_AFSEL_R |= 0x07;     // 3) enable alternate fun on PD0-2
	GPIO_PORTE_DEN_R &= ~0x07;      // 4) disable digital I/O on PD0-2
	GPIO_PORTE_AMSEL_R |= 0x07;	
	*/
		
	//GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE2 input
	//GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate fun on PE2
	//GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE2
	//GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog fun on PE2	
		
	SYSCTL_RCGCADC_R |= 0x01;       // 6) activate ADC0 	
	delay=SYSCTL_RCGCGPIO_R;
	delay=SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_DIR_R &= ~0x30;      // 3) make PE4 PE5 input
	GPIO_PORTE_AFSEL_R |= 0x30;     // 4) enable alternate function on PE4 PE5
	GPIO_PORTE_DEN_R &= ~0x30;      // 5) disable digital I/O on PE4 PE5
								  // 5a) configure PE4 as ?? (skip this line because PCTL is for digital only)
	GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R&0xFF00FFFF;
	GPIO_PORTE_AMSEL_R |= 0x30;     // 6) enable analog functionality on PE4 PE5
	ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field
	ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
	ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
	ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
	ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
	ADC0_SSMUX2_R = 0x0089;         // 12) set channels for SS2
	ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
	ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
	ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2
  
	/*
	ADC0_PC_R = 0x02;               // 7) configure for 125K 
	
	ADC0_SSPRI_R = 0x0123;          // 8) Seq 3 is highest priority
	
	//ADC0_ACTSS_R &= ~0x000C;        // 9) disable sample sequencer 2,3
	ADC0_ACTSS_R &= ~0x0008;
	
	//ADC0_EMUX_R &= ~0xFF00;         // 10) seq2,3 is software trigger
	ADC0_EMUX_R &= ~0xF000; 
	
	//ADC0_SSMUX3_R = 0x01235678;     // 11) 			change the numbers to the value of the ADC sampler you want to use.
	//ADC0_SSMUX2_R = 0x0009;			// 	   Ain9
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+1;
	
	//ADC0_SSCTL3_R = 0x0060;         // 12) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
	//ADC0_SSCTL2_R = 0x0006;			// no TS0 D0, yes IE0 END0 				not really sure what this does as of now
	ADC0_SSCTL3_R = 0x0006;
	//TS0 bit is for measuring temp. 
	//IE0 bit makes the RIS bit set when the sample is complete
	//END0 set if last sample in the sequence (only?)
	//what is D0?
	//assuming the TS1, IE1, END1 are the same, 	why set to what the are?
		
	//ADC0_IM_R &= ~0x000C;           // 13) disable SS2,3 interrupts
	ADC0_IM_R &= ~0x0008;
	
	//ADC0_ACTSS_R |= 0x000C;         // 14) enable sample sequencer 2,3
	ADC0_ACTSS_R |= 0x0008;
	*/
	
	//ADC0_SAC_R = 0x04; 				  // 15) enable hardware oversampling; A N means 2^N (16 here) samples are averaged; 0<=N<=6
}

/***********ADC_In****************
	Busy-wait Analog to digital conversion
	Input: none
	Output: ten 12-bit result of ADC conversion -- value between 0 and 4095, 				4 will always be empty (not using)
	data[0] is ADC0 (PE3) 0 to 4095														not sure if ordering is correct, just matching to SSMUX3 for now
	data[1] is ADC1 (PE2) 0 to 4095
	data[2] is ADC2 (PE1) 0 to 4095
	data[3] is ADC3 (PE0) 0 to 4095
	data[4] is empty
	data[5] is ADC5 (PD2) 0 to 4095
	data[6] is ADC6 (PD1) 0 to 4095
	data[7] is ADC7 (PD0) 0 to 4095
	data[8] is ADC8 (PE5) 0 to 4095
	data[9] is ADC9 (PE4) 0 to 4095
	*/

void ADC_In(int data[10]){
	ADC0_PSSI_R = 0x0008; //write to PSSI bit 3        
	while((ADC0_RIS_R&0x08)==0){};   //busy-wait
	data[0] = ADC0_SSFIFO3_R&0xFFF; 	//read from SSFIFO3									not sure if ordering is correct, just matching to SSMUX3 for now
	data[1] = ADC0_SSFIFO3_R&0xFFF;
	data[2] = ADC0_SSFIFO3_R&0xFFF;
	data[3] = ADC0_SSFIFO3_R&0xFFF;
	data[5] = ADC0_SSFIFO3_R&0xFFF;
	data[6] = ADC0_SSFIFO3_R&0xFFF;
	data[7] = ADC0_SSFIFO3_R&0xFFF;
	data[8] = ADC0_SSFIFO3_R&0xFFF;
	ADC0_ISC_R = 0x0008; 	//clear sample complete flag (write to ISC bit 3)
		
	ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
	while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
	data[9] = ADC0_SSFIFO2_R&0xFFF;  // 3) read first result
	ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
}

void ADC_In89(int data[2]){ 
  ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
  data[1] = ADC0_SSFIFO2_R&0xFFF;  // 3A) read first result
  data[0] = ADC0_SSFIFO2_R&0xFFF;  // 3B) read second result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
}

int ADC_In_Single(void){    
	uint32_t data;
	ADC0_PSSI_R = 0x0008; //write to PSSI bit 3        
	while((ADC0_RIS_R&0x08)==0){};   //busy-wait
	data = ADC0_SSFIFO3_R&0xFFF; 	//read from SSFIFO3
	ADC0_ISC_R = 0x0008; 	//clear sample complete flag (write to ISC bit 3)
	return data;
}

/***********ADC_Calib*************
	return the values of the DC sensing data
	Calibration for the ADC ports to convert ADC value (0-4095) to LCD display value
	Input: none
	Output: unsigned decimal value. Last 3 numbers are the tenths, hundredths, and thousandths values (to avoid floating point numbers)
	*/
void ADC_Calib (int data[10]){
	return;
}
