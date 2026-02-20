#include <avr/pgmspace.h>

#include "Multiprotocol.h"
#include "_Config.h"

#include "Pins.h"
#include "TX_Def.h"
#include "Validate.h"

#include <avr/eeprom.h>
#include <Arduino.h>
#include <avr/io.h>
#include "a7105.hpp"
#include "fhss.hpp"

// Global constants/variables
uint32_t MProtocol_id; // tx id,
uint32_t MProtocol_id_master;
uint32_t blink = 0, last_signal = 0;
//
uint16_t counter;
uint8_t channel;
uint8_t packet[50];

#define NUM_CHN 16
// Servo data
uint16_t Channel_data[NUM_CHN];
uint8_t Channel_AUX;

// Protocol variables
uint8_t cyrfmfg_id[6]; // for dsm2 and devo
uint8_t rx_tx_addr[5];
uint8_t rx_id[5];
uint8_t phase;
uint16_t bind_counter;
uint8_t bind_phase;
uint8_t binding_idx;
uint16_t packet_period;
uint8_t packet_count;
uint8_t packet_sent;
uint8_t packet_length;
uint8_t hopping_frequency[50];
uint8_t *hopping_frequency_ptr;
uint8_t hopping_frequency_no = 0;
uint8_t rf_ch_num;
uint8_t throttle, rudder, elevator, aileron;
uint8_t flags;
uint16_t crc;
uint16_t crc16_polynomial;
uint8_t crc8;
uint8_t crc8_polynomial;
uint16_t seed;
uint16_t failsafe_count;
uint16_t state;
uint8_t len;
uint8_t armed, arm_flags, arm_channel_previous;
uint8_t num_ch;
uint32_t pps_timer;
uint16_t pps_counter;

// Channel mapping for protocols
uint8_t CH_AETR[] = {AILERON, ELEVATOR, THROTTLE, RUDDER, CH5,  CH6,
                     CH7,     CH8,      CH9,      CH10,   CH11, CH12,
                     CH13,    CH14,     CH15,     CH16};
uint8_t CH_TAER[] = {THROTTLE, AILERON, ELEVATOR, RUDDER, CH5,  CH6,
                     CH7,      CH8,     CH9,      CH10,   CH11, CH12,
                     CH13,     CH14,    CH15,     CH16};
// uint8_t CH_RETA[]={RUDDER, ELEVATOR, THROTTLE, AILERON, CH5, CH6, CH7, CH8,
// CH9, CH10, CH11, CH12, CH13, CH14, CH15, CH16};
uint8_t CH_EATR[] = {ELEVATOR, AILERON, THROTTLE, RUDDER, CH5,  CH6,
                     CH7,      CH8,     CH9,      CH10,   CH11, CH12,
                     CH13,     CH14,    CH15,     CH16};

// Mode_select variables
uint8_t mode_select;
uint8_t protocol_flags = 0, protocol_flags2 = 0, protocol_flags3 = 0;
uint8_t option_override;

// Serial protocol
const uint8_t sub_protocol = KYOSHO_FHSS;
const uint8_t protocol = PROTO_KYOSHO;
uint8_t option;
uint8_t cur_protocol[3];
uint8_t prev_option;
uint8_t prev_power = 0xFD; // unused power value
uint8_t RX_num;

// Serial RX variables
#define BAUD 100000
#define RXBUFFER_SIZE 36 // 26+1+9
volatile uint8_t rx_buff[RXBUFFER_SIZE];
volatile uint8_t rx_ok_buff[RXBUFFER_SIZE];
volatile bool discard_frame = false;
volatile uint8_t rx_idx = 0, rx_len = 0;

// Callback
uint16_function_t remote_callback = 0;
uint8_t multi_protocols_index = 0xFF;

A7105 modem(2); // CSN on D2
FHSS trans(modem, 14); // Bind pin on D14

void setup() {
  // Setup diagnostic uart before anything else
  Serial.begin(115200);

  // ATMEGA328p
  // Set all ports to inputs
  DDRB = 0x00;
  DDRC = 0x00;
  DDRD = 0x00;
  // Set outputs

  // Timer1 config
  TCCR1A = 0;
  TCCR1B = (1 << CS11); // prescaler8, set timer1 to increment every
                        // 0.5us(16Mhz) and start timer

  bool success = trans.initialize();
  if (success){
    debugln("Init success");
  } else {
    debugln("Init fail");
    while(true){
      delay(100);
    }
  }
  delay(100);
}

uint16_t next_callback, diff;
uint8_t count = 0;
void loop() {
  TX_MAIN_PAUSE_on;
  // Timer ticks till next call back
  next_callback = trans.callback() << 1;
  TX_MAIN_PAUSE_off;
  cli(); // Prevent race condition in accessing multi-byte registers
  OCR1A += next_callback; // Calc when next_callback should happen
  TIFR1 = _BV(OCF1A);     // Clear compare A=callback flag
  diff = OCR1A - TCNT1;   // Calc the time difference
  sei();
  if ((diff & 0x8000) && !(next_callback & 0x8000)) {
    debugln("Short CB:%d", next_callback);
  } else {
    while ((TIFR1 & _BV(OCF1A)) == 0) {
      // Wait till compare timer triggers
    }
  }
}
