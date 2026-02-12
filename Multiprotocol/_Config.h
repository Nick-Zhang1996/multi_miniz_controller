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

#define A7105_INSTALLED

/** A7105 Fine Frequency Tuning **/
// This is required in rare cases where some A7105 modules and/or RXs have an
// inaccurate crystal oscillator. If using Serial mode only (for now), you can
// use CH15 to find the right tuning value. -100%=-300, 0%=default 0,
// +100%=+300. Uncomment the line below (remove the "//") to enable this
// feature. #define USE_A7105_CH15_TUNING

// Once a good tuning value is found it can be set here and will override the
// frequency tuning for a specific protocol. Uncomment the lines below (remove
// the "//") and set an appropriate value (replace the "0") to enable. Valid
// range is -300 to +300 and default is 0. #define FORCE_AFHDS2A_TUNING	0
// #define FORCE_KYOSHO_TUNING	0

/** Low Power **/
// Low power is reducing the transmit power of the multi module. This setting is
// configurable per model in PPM (table below) or Serial mode (radio GUI). It
// can be activated when flying indoor or small models since the distance is
// short or if a model is causing issues when flying closed to the TX. By
// default low power selection is enabled on all rf chips, but you can disable
// it by commenting (add //) the lines below if you don't want to risk flying a
// model with low power.
#define A7105_ENABLE_LOW_POWER

/*****************/
/*** GLOBAL ID ***/
/*****************/
// A global ID is used by most protocols to bind and retain the bind to models.
// To prevent duplicate IDs, it is automatically
//  generated using a random 32 bits number the first time the eeprom is
//  initialized.
// If you have 2 Multi modules which you want to share the same ID so you can
// use either to control the same RC model
//  then you can force the ID to a certain known value using the lines below.
// Default is commented, you should uncoment only for test purpose or if you
// know exactly what you are doing!!! The 8 numbers below can be anything
// between 0...9 and A..F #define FORCE_GLOBAL_ID	0x12345678

/****************************/
/*** PROTOCOLS TO INCLUDE ***/
/****************************/
// In this section select the protocols you want to be accessible when using the
// module. All the protocols will not fit in the STM32 or Atmega328p modules so
// you need to pick and choose. Comment the protocols you are not using with
// "//" to save Flash space.

// TODO check if these are indeed needed, some are just in checks, and not used
// in code Protocol for module configuration
#define MULTI_CONFIG_INO
#define ARDUINO_AVR_NANO
#define MIN_PPM_CHANNELS 2
#define MAX_PPM_CHANNELS 4
#define PPM_MAX_100 100
#define PPM_MIN_100 0
// Channel ordering
#define AILERON 0
#define ELEVATOR 2
#define THROTTLE 1
#define RUDDER 3

// The protocols below need an A7105 to be installed
#define KYOSHO_A7105_INO

/*
       PROTO_KYOSHO
               KYOSHO_FHSS
*/