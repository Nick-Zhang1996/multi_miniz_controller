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
FHSS trans1(modem, 14,0x3D743B); // Bind pin on D14 (A0)
FHSS trans2(modem, 15,0x4E5235); // Bind pin on D15 (A1)

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

  bool success = modem.initialize();
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
  delay(1);
}

uint8_t count = 0;
uint16_t trans1_callback_ts = 0; // tick for next callback for trans 1
uint16_t trans2_callback_ts = 0;
void loop() {
  cli(); // Prevent race condition in accessing multi-byte registers
  // Calc when next_callback should happen, 0.5us/ tick
  OCR1A += trans1.kCallbackInterval << 1; 
  TIFR1 = _BV(OCF1A);     // Clear compare A=callback flag
  trans1_callback_ts = OCR1A;
  sei();
  trans1.callback();

  delayMicroseconds(50);
  trans2_callback_ts = OCR1A + trans1.kCallbackInterval << 1;
  trans2.callback();
  while ((TIFR1 & _BV(OCF1A)) == 0) {
    // Wait till compare timer triggers
  }
}
