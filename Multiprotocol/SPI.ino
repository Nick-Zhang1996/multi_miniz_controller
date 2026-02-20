/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Multiprotocol is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Multiprotocol.  If not, see <http://www.gnu.org/licenses/>.
 */
/********************/
/**  SPI routines  **/
/********************/
#include <Arduino.h>
#include "Pins.h"

//#define SOFTWARE_SPI
#ifdef SOFTWARE_SPI
void SPI_Write(uint8_t command)
{
	uint8_t n=8; 

	SCLK_off;//SCK start low
	SDI_off;
	do
	{
		if(command&0x80)
			SDI_on;
		else
			SDI_off;
		SCLK_on;
		command = command << 1;
		SCLK_off;
	}
	while(--n) ; 
	SDI_on;
}

uint8_t SPI_Read(void)
{
	uint8_t result=0,i;
	for(i=0;i<8;i++)
	{
		result=result<<1;
		if(SDO_1)
			result |= 0x01;
		SCLK_on;
		NOP();
		SCLK_off;
	}
	return result;
}

// For 3 wire SPI
uint8_t SPI_SDI_Read(void)
{
	uint8_t result=0;
	SDI_input;
	for(uint8_t i=0;i<8;i++)
	{                    
		result=result<<1;
		if(SDI_1)  ///if SDIO =1 
			result |= 0x01;
		SCLK_on;
		NOP();
		SCLK_off;
	}
	SDI_output;
	return result;
}
#else
#include <SPI.h>

// Define your Chip Select pin
const int SCS_PIN = 10; 

// A7105 SPI Settings: 4MHz, MSB First, SPI Mode 0
SPISettings a7105Settings(4000000, MSBFIRST, SPI_MODE0);

// Replaces your manual SPI_Write
void SPI_Write(uint8_t command) {
    SPI.beginTransaction(a7105Settings);
    digitalWrite(SCS_PIN, LOW);
    
    SPI.transfer(command);
    
    digitalWrite(SCS_PIN, HIGH);
    SPI.endTransaction();
}

// Replaces your manual SPI_SDI_Read (3-wire mode)
uint8_t SPI_SDI_Read(uint8_t regAddress) {
    uint8_t result = 0;

    SPI.beginTransaction(a7105Settings);
    digitalWrite(SCS_PIN, LOW);
    
    // Step 1: Send the register address you want to read
    // For A7105, the read bit is usually the address | 0x40 or similar 
    // depending on your specific command byte format.
    SPI.transfer(regAddress); 
    
    // Step 2: Read the value back
    // We send a dummy byte (0x00) to trigger the clock cycles needed to read
    result = SPI.transfer(0x00); 
    
    digitalWrite(SCS_PIN, HIGH);
    SPI.endTransaction();
    
    return result;
}

#endif