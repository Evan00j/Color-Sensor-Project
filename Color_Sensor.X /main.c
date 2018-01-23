    /*
 * File:   main.c
 * Author: evan
 *
 * Created on November 3, 2017, 3:13 PM
 */


#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include  "i2c.h"
#include  "ri2c_LCD.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#define BAUD 9600
#define FOSC 4000000L
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1)) // Should be 25 for9600/4MhZ
#define NINE_BITS 0
#define SPEED 0x4
#define RX_PIN TRISC5
#define TX_PIN TRISC4
#define I2C_SLAVE 0x27	/* was 1E Channel of i2c slave depends on soldering on back of board*/
#define _XTAL_FREQ 4000000.0    /*for 4mhz*/

//void rI2C_LCD_Command(unsigned char,unsigned char);
//void rI2C_LCD_SWrite(unsigned char,unsigned char *, char);
//void rI2C_LCD_Init(unsigned char);
//void rI2C_LCD_Pos(unsigned char,unsigned char);
//unsigned char rI2C_LCD_Busy(unsigned char);
void setup_comms(void);
void putch(unsigned char);
unsigned char getch(void);
unsigned char getche(void);

int frequency,counter,count;
#define volt_thresh = 1;

void InitTimer1(){
  T1CON         = 0x85; //Timer1 asynchronous counter 1:1 Prescale Value and Enables Timer1
  TMR1H         = 0;
  TMR1L         = 0; //The TMR1IF /H /L bits should be cleared before enabling interrupts
  TMR1IF        = 0;     
  TMR1IE        = 1; 
  INTCON        = 0xE0; //GIE on enables all active interrupts PEIE on enables all Peripheral interrupts Enables overflow bit
  T1GCON   	=  	0XA1;
}

void setup_comms(void){
RX_PIN = 1;
TX_PIN = 1;

SPBRG = DIVIDER;
RCSTA = (NINE_BITS | 0x90);
TXSTA = (SPEED | NINE_BITS | 0x20);
TXEN = 1;
SYNC = 0;
SPEN = 1;
BRGH = 1;
}

void usartConfig(){
    APFCON0 = 0x84; //This sets pins RC5 and RC4 as RX & TX on pic16f1829
    TXCKSEL = 1;   
    RXDTSEL = 1;  //makes RC4 & 5 TX & RX for USART (Allows ICSP)
}


void InitTimer0(){
    OSCCON = 0X6A;
    OPTION_REG = 0xC5;
    TMR0IF = 0;
    
}  

void putch(unsigned char byte)
{
/* output one byte */
while(!TXIF) /* set when register is empty */
continue;
TXREG = byte;
}
unsigned char getch()
{
/* retrieve one byte */
while(!RCIF) /* set when register is not empty */
continue;
return RCREG;
}
unsigned char getche(void)
{
unsigned char c;
putch(c = getch());
return c;
}

void interrupt isr(){

  //Timer0   code to measure 1s duration
  if (TMR0IF){
    count++;
    TMR0IF = 0;
    TMR0 = 0;
    //Enter your code here
    if (count >= 300) { // 4s
           frequency=(TMR1H << 8) + TMR1L + (counter*256);
          TMR1H  = 0;
          TMR1L  = 0;
          counter=0;
          count = 0;             // Reset cnt
        }
  }
  // Timer! code to count incoming pulses
  if (TMR1IF){
      TMR1IF = 0;
      TMR1H  = 0;
      TMR1L  = 0;
     counter++;
  }
}
int main(int argc, char** argv) {
    frequency = 0;
    TRISA = 0x10; //RA5 as input from TCS3200 Color Sensing Module
    ANSELA =0x00;
    TRISB = 0x30;
    ANSELB =0x00;
    TRISC = 0xFF; /* set as output */
    ANSELC =0x00;
    RB5 = 0;
    RB4 = 0;
    RA5 = 0;
    setup_comms();
     TSEN = 1;
     TSRNG = 0;
    usartConfig();
    InitTimer0();
    InitTimer1();
    while(1){
        RA5 =0;
        RA5=1;
       printf("Frequency = %d \r\n", frequency);
         __delay_ms(1000);
    }
       
    
    return(EXIT_SUCCESS);
}
