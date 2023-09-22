/*    ADC.c
 Moses Peace McCabe
 
 Runs on LM4F120/TM4C123
 Provide functions that initialize ADC0 SS3 to be triggered by
 software and trigger a conversion, wait for it to finish,
 and return the result.
 
*/

#include "tm4c123gh6pm.h"

//unsigned long equationRes; 

// This initialization function 
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain1 (PE2)
// SS3 interrupts: flag set on completion but no interrupt requested

void ADC_Init(void){ 
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0X00000010;   //  activate clock for Port E
  delay = SYSCTL_RCGC2_R;           //  allow time for clock to stabilize
  GPIO_PORTE_DIR_R &= ~0X04;        //  make PE4 input
  GPIO_PORTE_AFSEL_R |= 0X04;       //  enable alternate function on PE2
	GPIO_PORTE_DEN_R &= ~0X04;      //  disable digital I/O on PE2
	GPIO_PORTE_AMSEL_R |= 0X04;     //  enable analog function on PE2
	SYSCTL_RCGC0_R |= 0X00010000;   //  activate ADCLK
	delay = SYSCTL_RCGC2_R;         //  allow time for clock to stabilize
	SYSCTL_RCGC0_R &= ~0X00000300;   // set speed ADC 
	ADC0_SSPRI_R = 0X0123;			 // set priority of sequencer
	ADC0_ACTSS_R &= ~0X0008;         // disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000;			 // set seq3 as software trigger
	ADC0_SSMUX3_R &= ~0x000F;		 // clear ss3 field
	ADC0_SSMUX3_R += 1;				 // set channel 7 (PE4)
	ADC0_SSCTL3_R = 0X0006;			 // set flag on sampling
	ADC0_ACTSS_R |= 0X0008;			 // enable sample sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC_In(void)
{ 
	unsigned long data;
  ADC0_PSSI_R = 0x0008;              // start  - initiate ss3
  while((ADC0_RIS_R&0x08)==0){};     // wait for conversion done
  data = ADC0_SSFIFO3_R&0xFFF;		 // read result
  ADC0_ISC_R = 0x0008;			     // acknowledge completion (clear bit)
	return data;                     // return scan value
}



// Initialize any PIN as Output PF2
void PORTF_Init(void)
{
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x20;            // activate port F clock
	delay = SYSCTL_RCGC2_R;			    // wait for clock to turn on
	GPIO_PORTF_LOCK_R = 0x4c4f434b;     // unlock but not needed
	GPIO_PORTF_CR_R |= 0x4;             // not needed
	
	GPIO_PORTF_DIR_R |= 0x04;       // Set PF2 as output
	
	GPIO_PORTF_AMSEL_R &= 0x00;     // Disable analog functionality
	GPIO_PORTF_AFSEL_R &= 0x00;     // disable alt funct on PF2
	GPIO_PORTF_DEN_R  |= 0x04; 	    // configure PF2 as GPIO

	GPIO_PORTF_PUR_R |= 0X4;
	GPIO_PORTF_PCTL_R |= 0x4;
}
 
