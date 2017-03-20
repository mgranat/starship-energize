//Sensing.h

/*Need 
	1 PV DC voltage pin
    1 PV DC current pin
    1 DC-DC voltage pin
    3 AC    voltage pin
    3 AC    current pin
	___________________
	Total: 9 ADC pins
		
	Available pins for ADC: PE 0-5, PB 0-5 ---
		PB 4 5 taken by Max for PWM
	
	reference: page 801 of http://www.ti.com/lit/ds/spms376e/spms376e.pdf
		
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


/***********ADC_Init()************
	Init Pins PE0-5, PD 0-2 for ADC functionality
	Use ADC0 module to handle all inputs
	Input: None
	Output: None
	*/
void ADC_Init(void);	



/***********ADC_In****************
	Busy-wait Analog to digital conversion
	Input: value between 0 and 9 to choose which ADC to be returned, corresonds with AIN value
	Output: 12-bit result of ADC conversion -- value between 0 and 4095
	*/
void ADC_In(int data[10]);



/***********ADC_Calib*************
//return the values of the DC sensing data
	Calibration for the ADC ports to convert ADC value (0-4095) to LCD display value
	Input: value between 0 and 9 to choose which ADC to be converted, corresponds with AIN value
	Output: unsigned decimal value. Last 3 numbers are the tenths, hundredths, and thousandths values (to avoid floating point numbers)
	*/
void ADC_Calib (int data[10]); 
