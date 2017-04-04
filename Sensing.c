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
#include "ST7735.h"

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
	
	SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D 
	while((SYSCTL_PRGPIO_R&0x08) == 0){};

	SYSCTL_RCGCADC_R |= 0x01;       // 2) activate ADC0 	
	delay=SYSCTL_RCGCGPIO_R;
	delay=SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTE_DIR_R &= ~0x3F;      // 3) make PE0 PE1 PE2 PE3 PE4 PE5 input
	GPIO_PORTE_AFSEL_R |= 0x3F;     // 4) enable alternate function on PE0 PE1 PE2 PE3 PE4 PE5
	GPIO_PORTE_DEN_R &= ~0x3F;      // 5) disable digital I/O on PE0 PE1 PE2 PE3 PE4 PE5
								    // 5a) configure PE4 as ?? (skip this line because PCTL is for digital only)
	GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R&0xFF000000;
	GPIO_PORTE_AMSEL_R |= 0x3F;     // 6) enable analog functionality on PE0 PE1 PE2 PE3 PE4 PE5
	
	GPIO_PORTD_DIR_R &= ~0x07;      // 3) make PD0 PD1 PD2
	GPIO_PORTD_AFSEL_R |= 0x07;     // 4) enable alternate function on PD0 PD1 PD2
	GPIO_PORTD_DEN_R &= ~0x07;      // 5) disable digital I/O on PD0 PD1 PD2
								    // 5a) configure PE4 as ?? (skip this line because PCTL is for digital only)
	GPIO_PORTD_PCTL_R = GPIO_PORTD_PCTL_R&0xFFFFF000;
	GPIO_PORTD_AMSEL_R |= 0x07;     // 6) enable analog functionality on PD0 PD1 PD2
	
	
	ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field
	ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
	ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
	
	ADC0_ACTSS_R &= ~0x0001;        // 10) disable sample sequencer 0
	ADC0_ACTSS_R &= ~0x0008;        // 10) disable sample sequencer 3
	ADC0_EMUX_R &= ~0x000F;         // 11) seq0 is software trigger
	ADC0_EMUX_R &= ~0xF000;         // 11) seq3 is software trigger
	ADC0_SSMUX0_R = 0x76532109;     // 12) set channels for SS0
	ADC0_SSMUX3_R = 0x0008;         // 12) set channels for SS3
	ADC0_SSCTL0_R = 0x60000000;     // 13) no TS7 D7, yes IE7 END7
	ADC0_SSCTL3_R = 0x0006;         // 13) no TS0 D0, yes IE0 END0
	ADC0_IM_R &= ~0x0001;           // 14) disable SS0 interrupts
	ADC0_IM_R &= ~0x0008;			// 14) disable SS3 interrupts
	ADC0_ACTSS_R |= 0x0001;         // 15) enable sample sequencer 0
	ADC0_ACTSS_R |= 0x0008;         // 15) enable sample sequencer 3
	
	ADC0_SAC_R = 0x04; 				  // 16) enable hardware oversampling; A N means 2^N (16 here) samples are averaged; 0<=N<=6
}

/***********ADC_In****************
	Busy-wait Analog to digital conversion
	Input: none
	Output: ten 12-bit result of ADC conversion -- value between 0 and 4095, 				
	data[0] is ADC9 (PE4) 0 to 4095	
	data[1] is ADC0 (PE3) 0 to 4095
	data[2] is ADC1 (PE2) 0 to 4095
	data[3] is ADC2 (PE1) 0 to 4095
	data[4] is ADC3 (PE0) 0 to 4095
	data[5] is ADC5 (PD2) 0 to 4095
	data[6] is ADC6 (PD1) 0 to 4095
	data[7] is ADC7 (PD0) 0 to 4095
	data[8] is ADC8 (PE5) 0 to 4095
	*/

void ADC_In(int data[9]){
	ADC0_PSSI_R = 0x0001; //write to PSSI bit 0        
	while((ADC0_RIS_R&0x01)==0){};   //busy-wait
	data[0] = ADC0_SSFIFO0_R&0xFFF; 	//read from SSFIFO0
	data[1] = ADC0_SSFIFO0_R&0xFFF;
	data[2] = ADC0_SSFIFO0_R&0xFFF;
	data[3] = ADC0_SSFIFO0_R&0xFFF;
	data[4] = ADC0_SSFIFO0_R&0xFFF;
	data[5] = ADC0_SSFIFO0_R&0xFFF;
	data[6] = ADC0_SSFIFO0_R&0xFFF;
	data[7] = ADC0_SSFIFO0_R&0xFFF;
	ADC0_ISC_R = 0x0001; 	//clear sample complete flag (write to ISC bit 3)
		
	ADC0_PSSI_R = 0x0008; //write to PSSI bit 3        
	while((ADC0_RIS_R&0x08)==0){};   //busy-wait
	data[8] = ADC0_SSFIFO3_R&0xFFF; 	//read from SSFIFO3
	ADC0_ISC_R = 0x0008; 	//clear sample complete flag (write to ISC bit 3)	
}


