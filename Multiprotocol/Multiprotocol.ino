#include <avr/pgmspace.h>

#include "Multiprotocol.h"
#include "_Config.h"

#include "Pins.h"
#include "TX_Def.h"
#include "Validate.h"

#include <avr/eeprom.h>

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
uint8_t sub_protocol;
uint8_t protocol;
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

void setup() {
  // Setup diagnostic uart before anything else
  Serial.begin(115200);

  // ATMEGA328p
  // Set all ports to inputs
  DDRB = 0x00;
  DDRC = 0x00;
  DDRD = 0x00;
  // Set outputs
  SDI_output;
  SCLK_output;
#ifdef A7105_CSN_pin
  A7105_CSN_output;
#endif
  PE1_output;
  PE2_output;
  SERIAL_TX_output;

  BIND_port |= _BV(BIND_pin);

  // Timer1 config
  TCCR1A = 0;
  TCCR1B = (1 << CS11); // prescaler8, set timer1 to increment every
                        // 0.5us(16Mhz) and start timer

  protocol_init();
  // Random
  random_init();

// Set Chip selects
#ifdef A7105_CSN_pin
  A7105_CSN_on;
#endif
  SDI_on;
  SCLK_off;

  delayMilliseconds(100);

  // Read status of bind button
  if (IS_BIND_BUTTON_on) {
    BIND_BUTTON_FLAG_on; // If bind button pressed save the status
    BIND_IN_PROGRESS;    // Request bind
  } else
    BIND_DONE;

  // Set default channels' value
  for (uint8_t i = 0; i < NUM_CHN; i++)
    Channel_data[i] = 1024;
  Channel_data[THROTTLE] = 0; // 0=-125%, 204=-100%

  // Init RF modules
  modules_reset();

  // Read or create protocol id
  MProtocol_id_master = random_id(EEPROM_ID_OFFSET, false);

  debugln("Module Id: %lx", MProtocol_id_master);
  debugln("Init complete");
}

