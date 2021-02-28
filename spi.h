#ifndef SPI_H_		// Header guard.
#define SPI_H_

/*------------------------------------------------------------------------------
  spi.h --
  
  Description:
    Provides function prototypes and macro definitions for utilizing the SPI
    system of the ATxmega128A1U. 
  
  Author(s): Dr. Eric M. Schwartz, Christopher Crary, Wesley Piard
  Last modified by: Christopher Crary
  Last modified on: 18 July 2020
------------------------------------------------------------------------------*/

/********************************DEPENDENCIES**********************************/

#include <avr/io.h>

/*****************************END OF DEPENDENCIES******************************/

/***********************************MACROS*************************************/

#define SS_bm     (1<<4)  // I added to this file from the backpack schematic
#define MOSI_bm	  (1<<5)
#define MISO_bm	  (1<<6)
#define SCK_bm    (1<<7)

/********************************END OF MACROS*********************************/

/*****************************FUNCTION PROTOTYPES******************************/

/*------------------------------------------------------------------------------
  spi_init -- 
  
  Description:
    Initializes the relevant SPI module to communicate with the LSM6DS3.

  Input(s): N/A
  Output(s): N/A
------------------------------------------------------------------------------*/
void spi_init(void);

/*------------------------------------------------------------------------------
  spi_write -- 
  
  Description:
    Transmits a single byte of data via the relevant SPI module.

  Input(s): `data` - 8-bit value to be written via the relevant SPI module. 
  Output(s): N/A
------------------------------------------------------------------------------*/
void spi_write(uint8_t data);

/*------------------------------------------------------------------------------
  spi_read -- 
  
  Description:
    Reads a byte of data via the relevant SPI module.

  Input(s): N/A
  Output(s): 8-bit value read from the relevant SPI module.
------------------------------------------------------------------------------*/
uint8_t spi_read(void);

/**************************END OF FUNCTION PROTOTYPES**************************/

#endif // End of header guard.