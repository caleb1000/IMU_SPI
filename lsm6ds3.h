#ifndef LSM6DS3_H_  // Header guard.
#define LSM6DS3_H_

/*------------------------------------------------------------------------------
  lsm6ds3.h --
  
  Description:
    Provides custom data types to make it easier to handle any data
    read from the LSM6DS3 IMU. 
	  
	  The LSM6DS3 can output accelerometer and gyroscope data. Data from both
    of these sensors is represented in a 16-bit signed format. 
  
  Author(s): Wesley Piard
  Last modified by: Christopher Crary
  Last modified on: 17 July 2020
------------------------------------------------------------------------------*/


/***********************************MACROS*************************************/

#define LSM6DS3_SPI_READ_STROBE_bm 				0x80
#define LSM6DS3_SPI_WRITE_STROBE_bm				0x00

/********************************END OF MACROS*********************************/


/*******************************CUSTOM DATA TYPES******************************/

/* Used to differentiate the accelerometer and gyroscope within the LSM6DS3. */
typedef enum {LSM6DS3_ACCEL, LSM6DS3_GYRO} lsm6ds3_module_t;

/* Can be used to contain the separated bytes of data as they are read from
 * the LSM6DS3. */
typedef struct lsm6ds3_data_raw
{
  uint8_t accel_x_low, accel_x_high;
  uint8_t accel_y_low, accel_y_high;
  uint8_t accel_z_low, accel_z_high;

  uint8_t gyro_x_low, gyro_x_high;
  uint8_t gyro_y_low, gyro_y_high;
  uint8_t gyro_z_low, gyro_z_high;
}lsm6ds3_data_raw_t;

/* Contains the full concatenated signed 16-bit words of data. */
typedef struct lsm6ds3_data_full
{
  int16_t accel_x, accel_y, accel_z;
  int16_t gyro_x, gyro_y, gyro_z;
}lsm6ds3_data_full_t;

/* Provides the ability to choose how to access the LSM6DS3 data. */
typedef union lsm6ds3_data
{
  lsm6ds3_data_full_t  word;
  lsm6ds3_data_raw_t   byte;
}lsm6ds3_data_t;

/***************************END OF CUSTOM DATA TYPES***************************/


/*****************************FUNCTION PROTOTYPES******************************/

/* INSERT YOUR LSM6DS3 FUNCTION PROTOTYPES HERE. */
void lsm6ds3_write(uint8_t reg_addr, uint8_t data);

uint8_t lsm6ds3_read(uint8_t reg_addr);

void lsm6ds3_init(void);

/**************************END OF FUNCTION PROTOTYPES**************************/

#endif // End of header guard.