uint16_t next_callback, diff;
uint8_t count = 0;
void loop() {
  TX_MAIN_PAUSE_on;
  // Timer ticks till next call back
  next_callback = remote_callback() << 1;
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

void End_Bind() {
  // Request protocol to terminate bind
  if (protocol == PROTO_FRSKYD || protocol == PROTO_FRSKYL ||
      protocol == PROTO_FRSKYX || protocol == PROTO_FRSKYX2 ||
      protocol == PROTO_FRSKYV || protocol == PROTO_FRSKY_R9 ||
      protocol == PROTO_DSM_RX || protocol == PROTO_AFHDS2A_RX ||
      protocol == PROTO_FRSKY_RX || protocol == PROTO_BAYANG_RX ||
      protocol == PROTO_AFHDS2A || protocol == PROTO_BUGS ||
      protocol == PROTO_BUGSMINI || protocol == PROTO_HOTT ||
      protocol == PROTO_ASSAN)
    BIND_DONE;
  else if (bind_counter > 2)
    bind_counter = 2;
}

// Update channels direction and Channel_AUX flags based on servo AUX positions
static void update_channels_aux(void) {
  // Calc AUX flags
  Channel_AUX = 0;
  for (uint8_t i = 0; i < 8; i++)
    if (Channel_data[CH5 + i] > CHANNEL_SWITCH)
      Channel_AUX |= 1 << i;
}

void rf_switch(uint8_t comp) {
  PE1_off;
  PE2_off;
  switch (comp) {
  case SW_CC2500:
    PE2_on;
    break;
  case SW_CYRF:
    PE2_on;
  case SW_NRF:
    PE1_on;
    break;
  }
}

void modules_reset() {
#ifdef A7105_INSTALLED
  A7105_Reset();
#endif
  // Wait for every component to reset
  delayMilliseconds(100);
  prev_power = 0xFD; // unused power value
}

void Mprotocol_serial_init() {
// ATMEGA328p
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0A = 0; // Clear X2 bit
  // Set frame format to 8 data bits, even parity, 2 stop bits
  UCSR0C = _BV(UPM01) | _BV(USBS0) | _BV(UCSZ01) | _BV(UCSZ00);
  while (UCSR0A & (1 << RXC0)) // flush receive buffer
    UDR0;
  // enable reception and RC complete interrupt
  UCSR0B = _BV(RXEN0) | _BV(RXCIE0); // rx enable and interrupt
}

// Convert 32b id to rx_tx_addr
static void set_rx_tx_addr(uint32_t id) { // Used by almost all protocols
  rx_tx_addr[0] = (id >> 24) & 0xFF;
  rx_tx_addr[1] = (id >> 16) & 0xFF;
  rx_tx_addr[2] = (id >> 8) & 0xFF;
  rx_tx_addr[3] = (id >> 0) & 0xFF;
  rx_tx_addr[4] = (rx_tx_addr[2] & 0xF0) | (rx_tx_addr[3] & 0x0F);
}

static uint32_t random_id(uint16_t address, uint8_t create_new) {
  uint32_t id = 0;

  if (eeprom_read_byte((EE_ADDR)(address + 10)) == 0xf0 &&
      !create_new) { // TXID exists in EEPROM
    for (uint8_t i = 4; i > 0; i--) {
      id <<= 8;
      id |= eeprom_read_byte((EE_ADDR)address + i - 1);
    }
    if (id != 0x2AD141A7) // ID with seed=0
    {
      // debugln("Read ID from EEPROM");
      return id;
    }
  }
  // Generate a random ID
  (void)address;
  (void)create_new;
  return 0x12345678; // Forced global ID, arbitrary
}

// Generate frequency hopping sequence in the range [02..77]
static void __attribute__((unused)) calc_fh_channels(uint8_t num_ch) {
  uint8_t idx = 0;
  uint32_t rnd = MProtocol_id;
  uint8_t max = (num_ch / 3) + 2;

  while (idx < num_ch) {
    uint8_t i;
    uint8_t count_2_26 = 0, count_27_50 = 0, count_51_74 = 0;

    rnd = rnd * 0x0019660D + 0x3C6EF35F; // Randomization
    // Use least-significant byte. 73 is prime, so channels 76..77 are unused
    uint8_t next_ch = ((rnd >> 8) % 73) + 2;
    // Keep a distance of 5 between consecutive channels
    if (idx != 0) {
      if (hopping_frequency[idx - 1] > next_ch) {
        if (hopping_frequency[idx - 1] - next_ch < 5)
          continue;
      } else if (next_ch - hopping_frequency[idx - 1] < 5)
        continue;
    }
    // Check that it's not duplicated and spread uniformly
    for (i = 0; i < idx; i++) {
      if (hopping_frequency[i] == next_ch)
        break;
      if (hopping_frequency[i] <= 26)
        count_2_26++;
      else if (hopping_frequency[i] <= 50)
        count_27_50++;
      else
        count_51_74++;
    }
    if (i != idx)
      continue;
    if ((next_ch <= 26 && count_2_26 < max) ||
        (next_ch >= 27 && next_ch <= 50 && count_27_50 < max) ||
        (next_ch >= 51 && count_51_74 < max))
      hopping_frequency[idx++] = next_ch; // find hopping frequency
  }
}

static uint8_t __attribute__((unused)) bit_reverse(uint8_t b_in) {
  uint8_t b_out = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    b_out = (b_out << 1) | (b_in & 1);
    b_in >>= 1;
  }
  return b_out;
}

static void __attribute__((unused)) crc16_update(uint8_t a, uint8_t bits) {
  crc ^= a << 8;
  while (bits--)
    if (crc & 0x8000)
      crc = (crc << 1) ^ crc16_polynomial;
    else
      crc = crc << 1;
}

