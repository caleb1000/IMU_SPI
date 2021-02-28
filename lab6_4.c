/*
 * lab6_4.c
 *
 * Created: 7/21/2020 1:06:26 PM
 *  Author: Caleb Steinmetz
 *PI: Jake Sparks
 * Description: This program simply sets up portC for i/o interrupts for INT1
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "lsm6ds3_registers.h"
#include "lsm6ds3.h"
//prototypes

void intr_init(void);
void portc_intr_init(void);

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
	//lsmread and write work
	//uint8_t temp;
	//	temp = lsm6ds3_read(CTRL1_XL);	
	while(1){
			lsm6ds3_write(INT1_CTRL,0x00); //disable interrupt
			lsm6ds3_write(INT1_CTRL,0x01); //re-enable interrupt
			//I still have the slave ss line high

	}


	
	//SPIC_INT_vect  we will use this later
	
}

ISR(PORTC_INT0_vect){
	//mask0 used
	PORTC_INTFLAGS = PORT_INT0IF_bm;//clear flag

	//interrupt cause by PC6
	
	
}