/***********ADC_Calib*************
	return the analog equivelent values of the sensing data
	Calibration for the ADC ports to convert ADC value (0-4095) to analog value (i.e 3.3V -> 4095 in ADC -> 220V returned)
	Input: 1) value between 0 and 8 to index which ADC to be converted, chooses formula to convert with (switch statement)
	Output: double with calculated value
	*/
double ADC_Calib (int choice){
	double retValue = 0;
	volatile int tempData = 0;
	int data[9] = {0};
	ADC_In(data);
	tempData = data[choice];
	
	switch(choice){
		case 0: //AIN9, b-leg AC voltage on PE4
			ST7735_OutUDec(9);							//ignore this for now, will need to experimentally calibrate then format to linear data then formula then implement
			break;
		case 1: //AIN0, a-leg AC voltage on PE3
			ST7735_OutUDec(0);
			break;
		case 2: //AIN1, DC-DC voltage on PE2
			ST7735_OutUDec(1);
			break;
		case 3: //AIN2, PV-DC current on PE1
			ST7735_OutUDec(2);
			break;
		case 4: //AIN3, PV-DC voltage on PE0
			ST7735_OutUDec(3);
			break;
		case 5: //AIN5, c-leg AC current on PD2
			ST7735_OutUDec(5);
			break;
		case 6: //AIN6, b-leg AC current on PD1
			ST7735_OutUDec(6);
			break;
		case 7: //AIN7, a-leg AC current on PD0
			ST7735_OutUDec(7);
			break;
		case 8: //AIN8, c-leg AC voltage on PE5
			ST7735_OutUDec(8);
			break;
	}
	return retValue;
}

/***********ADC_Print*************
	print the raw ADC data 0-4095
	if setup == 1 sets up screen with ADC data pairings if first / new screen printed to reduce flickering of all screen
	else prints ADC values in the correct location (number starts in 6th location, in the indexed (ith) row)
	Input: 9 int array with ADC data
	Output: None
	*/
void ADC_Print(int setup){
	int data[9] = {0};
	ADC_In(data);
	if(setup==1){
		ST7735_FillScreen(0x0000);
		for(int i=0; i<9; i++){
			ST7735_SetCursor(0,i);
			ST7735_OutString("ADC"); //19 characters each line max
			switch(i){
				case 0:
					ST7735_OutUDec(9);
					break;
				case 1:
					ST7735_OutUDec(0);
					break;
				case 2:
					ST7735_OutUDec(1);
					break;
				case 3:
					ST7735_OutUDec(2);
					break;
				case 4:
					ST7735_OutUDec(3);
					break;
				case 5:
					ST7735_OutUDec(5);
					break;
				case 6:
					ST7735_OutUDec(6);
					break;
				case 7:  
					ST7735_OutUDec(7);
					break;
				case 8:
					ST7735_OutUDec(8);
					break;
			}
			ST7735_OutString(": ");
		}
	} //end setup == 1
	else{
		for(int i=0; i<9; i++){
				ST7735_SetCursor(6,i);
				ST7735_OutString("    ");
				ST7735_SetCursor(6,i);
				ST7735_OutUDec(data[i]);
		}
	}
}

/***********error*************
	returns 1 if system has error
	Error is defined as: ???? Consult team/Baldick for advice and definitions
	Input: 9 int array with ADC data (calibrated)
	Output: 0 if no error, 1 if error
	*/
int error (){																		//needs defining and then coding
	// PV voltage significantly higher than rated
	// PV current significantly higher than rated
	// Output power significantly less than input power
	// Output power significantly greater than input power
	// Output power significantly greater than 220 V
	// Output power significantly higher than inverter rating
	// Converter voltage significantly less than PV voltage / 0.85
	// Converter voltage significantly greater than PV voltage / 0.1
	// Converter voltage significantly higher than rated
	return 0;
}

/***********getPvPower*************
	returns power from the DC sensing side, DC Voltage * DC Current
	Input: 9 int array with ADC data (calibrated)
	Output: double value, no truncation
	*/
double getPvPower(){
	return ADC_Calib(4) * ADC_Calib(3);
}

/***********getPvVoltage*************
	returns voltage from the PV sensing side
	Input: none
	Output: double value, no truncation
	*/
double getPvVoltage(){
	return ADC_Calib(4);
}

/***********getAcPower*************
	returns power from the AC sensing side 
	= (AC Voltage1 * AC Current1)*3 = active power assuming all three phases are working
	Input: 9 int array with ADC data (calibrated)
	Output: double value, no truncation
	*/
double getAcPower(){
	return ADC_Calib(1) * ADC_Calib(7);
}

/***********getDcConverterVoltage*************
	returns voltage from the DC converter side
	Input: 9 int array with ADC data (calibrated)
	Output: double value, no truncation
	*/

double getDcConverterVoltage(){
	return ADC_Calib(2);
}

