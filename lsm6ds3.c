/*------------------------------------------------------------------------------
  lsm6ds3.c --
  
  Description:
    Brief description of file.
	  
	  Extended description, if appropriate.
  
  Author(s):
  Last modified by: 
  Last modified on: DD Month YYYY
------------------------------------------------------------------------------*/

/********************************DEPENDENCIES**********************************/

#include "usart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "lsm6ds3_registers.h"
#include "lsm6ds3.h"
/*****************************END OF DEPENDENCIES******************************/


/*****************************FUNCTION DEFINITIONS*****************************/

/* INSERT YOUR LSM6DS3 FUNCTION DEFINITIONS BELOW. */
void lsm6ds3_write(uint8_t reg_addr, uint8_t data){
	//Set SS bit low meaning true enabling slave
	PORTF.OUTCLR = PIN4_bm;
	
	spi_write(reg_addr | LSM6DS3_SPI_WRITE_STROBE_bm); // set write bit OR with reg address
	spi_write(data); //here we are writing the data to the register address
	
	PORTF.OUTSET = PIN4_bm; //now disable slave again

} 

	
uint8_t lsm6ds3_read(uint8_t reg_addr){
	uint8_t data; //make an int named data to hold the data value we are going to read
	PORTF.OUTCLR = PIN4_bm; // set SS(L) low making it true enabling slave
	spi_write(reg_addr | LSM6DS3_SPI_READ_STROBE_bm); // now we are going to use the address and read strobe to determine where we are going to put the location
	data = spi_read();
	PORTF.OUTSET = PIN4_bm; //now disable slave again

	return data;
}

//configure ctrl1_XL, ctrl9_XL, and INT1_CTR
void lsm6ds3_init(void){
	
	 //add functionality software reset LSM6DS3 using CTRL3_C
	 lsm6ds3_write(CTRL3_C, 0b00000101); //set the reboot pin of ctrl3 and just use defaults
	
	 
	 //use 208 Hz mode (0101) as recommended in lab might need to change
	 //use +2g mode which is (00) for Scale
	 //use 400 Hz filter (00) for filter
	 lsm6ds3_write(CTRL1_XL,0b01010000); //CTRL1_XL (OUTPUT[3:0],Scale[1:0], filter[1:0])
	 
	 //might not work (CALEB) delete if doesn't work
	// lsm6ds3_write(CTRL7_G, 0b00000000); // setting up gyro
	// lsm6ds3_write(CTRL2_G, 0b01010000);
	 //might not work (CALEB) delete if doesn't work
	 
	 //we need to enable the x,y, and z access
	 // default (00), Zen, Yen, Xen, Soft_EN, default(00)
	 //X,Y, and Z enabled and soft-iron correction turned off
	 lsm6ds3_write(CTRL9_XL, 0b00111000);
	 
	 
	 //we need to set flag when accelerometer data is ready
	 //INT1_DRDY_XL needs to be set to allow for this interrupt to get sent on INT1
	 lsm6ds3_write(INT1_CTRL,0x01); //for accelerometer
	//lsm6ds3_write(INT1_CTRL,0b00000010);//enable gyro int
	
	//configure I/O port interrupt on the xmega side in main program
}
/***************************END OF FUNCTION DEFINITIONS************************/