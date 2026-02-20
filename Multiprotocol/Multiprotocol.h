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

//******************
// Version
//******************
#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>
#define VERSION_MAJOR 1
#define VERSION_MINOR 3
#define VERSION_REVISION 4
#define VERSION_PATCH_LEVEL 52

#define MODE_SERIAL 0

//******************
// Protocols
//******************
enum PROTOCOLS {
  PROTO_KYOSHO = 73,      // =>A7105
};

// subprotocols
enum KYOSHO {
  KYOSHO_FHSS = 0,
  KYOSHO_HYPE = 1,
};

#define NONE 0
#define P_HIGH 1
#define P_LOW 0
#define AUTOBIND 1
#define NO_AUTOBIND 0

// PPM protocols
struct PPM_Parameters {
  uint8_t protocol;
  uint8_t sub_proto : 3;
  uint8_t rx_num : 6;
  uint8_t power : 1;
  uint8_t autobind : 1;
  int8_t option;
  uint32_t chan_order;
};

// Callback
typedef uint16_t (*uint16_function_t)(
    void); // pointer to a function with no parameters which return an uint16_t
           // integer
typedef void (*void_function_t)(
    void); // pointer to a function with no parameters which returns nothing

// Protocols definition
struct __attribute__((__packed__)) mm_protocol_definition {
  uint8_t protocol;
  const char *ProtoString;
  const char *SubProtoString;
  uint8_t nbrSubProto : 4;
  uint8_t optionType : 4;
  uint8_t failSafe : 1;
  uint8_t chMap : 1;
  uint8_t rfSwitch : 2;
  void_function_t Init;
  uint16_function_t CallBack;
};
extern const mm_protocol_definition multi_protocols[];

// Telemetry
enum MultiPacketTypes {
  MULTI_TELEMETRY_STATUS = 1,
  MULTI_TELEMETRY_SPORT = 2,
  MULTI_TELEMETRY_HUB = 3,
  MULTI_TELEMETRY_DSM = 4,
  MULTI_TELEMETRY_DSMBIND = 5,
  MULTI_TELEMETRY_AFHDS2A = 6,
  MULTI_TELEMETRY_REUSE_1 = 7,
  MULTI_TELEMETRY_SYNC = 8,
  MULTI_TELEMETRY_REUSE_2 = 9,
  MULTI_TELEMETRY_HITEC = 10,
  MULTI_TELEMETRY_SCANNER = 11,
  MULTI_TELEMETRY_AFHDS2A_AC = 12,
  MULTI_TELEMETRY_RX_CHANNELS = 13,
  MULTI_TELEMETRY_HOTT = 14,
  MULTI_TELEMETRY_MLINK = 15,
  MULTI_TELEMETRY_CONFIG = 16,
  MULTI_TELEMETRY_PROTO = 17,
};

// Macros
#define NOP() __asm__ __volatile__("nop")

