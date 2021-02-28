/*
 * lab6_Gyro.c
 *
 * Created: 7/21/2020 1:06:26 PM
 *  Author: Caleb Steinmetz
 *PI: Jake Sparks
 * Description: This program simply writes out via usart the gyro data
 */ 

#include <stdio.h> 
#include "usart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "lsm6ds3_registers.h"
#include "lsmGyro.h"
//prototypes

void intr_init(void);
void portc_intr_init(void);

#define OOTB_UPAD_RGB_RED_bp (4) //bit position
#define OOTB_UPAD_RGB_RED_bm (1<<OOTB_UPAD_RGB_RED_bp)

#define OOTB_UPAD_RGB_GREEN_bp (5)
#define OOTB_UPAD_RGB_GREEN_bm (1<<OOTB_UPAD_RGB_GREEN_bp)

#define OOTB_UPAD_RGB_BLUE_bp (6)
#define OOTB_UPAD_RGB_BLUE_bm (1<<OOTB_UPAD_RGB_BLUE_bp)

#define OOTB_UPAD_RGB_gm (OOTB_UPAD_RGB_RED_bm | OOTB_UPAD_RGB_GREEN_bm | OOTB_UPAD_RGB_BLUE_bm)

volatile int accel_flag = 0; //create accel_flag
volatile int gyro_flag = 0; //create gyro_flag

volatile int accel_flag2 = 0; //create accel_flag
volatile int gyro_flag2 = 0; //create gyro_flag

void intr_init(void){
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm; //enable low level interrupt
	sei();
}

void portc_intr_init(void){
	PORTC_INT0MASK = 0b01000000; //set up interrupt for PC6 (INT1) gyro
	PORTC_INT1MASK = 0b10000000; //set up interrupt for PC7 (INT2) accel
	PORTC_OUTCLR = 0b11000000; //set PC6 default low and PC7
	PORTC_DIRCLR = 0b11000000; //set PC6 as input and PC7
	PORTC_INTCTRL = 0b00000101; //select the external interrupt as a low prio level 
	PORTC_PIN6CTRL = PORT_ISC_RISING_gc; // rising edge pin for external interrupt (input sense)
	PORTC_PIN7CTRL = PORT_ISC_RISING_gc;
	/*0100 0000
	  ^^^^ ^^^^
	  7654 3210 bit position
	  interrupt cause by PC6 
	*/
		PORTE_INT0MASK = 0b00000001; //set up interrupt for PC6 (INT1) gyro
		PORTE_PIN0CTRL = PORT_ISC_RISING_gc; // rising edge pin for external interrupt (input sense)
		PORTE_INT1MASK = 0b00000010; //set up interrupt for PC6 (INT1) gyro
		PORTE_PIN1CTRL = PORT_ISC_RISING_gc; // rising edge pin for external interrupt (input sense)
		PORTE_DIRCLR = 0b00000011; //Set button as input 
		PORTE_INTCTRL = 0b00000101; //select the external interrupt as a low prio level for both ints
		
}

void tcc0_init(void){
		TCC0.CNT = 0; //sets the count to zero
		//250e^-3 = .25 seconds
		//TCC0.PER = (2000000/1024) * 250e-3;
		TCC0.PER = (2000000/1024) * 100e-3; //50ms
		TCC0.CTRLB = TC_WGMODE_NORMAL_gc;
		TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc; //clear ovfif and enable low level interupts
	}
	void tcc1_init(void){
		TCC1.CNT = 0; //sets the count to zero
		//250e^-3 = .25 seconds
		//TCC1.PER = (2000000/1024) * 250e-3;
		TCC1.PER = (2000000/1024) * 50e-3; //50ms
		TCC1.CTRLB = TC_WGMODE_NORMAL_gc;
		TCC1.INTCTRLA = TC_OVFINTLVL_LO_gc; //clear ovfif and enable low level interupts
	}

