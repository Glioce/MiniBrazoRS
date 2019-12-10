/*
 * File:   multi_servo_01.c
 * 10 de diciembre de 2019
 * Cada 20 ms se genera un puslo para cada servo
 * Los valores de ancho de pulso se guardan en un array
 * Esos valores se acutalizan al recibir mensajes por I2C
 */

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 8000000
#include <xc.h>
#include <pic16f877a.h>

//array con valores de ancho de pulso en microsegundos
unsigned int pulso_us[8];
short z;

void __interrupt() I2C_Slave_Read()
{ 
    if(SSPIF == 1)
    {
       SSPCONbits.CKP = 0;
       
       if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL))
       {
             z = SSPBUF;            // Read the previous value to clear the buffer
             SSPCONbits.SSPOV = 0; // Clear the overflow flag
             SSPCONbits.WCOL = 0;   // Clear the collision bit
             SSPCONbits.CKP = 1;
       }

      if(!SSPSTATbits.D_nA) 
       {
           z = SSPBUF;
           //while(!BF);
           PORTD = SSPBUF;
           BF = 0;
           SSPCONbits.CKP = 1;
           //SSPM3 = 0;
       }
       SSPIF = 0;
    }
}

void I2C_Slave_Init(short address)
{
    SSPSTAT = 0x80;
    SSPADD = address;
    SSPCON = 0x36;
    SSPCON2 = 0x01;
    TRISC3 = 1;
    TRISC4 = 1;
    GIE = 1;
    PEIE = 1;
    SSPIF = 0;
    SSPIE = 1;
}

void main()
{
    nRBPU = 0;
    TRISB = 0xFF;
    TRISD = 0x00;
    PORTD = 0x00;
    I2C_Slave_Init(0x40);
    while(1);/*{
      PORTB = 0;
      __delay_ms(500);
      PORTB = 255;
      __delay_ms(500);
    }*/
}
