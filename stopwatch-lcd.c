#include <hidef.h> /* for EnableInterrupts macro */
#include "mc9s08qg8.h" /* include peripheral declarations */

// The following two define statements should be near the top of your code.
// They just give simpler and more mnemonic names to the bits that control the LCD.
// Use the port numbers that match the way you wired the LCD to your S08.
#define EN PTAD_PTAD2
#define RS PTAD_PTAD3


void cpuinit(void);
void mtiminit(void);

// *** LCD PROTOTYPES ***
void delay40us(void);
void delay1640us(void);
void lcdinit(void);
void testlcd(void);

void lcdputstr(unsigned char *);
void lcdputchr(unsigned char);
void lcdputcmd(unsigned char);

unsigned char advtime=0, zerotime=0;  // global variables

void main(void) {
   static unsigned char time[]="00:00:00.00";
   
   cpuinit();
   mtiminit();
   //lcdinit();
   //testlcd();
   
   lcdinit();
   lcdputcmd(0x80);
   lcdputstr(time);
   EnableInterrupts;
   for(;;)
       {
       if (advtime)
         {
         advtime = 0;
         if (++time[10]>'9')
           {
           time[10] = '0';
           if (++time[9]>'9')
             {
             // place the code here to complete the time update
			       time[9] = '0';
			       if (++time[7]>'9')
               {
               time[7] = '0';
               if (++time[6]>'5')
                 {
			           time[6] = '0';
				         if (++time[4]>'9')
                   {
                   time[4] = '0';
                   if (++time[3]>'5')
                     {
			               time[3] = '0';
				             if (++time[1]>'9')
                       {
                       time[1] = '0';
                       if (++time[0]>'9')
                         {
			                     time[0] = '0';
                        } // if time[0]
                       }// if time[1]
					   
                    }// if time[3]
                   } // // if time[4]
				   
                } // if time[6]
			   } // if time[7]
			   
			 } // if time[9]
		   } // if time[10]
   lcdputcmd(0x80);
   lcdputstr(time);
         } // if (advtime)
		 
       if (zerotime)
         {
         zerotime = 0;
         time[1]=time[3]=time[4]=time[6]=time[7]=time[9]=time[10]='0';
   lcdputcmd(0x80);
   lcdputstr(time);
         } // if (zerotime)
       } //for(;;)      /* loop forever */ 
    
  /* never leave main */
} // main


// *** LCD INITILIALIZATION ***
void lcdinit(void) {
   lcdputcmd(0x0f);  // Display on and set cursor to blinking underscore
   lcdputcmd(0x01);  // Clear display -- may or may not be needed
}

// *** WRITING AND READING TO LCD ***
void lcdputchr(unsigned char chr) {
	RS = 1; //show data is a command
	PTBD = chr;
	EN = 1; // start the Enable pulse
	EN = 0; // end the Enable pulse
	delay40us();
}

void lcdputcmd(unsigned char chr) {
  
	RS = 0; //show data is not a command -- register select is set to zero
	PTBD = chr;
	EN = 1; // start the Enable pulse
	EN = 0; // end the Enable pulse
	
  if (chr <= 3) {
     delay1640us();
  } else {    
     delay40us();
  }	
}

void lcdputstr(unsigned char *str) {
	unsigned char c;
	c = *str++;
	while (c)
	{
		if(c==0x0D) {  //send to putcmd if a carriage return
	    lcdputcmd(0x80);
	  }
	  else {
	    lcdputchr(c);
	    c = *str++;
	  }
	}
}

// ** DELAY FUNCTIONS**
void delay40us(void) {
  unsigned int k;
  for (k=0;k<14;k++);
}

void delay1640us(void) { //delay 1640 us by calling the del_40() function fourty-one times
  unsigned int j;
	for (j=0;j<41;j++) {
	 delay40us(); 
	}
}


// *** SCI READING AND WRITING FUNCTIONS ***
void testlcd(void) {
   lcdputchr('H');  // Write the word "Hi" at top left of LCD
   lcdputchr('i');
   lcdputcmd(0x38); // enable 2-line mode – contrast may change
   lcdputcmd(0x47); // start the next word in the middle of line 2 
   lcdputchr('t');
   lcdputchr('h');
   lcdputcmd(0x18);// shift the displayed chars to the left one char
   lcdputcmd(0x18); // shift another char
}

// *** INTERRUPT AND CPU INITIALIZATIONS ***
void cpuinit(void){
    unsigned int i;
    PTAPE = PTAPE_PTAPE0_MASK | PTAPE_PTAPE1_MASK;  // pull-ups on pins 13 and 14 
    PTADD = PTAPE_PTAPE2_MASK | PTAPE_PTAPE3_MASK;
    PTBDD = 0xFF;
    SOPT1 = 0x53;
    ICSTRM = NV_ICSTRM;
    for (i=0;i<3000;i++); //delay 15 ms for osc to stabilize
    ICSC2 = 0;  // double speed to 8 MHz
   
}
void mtiminit(void){       // the values shown produce 125/250 TOFs per second
    MTIMSC = MTIMSC_TOIE_MASK;  // they must be changed to provide 1000 TOFs/sec
    MTIMCLK = 5;  // 5 is the PS value
    MTIMMOD = 249; // cycles 0-249 for 250 different values
}   

interrupt VectorNumber_Vmtim void mtimisr(void){
    static unsigned char run=0,prev_ss=1,curr_ss,prev_rst=1,curr_rst,countdown=1;
    MTIMSC_TOF = 0; // acknowledge (clear) the event
    if (--countdown==0)
      {             // we enter here every 10 interrupts (every 10 ms)
      countdown = 10;
      if (run) advtime = 1;
      curr_ss = PTAD_PTAD0;
      curr_rst = PTAD_PTAD1;
      if (prev_ss && !curr_ss) run = !run;
      if (prev_rst && !curr_rst && !run) zerotime = 1;
      prev_ss = curr_ss;
      prev_rst = curr_rst;
      }
}

