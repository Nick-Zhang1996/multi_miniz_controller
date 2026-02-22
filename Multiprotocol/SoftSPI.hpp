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
#pragma once
#include <Arduino.h>

// 3 wire spi
class SoftSPI {
	private:
	uint8_t sdi_pin_;
	uint8_t sdi_bm_; // bitmask for the bit for the pin
    volatile uint8_t* sdi_port_;
    volatile uint8_t* sdi_ddr_;
    volatile uint8_t* sdi_ipr_;

	uint8_t sck_pin_;
	uint8_t sck_bm_; // bitmask for the bit for the pin
    volatile uint8_t* sck_port_;
    volatile uint8_t* sck_ddr_;

	public:
	explicit SoftSPI(uint8_t sdio_pin, uint8_t sck_pin):
	sdi_pin_{sdio_pin}, sck_pin_{sck_pin}{
		sdi_bm_ = digitalPinToBitMask(sdi_pin_);
		uint8_t port = digitalPinToPort(sdi_pin_);
		sdi_port_ = portOutputRegister(port);
		sdi_ddr_ = portModeRegister(port);
		sdi_ipr_ = portInputRegister(port);

		sck_bm_ = digitalPinToBitMask(sck_pin_);
		port = digitalPinToPort(sck_pin_);
		sck_port_ = portOutputRegister(port);
		sck_ddr_ = portModeRegister(port);
	}

	void begin(){
		*sdi_ddr_ |= sdi_bm_; // output
		*sck_ddr_ |= sck_bm_;
		*sdi_port_ |= sdi_bm_; // high
		*sck_port_ &= ~sck_bm_; // low
	}


	void write(uint8_t command)
	{
		uint8_t n=8; 

		// SCK start low
		*sck_port_ &= ~sck_bm_; // low
		*sdi_port_ &= ~sdi_bm_; // low
		do
		{
			if(command&0x80)
				*sdi_port_ |= sdi_bm_; // high
			else
				*sdi_port_ &= ~sdi_bm_; // low
			*sck_port_ |= sck_bm_; // high
			command = command << 1;
			*sck_port_ &= ~sck_bm_; // low
		}
		while(--n) ; 
		*sdi_port_ |= sdi_bm_; // high
	}

	uint8_t read(void)
	{
		uint8_t result=0;
		*sdi_ddr_ &= ~sdi_bm_; // input
		for(int i=0;i<8;i++)
		{
			result = result << 1;
			if((*sdi_ipr_) & sdi_bm_)
				result |= 0x01;
			*sck_port_ |= sck_bm_; // high
			_NOP(); // TODO necessary?
			*sck_port_ &= ~sck_bm_; // low
		}
		*sdi_ddr_ |= sdi_bm_; // output
		return result;
	}

};
extern SoftSPI soft_spi; // to be defined in global scope in main
