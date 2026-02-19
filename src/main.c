#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <avr/interrupt.h>
#include "I2C.h"
#include "ssd1306.h" 

bool olestate = false;
bool Iflag = false;
bool isbuttonactive();
void init(){
	PORTK|=0xFF;
	
    
    DDRE &=~(1<<5);
    PORTE |=(1<<5); //internal pullup on digital pin 3

    EICRB = 0x00; 
    EIMSK |= (1<<INT5); // enable INT5 interrupt
    sei(); // global interrupt

	DDRK &= ~(1<<0);



  PORTK|=(0xFF);
}
int counter = 0;
int main(void)
{  
    init(); 
  
    I2C_Init();  //initialize i2c interface to display

    InitializeDisplay(); //initialize  display


    int num [3] ={0}; //array til at holde værdierne fra dipswitchen samt sidste entry til at holde resultatet

    char numstring[4][8] = {0}; // Array til at holde værdierne fra dipswitchen, så det kan printes.


    for (int index = 0; index <= 3; ++index) // sætter lige hexa kode på alle strengene
    {
        if (index != 2){
            strcpy(numstring[index],"0x");
        }
        
    }
  clear_display();  
  while (1)
  {
    if (isbuttonactive()) //Checker om interupt flaget er blevet sat
    {
        // Læser første værdi sat på dip switchen ind
        // og sørger for at den bliver sat ind i en streng og sendt til oled skærmen
        num[0] = PINK; 
        itoa(num[0],numstring[0]+2,16);
        sendStrXY(numstring[0],0,0);
        _delay_ms(500);

        while (!isbuttonactive()); // Venter på næste tryk
        
        // Læser anden værdi sat på dip switchen in
        // og sørger for at den bliver sat ind i en streng og sendt til oled skærmen
        num[1] = PINK;
        itoa(num[1],numstring[1]+2,16);
        sendStrXY(numstring[1],4,0);
        _delay_ms(500);


        while (!isbuttonactive());
        switch (PINK)
        {
        case 0xff: // AND case
            strcat(numstring[2], "AND");
            num[2] = num[0] & num[1];
            itoa(num[2],numstring[3]+2,16);
            break;
        case 0xfe: // Or Case
            strcat(numstring[2], "OR");
            num[2] = num[0] | num[1];
            itoa(num[2],numstring[3]+2,16);
            break;
        case 0x7f: // AND NOT Case
            strcat(numstring[2], "AND NOT");
            num[2] = num[0] &~ num[1];
            itoa(num[2],numstring[3]+2,16);
            break;
        default: // Ole's special Case
            clear_display();
            sendStrXY("OLE HVA FAEN",3,0);
            
            break;
        }
        
        

        sendStrXY(numstring[2],2,0);
        sendStrXY(numstring[3],6,0);
        
        _delay_ms(500);
        

        
       

        while (!isbuttonactive()); // Venter på button reset tryk
        
        numstring[2][0] = '\0'; // nul terminerer strengen, så vi kan genbruge den
        clear_display();
        _delay_ms(500);
    }
  } 
}

bool isbuttonactive()
 {
    if (Iflag) // Interrupt flag
    {
        _delay_ms(50);
        Iflag = false;
        return true;
    }
     else return false;
 }

 ISR (INT5_vect)
 {
    Iflag = true;
 }