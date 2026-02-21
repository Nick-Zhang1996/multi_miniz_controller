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
// PIN mapping - bit-banging software SPI - 1.2MHz
// SDIO - D5
// SCLK - D4
// CSN - D2
// BIND - D14

// Hardware SPi
// MOSI D11 -> SDIO
// MISO D12 -> unconnected
// SCK D13 -> SCK
// CS D2 -> modem 1
// CS D3 -> modem 2
// D14 A0-> BIND 1
// D15 A1-> BIND 2
// D16 A2-> BIND 3
// D17 A3-> BIND 4



A7105 modem1(2); // CSN on D2
FHSS trans1(modem1, 14,0x3D743B); // Bind pin on D14 (A0)
A7105 modem2(3); // CSN on D3
FHSS trans2(modem2, 15,0x4E5235); // Bind pin on D15 (A1)
A7105 modem3(4); // CSN on D4
FHSS trans3(modem3, 16,0x521234); // Bind pin on D16 (A2)

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

  bool success = modem1.initialize();
  success = modem2.initialize();
  success = modem3.initialize();
  debugln("Unknown A7105 init status due to circuit limitations, still usable");
  /*
  if (success){
    debugln("A7105 Init success");
  } else {
    debugln("A7105 Init fail");
    while(true){
      delay(100);
    }
  }
  */
  trans1.initialize();
  trans2.initialize();
  trans3.initialize();

  // First callback will take place at 100 ticks
  cli();
  TCNT1 = 0;
  OCR1A = task_ts[task_idx];
  sei();
  TIFR1 = _BV(OCF1A);     // Clear compare A=callback flag
}

void loop() {
  while ((TIFR1 & _BV(OCF1A)) == 0) {
    // Wait till compare timer triggers
  }

  // Register next callback
  // us -> ticks, 2 tick = 1us
  uint16_t current_time = TCNT1;
  task_ts[new_task_idx] = OCR1A + (task_target[task_idx]->kCallbackInterval << 1); 
  task_target[new_task_idx] = task_target[task_idx];
  new_task_idx = (new_task_idx + 1) % kTaskSize;
  next_task_idx = (task_idx + 1) % kTaskSize;
  cli(); // Prevent race condition in accessing multi-byte registers
  OCR1A = task_ts[next_task_idx];
  TIFR1 = _BV(OCF1A);     // Clear compare A=callback flag
  sei();

  task_target[task_idx]->callback();
  //debugln("us: %u Calling %d",current_time/2, task_target[task_idx]->bind_pin_);

  task_idx = next_task_idx;

}