static void __attribute__((unused)) crc8_update(uint8_t byte) {
  crc8 = crc8 ^ byte;
  for (uint8_t j = 0; j < 8; j++)
    if (crc8 & 0x80)
      crc8 = (crc8 << 1) ^ crc8_polynomial;
    else
      crc8 <<= 1;
}

// Protocol start
static void protocol_init() {
  if (IS_WAIT_BIND_off) {
    remote_callback = 0;       // No protocol
    crc16_polynomial = 0x1021; // Default CRC crc16_polynomial
    crc8_polynomial = 0x31;    // Default CRC crc8_polynomial
    prev_option = option;

    multi_protocols_index = 0xFF;
    binding_idx = 0;

    // Set global ID and rx_tx_addr
    MProtocol_id = RX_num + MProtocol_id_master;
    set_rx_tx_addr(MProtocol_id);

    DATA_BUFFER_LOW_off;

    SUB_PROTO_INVALID;
    option_override = 0xFF;

    blink = millis();

    debugln("Protocol selected: %d, sub proto %d, rxnum %d, option %d",
            protocol, sub_protocol, RX_num, option);
    if (protocol) {
      // Reset all modules
      modules_reset();

      uint8_t index = 0;
      // #if defined(FRSKYX_CC2500_INO) && defined(MULTI_EU)
      //	if( ! ( (protocol == PROTO_FRSKYX || protocol == PROTO_FRSKYX2)
      //&& sub_protocol < 2 ) ) #endif
      while (multi_protocols[index].protocol != 0xFF) {
        if (multi_protocols[index].protocol == protocol) {
          // Save index
          multi_protocols_index = index;
          // Check sub protocol validity
          if (((sub_protocol & 0x07) == 0) ||
              (sub_protocol & 0x07) < multi_protocols[index].nbrSubProto)
            SUB_PROTO_VALID;
          if (IS_SUB_PROTO_VALID) { // Start the protocol
            // Set the RF switch
            rf_switch(multi_protocols[index].rfSwitch);
            // Init protocol
            multi_protocols[index].Init(); // Init could invalidate the sub
                                           // proto in case it is not suuported
            if (IS_SUB_PROTO_VALID)
              remote_callback =
                  multi_protocols[index]
                      .CallBack; // Save call back function address
          }
          break;
        }
        index++;
      }
    }
  }

#if defined(WAIT_FOR_BIND) && defined(ENABLE_BIND_CH)
  if (IS_AUTOBIND_FLAG_on && IS_BIND_CH_PREV_off &&
      (cur_protocol[1] & 0x80) == 0 &&
      mode_select ==
          MODE_SERIAL) { // Autobind is active but no bind requested by either
                         // BIND_CH or BIND. But do not wait if in PPM mode...
    WAIT_BIND_on;
    return;
  }
#endif
  WAIT_BIND_off;
  CHANGE_PROTOCOL_FLAG_off;

  if (protocol) {
    // Wait 5ms after protocol init
    cli();                    // disable global int
    OCR1A = TCNT1 + 5000 * 2; // set compare A for callback
#ifndef STM32_BOARD
    TIFR1 = OCF1A_bm; // clear compare A flag
#else
    TIMER2_BASE->SR =
        0x1E5F & ~TIMER_SR_CC1IF; // Clear Timer2/Comp1 interrupt flag
#endif
    sei();                // enable global int
    BIND_BUTTON_FLAG_off; // do not bind/reset id anymore even if protocol
                          // change
  }
}

static void random_init(void) {
  cli();     // Temporarily turn off interrupts, until WDT configured
  MCUSR = 0; // Use the MCU status register to reset flags for WDR, BOR, EXTR,
             // and POWR
  WDTCSR |=
      _BV(WDCE); // WDT control register, This sets the Watchdog Change Enable
                 // (WDCE) flag, which is  needed to set the prescaler
  WDTCSR = _BV(WDIE); // Watchdog interrupt enable (WDIE)
  sei();              // Turn interupts on
}
