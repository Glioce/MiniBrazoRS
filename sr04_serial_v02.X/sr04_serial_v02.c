/*
 * File:   sr04_serial_v02.c
 * Author: om
 *
 * Created on 11 de noviembre de 2019, 08:03 AM
 */

// BEGIN CONFIG
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 8000000 //8 MHz
#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>
#include "uart.h"

//Pines para los sensores ultrasonicos
#define TRIG1 RB1
#define TRIG2 RB2
#define TRIG3 RB3
#define ECHO  RB4 //Todos los ECHO se conectan al mismo pin

int d; //Variable global para guardar la distancia calculada
char buffer [8]; //Buffer para texto

void __interrupt() echo() //Rutina para responder a la interrupcion
{
    if (RBIF == 1) //Makes sure that it is PORTB On-Change Interrupt
    {
        RBIE = 0; //Disable On-Change Interrupt
        if (ECHO == 1) { //If ECHO is HIGH
            //TMR1H = 0; //Valor inicial del timer
            //TMR1L = 0; //Valor inicial del timer
            TMR1ON = 1; //Iniciar timer
        }
        if (ECHO == 0) { //If ECHO is LOW
            TMR1ON = 0; //Stop Timer
            d = (TMR1L | (TMR1H << 8)) / 58; //Calculate Distance
        }
    }
    RBIF = 0; //Clear PORTB On-Change Interrupt flag
    RBIE = 1; //Enable PORTB On-Change Interrupt
}

void main(void) {
    PORTB = 0; //Limpiar Puerto B
    TRISB = 0b00010000; //ECHO es entrada

    GIE = 1; //Global Interrupt Enable
    RBIF = 0; //Clear PORTB On-Change Interrupt Flag
    RBIE = 1; //Enable PORTB On-Change Interrupt

    UART_Init(9600); //Iniciar comunicacion serial
    UART_Write_Text("Sensor Ultrasonico"); //Enviar saludo
    UART_Write('\n');

    T1CON = 0x10; //Initialize Timer Module (Prescaler 1:2)

    while (1) {
        // SENSOR 1 -----------------------------------------------------------
        TMR1H = 0; //Valor inicial del timer
        TMR1L = 0; //Valor inicial del timer
        d = 0; //reiniciar distancia
        TRIG1 = 1; //TRIGGER HIGH
        __delay_us(10); //10uS Delay
        TRIG1 = 0; //TRIGGER LOW
        __delay_ms(50); //Waiting for ECHO

        if (d > 0 && d < 400) //Check whether the result is valid or not
        {
            sprintf(buffer, "%d", d); //Formato decimal
            UART_Write_Text(buffer);
            UART_Write(',');
        } else {
            UART_Write('0'); //Escribir 0 si esta fuera de rango
            UART_Write(',');
        }
        
        // SENSOR 2 -----------------------------------------------------------
        TMR1H = 0; //Valor inicial del timer
        TMR1L = 0; //Valor inicial del timer
        d = 0;
        TRIG2 = 1; //TRIGGER HIGH
        __delay_us(10); //10uS Delay
        TRIG2 = 0; //TRIGGER LOW
        __delay_ms(50); //Waiting for ECHO

        if (d > 0 && d < 400) //Check whether the result is valid or not
        {
            sprintf(buffer, "%d", d); //Formato decimal
            UART_Write_Text(buffer);
            UART_Write(',');
        } else {
            UART_Write('0'); //Escribir 0 si esta fuera de rango
            UART_Write(',');
        }

        // SENSOR 3 -----------------------------------------------------------
        TMR1H = 0; //Valor inicial del timer
        TMR1L = 0; //Valor inicial del timer
        d = 0;
        TRIG3 = 1; //TRIGGER HIGH
        __delay_us(10); //10uS Delay
        TRIG3 = 0; //TRIGGER LOW
        __delay_ms(50); //Waiting for ECHO

        if (d > 0 && d < 400) //Check whether the result is valid or not
        {
            sprintf(buffer, "%d", d); //Formato decimal
            UART_Write_Text(buffer);
            UART_Write('\n');
        } else {
            UART_Write('0'); //Escribir 0 si esta fuera de rango
            UART_Write('\n');
        }

        __delay_ms(350);
    }
    return;
}
//FIN DEL CODIGO