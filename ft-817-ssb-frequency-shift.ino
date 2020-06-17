/*
	 This file is part of FT-817 SSB Frequency Shift.

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this project.  If not, see http://www.gnu.org/licenses/;.	  

    Copyright 2020 Christian Obersteiner, DL1COM
*/

#include <SoftwareSerial.h>
#include "FT817.h"
#include "buttonhw.h"

/*==================================*/
// Configurables
const unsigned long FREQ_SHIFT = 350000;  // x10 Hz
const unsigned long INIT_FREQ = 43350000; // x10 Hz

#define PTT_PIN A5
#define LED_PIN 13
/*===================================*/
#define DEBUG_PRINTLN(x) Serial.println(x)
#define INIT_WAIT_TIME 1000
#define RIG_READ_PERIOD 100

#define FT817_SPEED 4800
#define FT817_MODE_LEN 5
/*===================================*/

SoftwareSerial Serial_Cat(2, 3); // RX, TX
FT817 ft817(&Serial_Cat);
ButtonHW ptt_button(PTT_PIN);


typedef struct
{
  // current status
  long freq;
  char mode[FT817_MODE_LEN];
} 
t_status;
t_status rig; 

void initialize_ft817 ()
{
  ft817.begin(FT817_SPEED);
  delay(INIT_WAIT_TIME);
  read_rig();  
}

void read_rig ()
{
  do // rig frequency may initially be 0
  {
    rig.freq = ft817.getFreqMode(rig.mode);
  } 
  while (rig.freq == 0);

  static unsigned long old_freq = 0;
  if (rig.freq != old_freq) {
    Serial.println(String(rig.freq));
    old_freq = rig.freq;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}

void setFreq(long f) {
  do // it may happen, that the frequency is not set correctly during the 1st attempt.
  {
    ft817.setFreq(f);
    read_rig();
  } 
  while (rig.freq != f);
}

void setup ()
{
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  DEBUG_PRINTLN("CAT-SSB-SHIFT");
  DEBUG_PRINTLN("Shift: " + String(FREQ_SHIFT));

  Serial_Cat.begin(FT817_SPEED);

  initialize_ft817();
  
  setFreq(INIT_FREQ);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}
  
void loop ()
{
  // Regularly reading current frequency from trx
  static unsigned long last_time = 0;
  unsigned long current_time = millis();
  if (last_time + RIG_READ_PERIOD < current_time) {
    read_rig();
    last_time = current_time;
  }

  // On PTT button action
  ptt_button.update();
  static bool button_pressed = false;
  if (ptt_button.isPressedEdge()) {
    setFreq(rig.freq - long(FREQ_SHIFT));
    ft817.setPTTOn();
    button_pressed = true;
    DEBUG_PRINTLN("PTT ON");
  }
  if (button_pressed && ptt_button.isReleasedEdge()) {    
    ft817.setPTTOff();
    setFreq(rig.freq + long(FREQ_SHIFT));
    button_pressed = false;
    DEBUG_PRINTLN("PTT OFF");
  }
}
