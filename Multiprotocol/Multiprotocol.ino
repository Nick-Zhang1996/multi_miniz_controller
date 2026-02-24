#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <Arduino.h>
#include <avr/io.h>

#include "Multiprotocol.h"
#include "_Config.h"
#include "Validate.h"
#include "a7105.hpp"
#include "fhss.hpp"
// MOSI D11 -> SDIO
// MISO D12 -> unconnected
// SCK D13 -> SCK
// CS D2 -> modem 1
// CS D3 -> modem 2
// D14 A0-> BIND 1
// D15 A1-> BIND 2
// D16 A2-> BIND 3
// D17 A3-> BIND 4

// --- Constants & Types ---
constexpr uint8_t kStartByte1 = 0xAA;
constexpr uint8_t kStartByte2 = 0x55;
constexpr size_t kPayloadSize = 20; // 10 uint16_t = 20 bytes

enum class RxState {
  kWaitStart1,
  kWaitStart2,
  kReadPayload,
  kReadCrc
};

// Global or class-member array to hold the successfully validated data
uint16_t g_pwm_data[10] = {0}; 


SoftSPI soft_spi(11, 13);
A7105 modem1(2); // CSN on D2
FHSS trans1(modem1, 14,0x3D743B, g_pwm_data, g_pwm_data+1); // Bind pin on D14 (A0)
A7105 modem2(3); // CSN on D3
FHSS trans2(modem2, 15,0x4E5235, g_pwm_data+2, g_pwm_data+3); // Bind pin on D15 (A1)
A7105 modem3(4); // CSN on D4
FHSS trans3(modem3, 16,0x521234, g_pwm_data+4, g_pwm_data+5); // Bind pin on D16 (A2)

// Schedule callbacks, min val: number of trans + 1
constexpr uint8_t kTaskSize = 4;
// Index for pending task
uint8_t task_idx = 0;
uint8_t next_task_idx;
// Index for adding new task to end of task list
uint8_t new_task_idx = 3;
// Timer stamp (OCR1A) for pending callbacks, 1 tick = 0.5us
// Transmission = 3100us, SPI transaction = 160us
// Even tasks are actual transmission, odd tasks are dummy
// trans1 (real) --- trans1 (dummy) - trans2(real) --- trans1(real) - trans2(dummy)
uint16_t task_ts[kTaskSize] = {100, 100+400*2, 100+400*4};
// Callback trans 
FHSS* task_target[kTaskSize] = {&trans1, &trans2, &trans3};