//***************
//***  Flags  ***
//***************
#define RX_FLAG_on protocol_flags |= _BV(0)
#define RX_FLAG_off protocol_flags &= ~_BV(0)
#define IS_RX_FLAG_on ((protocol_flags & _BV(0)) != 0)
//
#define CHANGE_PROTOCOL_FLAG_on protocol_flags |= _BV(1)
#define CHANGE_PROTOCOL_FLAG_off protocol_flags &= ~_BV(1)
#define IS_CHANGE_PROTOCOL_FLAG_on ((protocol_flags & _BV(1)) != 0)
//
#define POWER_FLAG_on protocol_flags |= _BV(2)
#define POWER_FLAG_off protocol_flags &= ~_BV(2)
#define IS_POWER_FLAG_on ((protocol_flags & _BV(2)) != 0)
//
#define RANGE_FLAG_on protocol_flags |= _BV(3)
#define RANGE_FLAG_off protocol_flags &= ~_BV(3)
#define IS_RANGE_FLAG_on ((protocol_flags & _BV(3)) != 0)
//
#define AUTOBIND_FLAG_on protocol_flags |= _BV(4)
#define AUTOBIND_FLAG_off protocol_flags &= ~_BV(4)
#define IS_AUTOBIND_FLAG_on ((protocol_flags & _BV(4)) != 0)
//
#define BIND_BUTTON_FLAG_on protocol_flags |= _BV(5)
#define BIND_BUTTON_FLAG_off protocol_flags &= ~_BV(5)
#define IS_BIND_BUTTON_FLAG_on ((protocol_flags & _BV(5)) != 0)
// PPM RX OK
#define PPM_FLAG_off protocol_flags &= ~_BV(6)
#define PPM_FLAG_on protocol_flags |= _BV(6)
#define IS_PPM_FLAG_on ((protocol_flags & _BV(6)) != 0)
// Bind flag
#define BIND_IN_PROGRESS protocol_flags &= ~_BV(7)
#define BIND_DONE protocol_flags |= _BV(7)
#define IS_BIND_DONE ((protocol_flags & _BV(7)) != 0)
#define IS_BIND_IN_PROGRESS ((protocol_flags & _BV(7)) == 0)
//
#define FAILSAFE_VALUES_off protocol_flags2 &= ~_BV(0)
#define FAILSAFE_VALUES_on protocol_flags2 |= _BV(0)
#define IS_FAILSAFE_VALUES_on ((protocol_flags2 & _BV(0)) != 0)
//
#define RX_DONOTUPDATE_off protocol_flags2 &= ~_BV(1)
#define RX_DONOTUPDATE_on protocol_flags2 |= _BV(1)
#define IS_RX_DONOTUPDATE_on ((protocol_flags2 & _BV(1)) != 0)
//
#define RX_MISSED_BUFF_off protocol_flags2 &= ~_BV(2)
#define RX_MISSED_BUFF_on protocol_flags2 |= _BV(2)
#define IS_RX_MISSED_BUFF_on ((protocol_flags2 & _BV(2)) != 0)
// TX Pause
#define TX_MAIN_PAUSE_off protocol_flags2 &= ~_BV(3)
#define TX_MAIN_PAUSE_on protocol_flags2 |= _BV(3)
#define IS_TX_MAIN_PAUSE_on ((protocol_flags2 & _BV(3)) != 0)
#define TX_RX_PAUSE_off protocol_flags2 &= ~_BV(4)
#define TX_RX_PAUSE_on protocol_flags2 |= _BV(4)
#define IS_TX_RX_PAUSE_on ((protocol_flags2 & _BV(4)) != 0)
#define IS_TX_PAUSE_on ((protocol_flags2 & (_BV(4) | _BV(3))) != 0)
#define IS_TX_PAUSE_off ((protocol_flags2 & (_BV(4) | _BV(3))) == 0)
// Signal OK
#define INPUT_SIGNAL_off protocol_flags2 &= ~_BV(5)
#define INPUT_SIGNAL_on protocol_flags2 |= _BV(5)
#define IS_INPUT_SIGNAL_on ((protocol_flags2 & _BV(5)) != 0)
#define IS_INPUT_SIGNAL_off ((protocol_flags2 & _BV(5)) == 0)
// Bind from channel
#define BIND_CH_PREV_off protocol_flags2 &= ~_BV(6)
#define BIND_CH_PREV_on protocol_flags2 |= _BV(6)
#define IS_BIND_CH_PREV_on ((protocol_flags2 & _BV(6)) != 0)
#define IS_BIND_CH_PREV_off ((protocol_flags2 & _BV(6)) == 0)
// Wait for bind
#define WAIT_BIND_off protocol_flags2 &= ~_BV(7)
#define WAIT_BIND_on protocol_flags2 |= _BV(7)
#define IS_WAIT_BIND_on ((protocol_flags2 & _BV(7)) != 0)
#define IS_WAIT_BIND_off ((protocol_flags2 & _BV(7)) == 0)
// Incoming telemetry data buffer
#define DATA_BUFFER_LOW_off protocol_flags3 &= ~_BV(0)
#define DATA_BUFFER_LOW_on protocol_flags3 |= _BV(0)
#define IS_DATA_BUFFER_LOW_on ((protocol_flags3 & _BV(0)) != 0)
#define IS_DATA_BUFFER_LOW_off ((protocol_flags3 & _BV(0)) == 0)
#define SEND_MULTI_STATUS_off protocol_flags3 &= ~_BV(1)
#define SEND_MULTI_STATUS_on protocol_flags3 |= _BV(1)
#define IS_SEND_MULTI_STATUS_on ((protocol_flags3 & _BV(1)) != 0)
#define IS_SEND_MULTI_STATUS_off ((protocol_flags3 & _BV(1)) == 0)
#define DISABLE_CH_MAP_off protocol_flags3 &= ~_BV(2)
#define DISABLE_CH_MAP_on protocol_flags3 |= _BV(2)
#define IS_DISABLE_CH_MAP_on ((protocol_flags3 & _BV(2)) != 0)
#define IS_DISABLE_CH_MAP_off ((protocol_flags3 & _BV(2)) == 0)
#define DISABLE_TELEM_off protocol_flags3 &= ~_BV(3)
#define DISABLE_TELEM_on protocol_flags3 |= _BV(3)
#define IS_DISABLE_TELEM_on ((protocol_flags3 & _BV(3)) != 0)
#define IS_DISABLE_TELEM_off ((protocol_flags3 & _BV(3)) == 0)
// Valid/invalid sub_proto
#define SUB_PROTO_VALID protocol_flags3 &= ~_BV(6)
#define SUB_PROTO_INVALID protocol_flags3 |= _BV(6)
#define IS_SUB_PROTO_INVALID ((protocol_flags3 & _BV(6)) != 0)
#define IS_SUB_PROTO_VALID ((protocol_flags3 & _BV(6)) == 0)
// LBT power
#define LBT_POWER_off protocol_flags3 &= ~_BV(7)
#define LBT_POWER_on protocol_flags3 |= _BV(7)
#define IS_LBT_POWER_on ((protocol_flags3 & _BV(7)) != 0)
#define IS_LBT_POWER_off ((protocol_flags3 & _BV(7)) == 0)

