#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include <string.h>
#define SECCOUNT 125
#define COL0 PTAD_PTAD3 
#define COL1 PTAD_PTAD2  
#define BUFS 100
#define GREEN PTAD_PTAD1   
#define RED PTAD_PTAD0     

unsigned char newkey=0,badcount=0;
unsigned int mtimcntdn=0,ledcntdn=0;
const char rowtabl[] = {0,1,1,1,3,1,6,9};
static unsigned char buffer[BUFS], *inptr=buffer, *outptr=buffer;
void addbyte(unsigned char);   
void waitp(void), waitr(void), decode(void);
void(*keystate)(void);        /* key state ptr */

void next(unsigned char), wrong(unsigned char);  /* combination state functions */
void(*combostate)(unsigned char);        /* code state ptr */
     
void badcode(void); /* this function called by last and wrong */

unsigned const char code[]="12345";
unsigned const char keycode[50];

unsigned const char *nextdigit=code;
const char keymapping[] = {'1','2','3','4','5','6','7','8','9','*','0','#'};
 
 
void main(void) {
    unsigned int i;    
    SOPT1 = SOPT1_BKGDPE_MASK+SOPT1_RSTPE_MASK;
    ICSTRM = NV_ICSTRM;
    for (i=0;i<400;i++); /* delay 2 ms (at 4 MHz) */
    ICSC2 = 0;  /* up speed to 8 MHz */
 
    PTAPE = 0xff;
    PTBPE = 0xff;
   
    PTADD = 0xff; /* set pin 16 and 15 as an output */
    PTADS = 0xff; /* make capable of driving LEDs */
   
    MTIMCLK = 0x08;  /* use bus clock divided by 256 */
    MTIMMOD = 249;   /* make period exactly 8 ms */
    MTIMSC = 0x40;   /* start timer and enable TOF ints */
    COL0 = 0; 
    COL1 = 0;   
    keystate=waitp;
    combostate=next;
    nextdigit=code;
    EnableInterrupts; /* enable interrupts */

    for(;;);   /* loop forever */
 }

// place the five keypad decoding state functions here
// the decode state must trigger a combostate function
 void waitp(void) {
	if ((PTBD & 0x0f) != 0x0f) 
		keystate = decode;
	else 
		keystate = waitp;
 } 
 
void decode(void) {
  newkey = rowtabl[(PTBD & 0xF)-0x7]; 
  COL0 = 1;
  if ((PTBD & 0xF) != 0xF)
	  newkey += 1; 
	COL1 = 1;
	if ((PTBD & 0xF) != 0xF) 
		newkey += 1; 
  newkey = keymapping[newkey];
  COL0 = 0; 
  COL1 = 0; 
  combostate(newkey);
  keystate=waitr;
 }           
 
void waitr(void){
	if ((PTBD & 0x0f) == 0x0f)
		keystate = waitp;
	else 
		keystate = waitr;
 }
 
 
 
 
void next(unsigned char key) {
    // check for * and # and perform appropriate action.
// for * you just set nextdigit back to start of code
// for # you see if the code is complete (current digit
//     is null) -- if complete light GREEN, reset badcount
    //     to zero, & set mtimcntdn; else call badcode().
 
   if( newkey =='*'){
           
       wrong(newkey);
     
    }
   else if (newkey =='#')
   {
             
        if(nextdigit>&code[strlen(code)-1])
        {   
          GREEN=1;
          ledcntdn=1;
          mtimcntdn= SECCOUNT;
         
           RED=0;
           badcount=0;
           nextdigit=code;
        }  else{
         
       
        wrong(newkey);  
        }
       
   }
           
        else if(newkey ==* nextdigit){
       
           nextdigit++;
           
           } else   {
             
            combostate = wrong;
           
            }           
                         
}
   
   
     
void wrong(unsigned char newkey) {
 nextdigit=code;
 combostate=next;
 if(newkey =='#')
    badcode();
 }
   
void badcode(void) {
 /* must light red LED, increment badcount */
/* if badcount too high delay 30 else delay 2 */
/* set reinitialize combostate and nextdigit for a new try */
   RED = 1;
   ledcntdn = 1;
   if(badcount >= 3){
    nextdigit=code;
    combostate=next;
    mtimcntdn = SECCOUNT*5;
   } else {
      badcount++;
      mtimcntdn = SECCOUNT;
   }
  }  
  
  
void addbyte(unsigned char newchar) {
	*inptr = newchar;
	if (inptr >= buffer+BUFS)
		inptr = buffer;
}	

 interrupt VectorNumber_Vmtim mtimisr() {
    MTIMSC_TOF = 0;  /* ack interrupt */
    if (ledcntdn == 0) {
       keystate();    /* execute current state */
    }
    else if (mtimcntdn-- == 0){
      /* dec count */
      /* turn off LEDs */
      RED =  0;
      GREEN = 0;
      ledcntdn=0;
    }
 }

   
/* end of source file */