// --- Helper: CRC-8 Calculation ---
uint8_t calculateCrc8(const uint8_t* data, size_t len) {
  uint8_t crc = 0x00;
  for (size_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; ++j) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

void processSerialData() {
  static RxState state = RxState::kWaitStart1;
  static uint8_t payload_buffer[kPayloadSize];
  static size_t buffer_index = 0;

  while (Serial.available() > 0) {
    uint8_t incoming_byte = Serial.read();

    switch (state) {
      case RxState::kWaitStart1:
        if (incoming_byte == kStartByte1) {
          state = RxState::kWaitStart2;
        }
        break;

      case RxState::kWaitStart2:
        if (incoming_byte == kStartByte2) {
          state = RxState::kReadPayload;
          buffer_index = 0;
        } else if (incoming_byte == kStartByte1) {
          // Edge case: Saw 0xAA 0xAA (overlapping start bytes)
          state = RxState::kWaitStart2; 
        } else {
          state = RxState::kWaitStart1;
        }
        break;

      case RxState::kReadPayload:
        payload_buffer[buffer_index++] = incoming_byte;
        if (buffer_index >= kPayloadSize) {
          state = RxState::kReadCrc;
        }
        break;

      case RxState::kReadCrc:
        // Calculate CRC over the received payload
        uint8_t expected_crc = calculateCrc8(payload_buffer, kPayloadSize);
        
        if (incoming_byte == expected_crc) {
          // Packet is 100% valid. Safely copy to the working array.
          // Because AVR is an 8-bit architecture, casting a byte array to uint16_t* // works directly since Python struct.pack uses Little-Endian natively.
          memcpy(g_pwm_data, payload_buffer, kPayloadSize);
          // debugln("Good");
          // Optional: Set a flag here to notify the rest of the system new data arrived
        } else {
          // Checksum failed, discard the packet
          // debugln("val: %u, CRC Err Got:0x%02X, not 0x%02x ", g_pwm_data[0], incoming_byte, expected_crc); 
          // debugln("Bad: %02X %02X %02X %02X %02X ", payload_buffer[0], payload_buffer[1], payload_buffer[2], payload_buffer[3], payload_buffer[4]);
        }
        
        // Reset state machine to wait for the next frame
        state = RxState::kWaitStart1;
        break;
    }
  }
}

void setup() {
  // Setup diagnostic uart before anything else
  Serial.begin(115200);

  // ATMEGA328p
  // Set outputs
  // Set hardware CS pin to output to avoid capacitive change taking over SPI bus
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH);
  // DEBUG pin
  pinMode(8, OUTPUT);
  digitalWrite(8,LOW);

  // Timer1 config
  TCCR1A = 0;
  TCCR1B = (1 << CS11); // prescaler8, set timer1 to increment every
                        // 0.5us(16Mhz) and start timer

  // SPI lib has a reference counter, 
  // if we call begin() twice and end() once, hardware SPI won't be turned off
  // This will block software SPI from accessing the pins
  SPI.begin(); 
  bool success1 = modem1.initialize();
  if (success1){
    debugln("A7105 1 Init success");
  } else {
    debugln("A7105 1 Init fail");
  }

  bool success2 = modem2.initialize();
  if (success2){
    debugln("A7105 2 Init success");
  } else {
    debugln("A7105 2 Init fail");
  }

  bool success3 = modem3.initialize();
  if (success3){
    debugln("A7105 3 Init success");
  } else {
    debugln("A7105 3 Init fail");
  }
  if (! (success1 && success2 && success3)){
    while (1){
      delay(100);
    }
  }

  trans1.initialize();
  trans2.initialize();
  trans3.initialize();

  // Flush serial RX buffer
  while (Serial.available() > 0) {
    Serial.read();
  }
  for (int i=0; i<10; i++){
    g_pwm_data[i] = 1500;
  }
  // First callback will take place at 100 ticks
  cli();
  TCNT1 = 0;
  OCR1A = task_ts[task_idx];
  sei();
  TIFR1 = _BV(OCF1A);     // Clear compare A=callback flag
}

void loop() {
  uint16_t current_time,dt;
  // Check time budget till next callback, determine if reading serial
  if (TCNT1 < OCR1A && OCR1A - TCNT1 > 2800*2 ){
    PORTB |= 1;
    processSerialData(); // <250
    PORTB &= ~1;
  }

  bool trans_finished = false;
  while ((TIFR1 & _BV(OCF1A)) == 0) {
    // Wait till compare timer triggers

    // Check transmission duration, 1400us
    /*
    if (TCNT1 < OCR1A && OCR1A - TCNT1 > 100*2 ){
      if ((!trans_finished) && modem3.checkTransmission()){
        trans_finished = true;
        PORTB |= 1;
        _NOP();
        _NOP();
        PORTB &= ~1;
      }
    }
    */
  }

  // Register next callback
  // us -> ticks, 2 tick = 1us
  task_ts[new_task_idx] = OCR1A + (task_target[task_idx]->kCallbackInterval << 1); 
  task_target[new_task_idx] = task_target[task_idx];
  new_task_idx = (new_task_idx + 1) % kTaskSize;
  next_task_idx = (task_idx + 1) % kTaskSize;
  cli(); // Prevent race condition in accessing multi-byte registers
  OCR1A = task_ts[next_task_idx];
  TIFR1 = _BV(OCF1A);     // Clear compare A=callback flag
  sei();

  task_target[task_idx]->callback(); // ~200us
  task_idx = next_task_idx;


}

