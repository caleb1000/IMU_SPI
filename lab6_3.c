/*
 * lab6_3.c
 *
 * Created: 7/21/2020 1:06:26 PM
 *  Author: Caleb Steinmetz
 *PI: Jake Sparks
 * Description: Reads from who am I register
 */ 


#include <avr/io.h>
#include "spi.h"
#include "lsm6ds3_registers.h"
#include "lsm6ds3.h"
//prototypes


int main(void)
{
	
	spi_init(); // init the spi
	uint8_t temp;
	while(1){
		
	temp = lsm6ds3_read(WHO_AM_I);	
	
	}
	
	
	
	
}