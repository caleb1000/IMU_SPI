/*------------------------------------------------------------------------------
  spi.c --
  
  Description:
    Provides useful definitions for manipulating the relevant SPI
    module of the ATxmega128A1U. 
  
  Author(s): Dr. Eric M. Schwartz, Christopher Crary, Wesley Piard
  Last modified by: Christopher Crary
  Last modified on: 18 July 2020
------------------------------------------------------------------------------*/

/********************************DEPENDENCIES**********************************/

#include <avr/io.h>
#include "spi.h"

/*****************************END OF DEPENDENCIES******************************/


/*****************************FUNCTION DEFINITIONS*****************************/


void spi_init(void)
{
	
  /* Initialize the relevant SPI output signals to be in an "idle" state.
   * Refer to the relevant timing diagram within the LSM6DS3 datasheet.
   * (You may wish to utilize the macros defined in `spi.h`.) */
  
  PORTF.OUTSET = (SS_bm ); //set the output of the pins to their false values 
  //note that because SS is active low we have to set its output as high
  
  
  /* Configure the pin direction of relevant SPI signals. */
  PORTF.DIRSET = (MOSI_bm|SCK_bm|SS_bm );//set all of these as outputs
  //Set all outputs

 	PORTF.DIRCLR = (MISO_bm); //set MIS0 as output
	 //Set all inputs
	
	/* Set the other relevant SPI configurations. */
	
	SPIF.CTRL	= SPI_PRESCALER_DIV4_gc | SPI_MASTER_bm | SPI_MODE_3_gc | SPI_ENABLE_bm ;
	//ANDing the dord with zero makes sure that we write a zero in the DORD_bp meaning MSB transfer first 
	//Set that we are not using CLK2x by writing a 0 to that bit position
	//set order of bits, configure transfer mode, set clock frequency, enable master mode, enable SPI module
}

void spi_write(uint8_t data)
{
	//this function takes in some 8 bit data and saves it to the SPIF.DATA reg
	
	/* Write to the relevant DATA register. */
	SPIF.DATA = data;

	/* Wait for relevant transfer to complete. */
	while(!(SPIF.STATUS & SPI_IF_bm)){
		//SPIF.STATUS = SPI_IF_bm; //clear the flag
		};
	
  /* In general, it is probably wise to ensure that the relevant flag is
   * cleared at this point, but, for our contexts, this will occur the 
   * next time we call the `spi_write` (or `spi_read`) routine. 
   * Really, because of how the flag must be cleared within
   * ATxmega128A1U, it would probably make more sense to have some single 
   * function, say `spi_transceive`, that both writes and reads 
   * data, rather than have two functions `spi_write` and `spi_read`,
   * but we will not concern ourselves with this possibility
   * during this semester of the course. */
}

uint8_t spi_read(void)
{
  /* Write some arbitrary data to initiate a transfer. */
  SPIF.DATA = 0x37;

  /* Wait for relevant transfer to be complete. */
  while(!(SPIF.STATUS & SPI_IF_bm)){
	  //wait while the SPIF IF flag is not set, if it is set then proceed 
	//  SPIF.STATUS = SPI_IF_bm; //clear the flag
	  };
	  
	  

	/* After the transmission, return the data that was received. */
	return SPIF.DATA;
}

/***************************END OF FUNCTION DEFINITIONS************************/
