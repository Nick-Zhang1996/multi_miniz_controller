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
// MOSI D11 -> output 1k ohm -> SDIO
// MISO D12 -> SDIO
// SCK D13 -> SCK
// CS D2



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
  delay(1);
}

uint16_t diff;
uint8_t count = 0;
void loop() {
  cli(); // Prevent race condition in accessing multi-byte registers
  OCR1A += trans.kCallbackInterval << 1; // Calc when next_callback should happen, 0.5us/ tick
  TIFR1 = _BV(OCF1A);     // Clear compare A=callback flag
  diff = OCR1A - TCNT1;   // Calc the time difference
  sei();
  // Timer ticks till next call back
  trans.callback();
  while ((TIFR1 & _BV(OCF1A)) == 0) {
    // Wait till compare timer triggers
  }
}
