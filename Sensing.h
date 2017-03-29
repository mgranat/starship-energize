//Sensing.h

/*Need 
	1 PV DC voltage pin
    1 PV DC current pin
    1 DC-DC voltage pin
    3 AC    voltage pin
    3 AC    current pin
	___________________
	Total: 9 ADC pins
		
	Available pins for ADC: PE 0-5, PB 4-5, PD 0-3 ---
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
	Input: 9 int array to store data in
	Output: none
	*/
void ADC_In(int data[9]);


/***********ADC_Calib*************
	return the values of the DC sensing data
	Calibration for the ADC ports to convert ADC value (0-4095) to analog value
	Input: 1) value between 0 and 8 to index which ADC to be converted, chooses formula to convert with (switch statement)
		   2) ADC data (0-4095) for corresponding value
	Output: double with calculated value
	*/
double ADC_Calib (); 

/***********ADC_Print*************
	print the raw ADC data 0-4095
	if setup == 1 sets up screen with ADC data pairings if first / new screen printed to reduce flickering of all screen
	else prints ADC values in the correct location (number starts in 6th location, in the indexed (ith) row)
	Input: a 0 or a 1 to decide if using setup mode or no
	Output: none
	*/
void ADC_Print(int setup);

/***********error*************
	returns 1 if system has error
	Error is defined as: ???? Consult team/Baldick for advice and definitions
	Input: none
	Output: 0 if no error, 1 if error
	*/
int error();

/***********getPvPower*************
	returns power from the PV sensing side, PV Voltage * DC Current
	Input: none
	Output: double value, no truncation
	*/
double getPvPower();

/***********getPvVoltage*************
	returns voltage from the PV sensing side
	Input: none
	Output: double value, no truncation
	*/
double getPvVoltage();

/***********getAcPower*************
	returns power from the AC sensing side 
	= (AC Voltage1 * AC Current1 + AC Voltage2 * AC Current2 + AC Voltage3 * AC Current3)    //update formula, definitely incorrect
	Input: none
	Output: double value, no truncation
	*/
double getAcPower();

/***********getDcConverterVoltage*************
	returns voltage from the DC converter side
	Input: none
	Output: double value, no truncation
	*/

double getDcConverterVoltage();

