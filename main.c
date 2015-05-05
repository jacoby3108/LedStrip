#include "mc9s12xdp512.h"
#include "rti.h"
#include "pll_asm.h"
#define T1H 16  // 800ns  @ 50ns OCTic
#define T0H 8   // 400ns  @ 50ns OCTic
#define TP 25  // 1.25 us @ 50ns OCTic
#define TRES  1000  // 50 us @ 50ns OCTic

                                       

unsigned  int delta;
unsigned char mask,ByteCtr=2;
unsigned char *p2rgb;

unsigned char temp[]={0xA5,0x5A};


static void system_init(void);



void main(void) {                  
   
 
 
 system_init();
    
 p2rgb=temp;   
    
    //largamos!
//    _asm cli;

    
  TSCR2 = (TSCR2 & 0xF8) | 0x01;    // Prescaler en 2, cuenta cada 2/40MHz = 50ns
  TSCR1 |= TSCR1_TEN_MASK;          // Habilita el timer para el IOC

  TIOS_IOS7=1;     // Set Timer 6 and 7 in OC mode
  TIOS_IOS6=1;
 

  TCTL1_OL7 = 1;   // Toggle on OC7
  TCTL1_OM7 = 0;

  
  TCTL1_OL6 =0;    // When OC6 is reached OC6 pin (DIN) goes LO 
  TCTL1_OM6 =1;


  CFORC_FOC6=1; 
  CFORC_FOC7=1; 


  TC7=TCNT+2000;   // lets breathle 
  TC6=TC7;
   
    
  TFLG1=0x80; //  Clr C7F
  TFLG1=0x40; //  Clr C6F
  
  
  
  while (!TFLG1_C7F); //Wait for Sync 
  
  while (!TFLG1_C6F); // Sync 
  
    
  
     // Reset flags
  
  TFLG1=0x80; //  Clr C7F
  TFLG1=0x40; //  Clr C6F
  
  
   
  TC7=TC7+1000;
  TC6=TC6+1000;
     
// **********************************************     
// DIN signal of WS2812B is generated at OC6 pin 
   
// OC7 Timer generates the signal period (always 1.25us)
// OC6 Timer generates the signal pulse width 0.4us for 0 and 0.8us for 1



// At OC7 compare the following takes place:
  
// OC7 sets OC6 at 1
// 
                          

// set up 


    OC7M_OC7M6=1;          // OC7 controls OC6 pin 
    OC7D=0x40;             // When OC7 is reached OC6 pin goes HI
        
        
    mask=0x80;             // Init walking '1'
 
 
 
    if((*p2rgb)&mask)      // Setup first Bit
          delta=T1H;
    else
          delta=T0H;
    
    
       
    
//for(;;)

while (ByteCtr)
  
  {

    
    
    while (!TFLG1_C7F);  // Sync with start of period
  
  // WARNING: Critical timing Don't put any code here

     TC6=TC7+delta;   // set T1H,T0H value on OC6
    

//   TC7=TC7+25;      // 1.25us   (nominal period) 

// Add some extra time to the nominal period 
// in order to set up next iteration

   TC7=TC7+65;      // 1.25us + 2us = 3.25us < 5us
 ///  TC7=TC7+85;      // xxxxx1.25us + 2us = 3.25us < 5us

// Reset OC flags           

     TFLG1=0x80|0x40; //  Clr C7F and C6F

// Set up next bit     
     
     mask>>=1;        // Next bit

     if(mask){        // if there are bits left ...
              
          if((*p2rgb)&mask)
              delta=T1H;
          else
              delta=T0H;
     }
     else{                  // Start a new Byte     
      
                      
          p2rgb++;             // Next byte
          ByteCtr--;    
          mask=0x80;           // Restart
 
/*       
       if(!ByteCtr){
      
            
           OC7D=0x00;   // When OC7 is reached OC6 pin goes LO
           // aca va return

       }
*/       
          if((*p2rgb)&mask)     // Setup first Bit
              delta=T1H;
          else
              delta=T0H;
       
       
     } // else
       
   } // while
   
     OC7D=0x00;   // When OC7 is reached OC6 pin goes LO       
     for (;;) ; 
   
   
}  // main



static void system_init(void) 
{


  #ifdef FLASH     

    pll_init();	  // solo para flash  (ver config.h)

  #endif				     
   
    //PORTA = todas salidas en 0
    PORTA = 0x00;
    DDRA = 0xFF;
    
//    rti_init();
}


