#include "mc9s12xdp512.h"
#include "rti.h"

static volatile unsigned int rti_dly;
			  
void rti_init(void) 
{
    RTICTL = 0x50;              // (16MHz / 2^14) = 0.976 mseg ~ 1mseg
    CRGINT |= CRGINT_RTIE_MASK;
 
    rti_dly = 0;			    // delay counter start in reset state  	   
}


void delayms(unsigned int dly) 
{
    // set delay
    CRGINT &= ~CRGINT_RTIE_MASK;
	rti_dly = dly;	    
	CRGFLG = CRGFLG_RTIF_MASK;
    CRGINT |= CRGINT_RTIE_MASK;
	
	// and wait
	while (rti_dly);    
}


void interrupt ISR_rti(void) 
{     
    //acá llego cada 1 mseg
	
	// bajo la bandera
	CRGFLG = CRGFLG_RTIF_MASK;  
	
	//Toggle PA1 (DEBUG)
	PORTA ^= 0x02;     
	
	// pump down user timer  
	if (rti_dly) rti_dly--;
}