int main(void)
{
	
	
	//configure I/O port interrupt on the xmega side in main program

	spi_init(); // init the spi
	portc_intr_init();//init port c i/o interrupt
	lsm6ds3_init();//init the lsm
	tcc0_init();
	tcc1_init();
	intr_init();//init all low/med level interrupts
	usartd0_init();
	PORTD.OUTSET = OOTB_UPAD_RGB_gm; //PORTD_OUTSET is equal to PORTD.OUTSET
	PORTD.DIRSET = OOTB_UPAD_RGB_gm;
	
	int gyro_max = 7000;
	int gyro_min = -7000;
	int accel_max = 15000;
	int accel_min =-15000;
	
	while(1){
		
		if((gyro_flag == 1)&(gyro_flag2 == 1)&(accel_flag2 == 0)){
			char data[6];
			int index = 0;
			data[index]=lsm6ds3_read(OUTX_L_G);
			index++;
			data[index]=lsm6ds3_read(OUTX_H_G);
			
			int16_t X = ((uint8_t )data[index] << 8 )|((uint8_t )data[index-1]);
			
			index++;
			data[index]=lsm6ds3_read(OUTY_L_G);
			index++;
			data[index]=lsm6ds3_read(OUTY_H_G);
			int16_t Y = ((uint8_t )data[index] << 8 )|((uint8_t )data[index-1]);
			
			index++;
			data[index]=lsm6ds3_read(OUTZ_L_G);
			index++;
			data[index]=lsm6ds3_read(OUTZ_H_G);
			int16_t Z = ((uint8_t )data[index] << 8 )|((uint8_t )data[index-1]);
			
			if((X>gyro_max)|(X<gyro_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_RED_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_RED_bm;
			}
			if((Y>gyro_max)|(Y<gyro_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_BLUE_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_BLUE_bm;
			}
			if((Z>gyro_max)|(Z<gyro_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_GREEN_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_GREEN_bm;
			}
			//send this data string to the usartd0_out_string
			usartd0_out_string(data);
			gyro_flag = 0;
		}
		
		
		if((accel_flag == 1)&(accel_flag2 == 1)&(gyro_flag2 == 0)){
			char data[6];
			int index = 0;
			data[index]=lsm6ds3_read(OUTX_L_XL);
			index++;
			data[index]=lsm6ds3_read(OUTX_H_XL);
			
			int16_t X = ((uint8_t )data[index] << 8 )|((uint8_t )data[index-1]);
			
			index++;
			data[index]=lsm6ds3_read(OUTY_L_XL);
			index++;
			data[index]=lsm6ds3_read(OUTY_H_XL);
			int16_t Y = ((uint8_t )data[index] << 8 )|((uint8_t )data[index-1]);
			
			index++;
			data[index]=lsm6ds3_read(OUTZ_L_XL);
			index++;
			data[index]=lsm6ds3_read(OUTZ_H_XL);
			int16_t Z = ((uint8_t )data[index] << 8 )|((uint8_t )data[index-1]);
			
			if((X>accel_max)|(X<accel_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_RED_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_RED_bm;
			}
			if((Y>accel_max)|(Y<accel_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_BLUE_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_BLUE_bm;
			}
			if((Z>accel_max)|(Z<accel_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_GREEN_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_GREEN_bm;
			}
			//send this data string to the usartd0_out_string
			usartd0_out_string(data);
			accel_flag = 0;
		}
		
		if((gyro_flag == 1)&(gyro_flag2 == 1)&(accel_flag == 1)&(accel_flag2 == 1)){
			char gyro[6];
			char accel[6];
			int index = 0;
			
			
			gyro[index]=lsm6ds3_read(OUTX_L_G);
			index++;
			gyro[index]=lsm6ds3_read(OUTX_H_G);
			
			int16_t X = ((uint8_t )gyro[index] << 8 )|((uint8_t )gyro[index-1]);
			
			index++;
			gyro[index]=lsm6ds3_read(OUTY_L_G);
			index++;
			gyro[index]=lsm6ds3_read(OUTY_H_G);
			int16_t Y = ((uint8_t )gyro[index] << 8 )|((uint8_t )gyro[index-1]);
			
			index++;
			gyro[index]=lsm6ds3_read(OUTZ_L_G);
			index++;
			gyro[index]=lsm6ds3_read(OUTZ_H_G);
			int16_t Z = ((uint8_t )gyro[index] << 8 )|((uint8_t )gyro[index-1]);
			
			if((X>gyro_max)|(X<gyro_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_RED_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_RED_bm;
			}
			if((Y>gyro_max)|(Y<gyro_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_BLUE_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_BLUE_bm;
			}
			if((Z>gyro_max)|(Z<gyro_min)){
				PORTD_OUTCLR = OOTB_UPAD_RGB_GREEN_bm;
			}
			else{
				PORTD_OUTSET = OOTB_UPAD_RGB_GREEN_bm;
			}
			
		
				 index = 0;
			
				accel[index]=lsm6ds3_read(OUTX_L_XL);
				index++;
				accel[index]=lsm6ds3_read(OUTX_H_XL);
				
				X = ((uint8_t )accel[index] << 8 )|((uint8_t )accel[index-1]);
				
				index++;
				accel[index]=lsm6ds3_read(OUTY_L_XL);
				index++;
				accel[index]=lsm6ds3_read(OUTY_H_XL);
				Y = ((uint8_t )accel[index] << 8 )|((uint8_t )accel[index-1]);
				
				index++;
				accel[index]=lsm6ds3_read(OUTZ_L_XL);
				index++;
				accel[index]=lsm6ds3_read(OUTZ_H_XL);
				Z = ((uint8_t )accel[index] << 8 )|((uint8_t )accel[index-1]);
				
				if((X>accel_max)|(X<accel_min)){
					PORTD_OUTCLR = OOTB_UPAD_RGB_RED_bm;
				}
				else{
					PORTD_OUTSET = OOTB_UPAD_RGB_RED_bm;
				}
				if((Y>accel_max)|(Y<accel_min)){
					PORTD_OUTCLR = OOTB_UPAD_RGB_BLUE_bm;
				}
				else{
					PORTD_OUTSET = OOTB_UPAD_RGB_BLUE_bm;
				}
				if((Z>accel_max)|(Z<accel_min)){
					PORTD_OUTCLR = OOTB_UPAD_RGB_GREEN_bm;
				}
				else{
					PORTD_OUTSET = OOTB_UPAD_RGB_GREEN_bm;
				}
				//send this data string to the usartd0_out_string
				usartd0_out_string(gyro);
				usartd0_out_string(accel);
				gyro_flag = 0;
				accel_flag = 0;
			
		}
		
		
		
		
	}
	//SPIC_INT_vect  we will use this later
	
}

ISR(PORTC_INT1_vect){
	//mask1 used for interrupt
	accel_flag = 1;// set flag
	PORTC_INTFLAGS = PORT_INT1IF_bm;//clear flag
	//interrupt cause by PC7
}
ISR(PORTC_INT0_vect){
	//mask0 used for interrupt
	gyro_flag = 1;// set flag
	PORTC_INTFLAGS = PORT_INT0IF_bm;//clear flag
	//interrupt cause by PC6
}
ISR(PORTE_INT0_vect){

	PORTE_INTCTRL = 0b00000000; //turn off porte interrupts
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc; //set prescaler
	//mask0 used for interrupt
	//interrupt cause by PC6
}
ISR(PORTE_INT1_vect){
	PORTE_INTCTRL = 0b00000000; //turn off porte interrupts
	TCC1.CTRLA = TC_CLKSEL_DIV1024_gc;
	//mask1 used for interrupt
	//interrupt cause by PC6
}
ISR(TCC0_OVF_vect){
	TCC0.CTRLA = TC_CLKSEL_OFF_gc;
	TCC0.CNT = 0;
	if ((PORTE.IN & 0b00000001)){
			
			
			if(gyro_flag2 == 1){
				gyro_flag2 = 0;// set flag
				PORTD_OUTSET = OOTB_UPAD_RGB_gm;
			}
		
			else{
				gyro_flag2 = 1;// set flag
				PORTD_OUTSET = OOTB_UPAD_RGB_gm;
			}
	}
		PORTE_INTFLAGS = PORT_INT0IF_bm | PORT_INT1IF_bm;//clear flag
		PORTE_INTCTRL = 0b00000101; //turn on porte interrupts
		TCC0_INTFLAGS = 0b00000001; //clear flag 0
}

ISR(TCC1_OVF_vect){
	TCC1.CTRLA = TC_CLKSEL_OFF_gc;
	TCC1.CNT = 0;
	if ((PORTE.IN & 0b00000010)){
	


	if(accel_flag2 == 1){
		accel_flag2 = 0;// set flag
		PORTD_OUTSET = OOTB_UPAD_RGB_gm;
	}
	
	else{
		accel_flag2 = 1;// set flag
		PORTD_OUTSET = OOTB_UPAD_RGB_gm;
	}
	}
	PORTE_INTFLAGS = PORT_INT1IF_bm | PORT_INT0IF_bm;//clear flag
	PORTE_INTCTRL = 0b00000101; //turn on porte interrupts
	TCC1_INTFLAGS = 0b00000001; //clear flag 1
}