/*
 * lab6_2.c
 *
 * Created: 7/21/2020 1:06:26 PM
 *  Author: Caleb Steinmetz
 *PI: Jake Sparks
 * Description: This program simply writes out hex 0x53 via the MOSI line
 */ 
#include <avr/io.h>
#include "spi.h"
//prototypes

int main(void)
{
	uint8_t receive;
	uint8_t data = 0x53;
	spi_init(); // init the spi
	while(1){
	PORTF.OUTCLR = PIN4_bm;
	spi_write(data);
	receive = spi_read();
	PORTF.OUTSET = PIN4_bm;
	
	
	}
	
}