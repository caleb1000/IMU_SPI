/*
 * lab6_5.c
 *
 * Created: 7/21/2020 1:06:26 PM
 *  Author: Caleb Steinmetz
 *PI: Jake Sparks
 * Description: This program simply writes out via usart the accel data
 */ 


#include "usart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "lsm6ds3_registers.h"
#include "lsm6ds3.h"


#define OOTB_UPAD_RGB_RED_bp (4) //bit position
#define OOTB_UPAD_RGB_RED_bm (1<<OOTB_UPAD_RGB_RED_bp)

#define OOTB_UPAD_RGB_GREEN_bp (5)
#define OOTB_UPAD_RGB_GREEN_bm (1<<OOTB_UPAD_RGB_GREEN_bp)

#define OOTB_UPAD_RGB_BLUE_bp (6)
#define OOTB_UPAD_RGB_BLUE_bm (1<<OOTB_UPAD_RGB_BLUE_bp)

#define OOTB_UPAD_RGB_gm (OOTB_UPAD_RGB_RED_bm | OOTB_UPAD_RGB_GREEN_bm | OOTB_UPAD_RGB_BLUE_bm)


//prototypes
void intr_init(void);
void portc_intr_init(void);

volatile int accel_flag = 0; //create accel_flag

void intr_init(void){
	PMIC.CTRL = PMIC_LOLVLEN_bm; //enable low level interrupt
	sei();
}

void portc_intr_init(void){
	PORTC_INT0MASK = 0b01000000; //set up interrupt for PC6 (INT1)
	PORTC_OUTCLR = 0b01000000; //set PC6 default low
	PORTC_DIRCLR = 0b01000000; //set PC6 as input
	PORTC_INTCTRL = 0b00000001; //select the external interrupt as a low prio level 
	PORTC_PIN6CTRL = PORT_ISC_RISING_gc; // rising edge pin for external interrupt (input sense)
	/*0100 0000
	  ^^^^ ^^^^
	  7654 3210 bit position
	  interrupt cause by PC6 
	*/
}

int main(void)
{
	
	
	//configure I/O port interrupt on the xmega side in main program

	spi_init(); // init the spi
	portc_intr_init();//init port c i/o interrupt
	lsm6ds3_init();//init the lsm
	intr_init();//init all low level interrupts
	usartd0_init();
	
	PORTC_OUTSET = 0b00010111;//turn on motor to full power
	PORTC_DIRSET = 0b00011111;//set as output
	PORTD.OUTSET = OOTB_UPAD_RGB_gm; //PORTD_OUTSET is equal to PORTD.OUTSET
	PORTD.DIRSET = OOTB_UPAD_RGB_gm;
	
	while(1){
		
		if(accel_flag == 1){
			char data[6];
			char colordata[12];
			int index = 0;
			int cindex = 0;
			data[index]=lsm6ds3_read(OUTX_L_XL);
			index++;
			data[index]=lsm6ds3_read(OUTX_H_XL);
			if(((data[index]>=0x00) & (data[index]<=0x7F))&((data[index-1]>=0x00) & (data[index-1]<=0xFF))){
				PORTD.OUTCLR = OOTB_UPAD_RGB_RED_bm ; 
				//playing around with leds to see if
				//I can detect gravity!!!
				//if the number is large in the X turn on red
				colordata[cindex] = '+';
				cindex++;
				colordata[cindex] ='X';
				cindex++;
			}
			else if(((data[index]>=0x80) & (data[index]<=0xFF))&((data[index-1]>=0x00) & (data[index-1]<=0xFF))){
				PORTD.OUTSET = OOTB_UPAD_RGB_RED_bm ;
				colordata[cindex] = '-';
				cindex++;
				colordata[cindex] ='X';
				cindex++;
				//if turned off aka
				}
			index++;
			data[index]=lsm6ds3_read(OUTY_L_XL);
			index++;
			data[index]=lsm6ds3_read(OUTY_H_XL);
			if(((data[index]>=0x00) & (data[index]<=0x7F))&((data[index-1]>=0x00) & (data[index-1]<=0xFF))){
				PORTD.OUTCLR = OOTB_UPAD_RGB_BLUE_bm;
				colordata[cindex] = '+';
				cindex++;
				colordata[cindex] ='Y';
				cindex++;
			//	PORTC_OUTSET = 0b00010101;//turn on motor to full power
				//if the number is large in the Y turn on blue
			}
			else if(((data[index]>=0x80) & (data[index]<=0xFF))&((data[index-1]>=0x00) & (data[index-1]<=0xFF))){
					PORTD.OUTSET = OOTB_UPAD_RGB_BLUE_bm ;
					colordata[cindex] = '-';
					cindex++;
					colordata[cindex] ='Y';
					cindex++;
				
					
			}
			index++;
			data[index]=lsm6ds3_read(OUTZ_L_XL);
			index++;
			data[index]=lsm6ds3_read(OUTZ_H_XL);
			if(((data[index]>=0x00) & (data[index]<=0x7F))&((data[index-1]>=0x00) & (data[index-1]<=0xFF))){
				//int twos comp hex 0x3A is 16000 (max pos is 32,767) or
				//if the number is large in the Z turn on green
				PORTD.OUTCLR = OOTB_UPAD_RGB_GREEN_bm;
				colordata[cindex] = '+';
				cindex++;
				colordata[cindex] ='Z';
				cindex++;
					PORTC_OUTSET = 0b00010111;//turn on motor to full power
			}
			else if(((data[index]>=0x80) & (data[index]<=0xFF))&((data[index-1]>=0x00) & (data[index-1]<=0xFF))){
				PORTD.OUTSET = OOTB_UPAD_RGB_GREEN_bm;
				colordata[cindex] = '-';
				cindex++;
				colordata[cindex] ='Z';
				cindex++;
					if(data[index] >= 0b11011000){
						PORTC_OUTCLR = 0b00010111;//turn on motor to full power
						//PORTC_DIRSET = 0b00010101;//turn on motor to full power
					}
					
			}
			
			//send this data string to the usartd0_out_string
			usartd0_out_string(data);
			accel_flag = 0;
			PORTD.OUTSET = OOTB_UPAD_RGB_gm;
		}
	}
	//SPIC_INT_vect  we will use this later
	
}

ISR(PORTC_INT0_vect){
	//mask0 used for interrupt
	accel_flag = 1;// set flag
	PORTC_INTFLAGS = PORT_INT0IF_bm;//clear flag
	//interrupt cause by PC6
}