// Failsafe
#define FAILSAFE_CHANNEL_HOLD 2047
#define FAILSAFE_CHANNEL_NOPULSES 0

//** Debug messages **

inline void debug(const char* format, ...) {
    char buffer[128];
    
    // Initialize the variadic argument list
    va_list args;
    va_start(args, format);
    
    // vsnprintf safely limits the output to the size of our buffer,
    // guaranteeing we never overwrite adjacent memory.
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    // Clean up the argument list
    va_end(args);
    
    Serial.print(buffer);
}

inline void debugln(const char* format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Serial.print(buffer);
    Serial.println(); // Let Arduino's built-in library handle the \r\n cleanly
}

//********************
//*** Blink timing ***
//********************
#define BLINK_BIND_TIME 100
#define BLINK_SERIAL_TIME 500
#define BLINK_PPM_TIME 1000
#define BLINK_BAD_PROTO_TIME_HIGH 50
#define BLINK_BAD_PROTO_TIME_LOW 1000
#define BLINK_WAIT_BIND_TIME_HIGH 1000
#define BLINK_WAIT_BIND_TIME_LOW 100
#define BLINK_BANK_TIME_HIGH 50
#define BLINK_BANK_TIME_LOW 500
#define BLINK_BANK_REPEAT 1500

//*******************
//***  AUX flags  ***
//*******************
#define GET_FLAG(ch, mask) (ch ? mask : 0)
#define CH5_SW (Channel_AUX & _BV(0))
#define CH6_SW (Channel_AUX & _BV(1))
#define CH7_SW (Channel_AUX & _BV(2))
#define CH8_SW (Channel_AUX & _BV(3))
#define CH9_SW (Channel_AUX & _BV(4))
#define CH10_SW (Channel_AUX & _BV(5))
#define CH11_SW (Channel_AUX & _BV(6))
#define CH12_SW (Channel_AUX & _BV(7))
#define CH13_SW (Channel_data[CH13] > CHANNEL_SWITCH)
#define CH14_SW (Channel_data[CH14] > CHANNEL_SWITCH)
#define CH15_SW (Channel_data[CH15] > CHANNEL_SWITCH)
#define CH16_SW (Channel_data[CH16] > CHANNEL_SWITCH)

//************************
//***  Power settings  ***
//************************
enum {
  TXPOWER_100uW,
  TXPOWER_300uW,
  TXPOWER_1mW,
  TXPOWER_3mW,
  TXPOWER_10mW,
  TXPOWER_30mW,
  TXPOWER_100mW,
  TXPOWER_150mW
};

// A7105 power
// The numbers do not take into account any outside amplifier
enum A7105_POWER {
  A7105_POWER_0 = 0x00 << 3 | 0x00, // -23dBm == PAC=0 TBG=0
  A7105_POWER_1 = 0x00 << 3 | 0x01, // -20dBm == PAC=0 TBG=1
  A7105_POWER_2 = 0x00 << 3 | 0x02, // -16dBm == PAC=0 TBG=2
  A7105_POWER_3 = 0x00 << 3 | 0x04, // -11dBm == PAC=0 TBG=4
  A7105_POWER_4 = 0x01 << 3 | 0x05, //  -6dBm == PAC=1 TBG=5
  A7105_POWER_5 = 0x02 << 3 | 0x07, //   0dBm == PAC=2 TBG=7
  A7105_POWER_6 = 0x03 << 3 | 0x07, //  +1dBm == PAC=3 TBG=7
  A7105_POWER_7 = 0x03 << 3 | 0x07  //  +1dBm == PAC=3 TBG=7
};
#define A7105_HIGH_POWER A7105_POWER_7
#define A7105_LOW_POWER A7105_POWER_3
#define A7105_RANGE_POWER A7105_POWER_0
#define A7105_BIND_POWER A7105_POWER_0

