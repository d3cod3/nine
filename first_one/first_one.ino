/*

    Project : ·NINE· [dasaki && n3m3da]
    
    Concept : BY-SA 4.0
    Code    : MIT
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    (BY-SA) 2017
*/

// External Libraries
#include <LowPower.h> // https://github.com/rocketscream/Low-Power

// Custom Code
#include "morse.h"

bool DEBUG = true;

unsigned long arTime = millis();
unsigned long reset = millis();
unsigned int timeMultiplier = 1; // x1
unsigned int MORSE_UNIT_TIME = 250*timeMultiplier;
unsigned int wait = 150*timeMultiplier;
unsigned int waitLynch = 33*timeMultiplier;
unsigned int actualPin = 2;
bool lynchON = false;

int LDR = 0;
long voltage = 0;
unsigned int minVoltage = 3200;
unsigned int ldrMinLight = 60;
bool weAreTheNight = false; 
 
unsigned int binaryCounter = 0; // 9 bits, 0 - 512

String message = encode( "PWNED " );

int ledPins[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10 };
int ledPinsRAND[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10 };


/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// SETUP
/////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  for(unsigned int i=0;i<9;i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  shuffleLEDS();

  if(DEBUG){
    Serial.begin(9600);
  }
}

/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// LOOP
/////////////////////////////////////////////////////////

void loop() {
  // Avoid arduino calls between loops
  while (true) {
    
    // Read voltage, check charge state
    voltage = readVcc();
    // read LDR sensor, check darkness
    readLDR();

    if(voltage < minVoltage || !weAreTheNight){
      // Charging or before dusk
      if(DEBUG){
        Serial.println("Low battery/It' not dark yet!");
      }
      // Enter power down state for 8 s with ADC and BOD module disabled
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }else if(voltage >= minVoltage && weAreTheNight){
      // We're ON
      arTime = millis();
  
      //checkLEDS();
      //sendMorseMessage();
      //binaryLED();
      binaryLEDRand();
      //lynchLED();
    }else{
      // SLEEP
      for(unsigned int i=0;i<9;i++){
        digitalWrite(ledPins[i], LOW);
      }
    }
    
  } // End while
}

/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// CUSTOM FUNCTIONS
/////////////////////////////////////////////////////////

/*
 * Leds check test, sequencially turning on
 * 
 */
void checkLEDS(){
  for(unsigned int i=0;i<9;i++){
    digitalWrite(ledPins[i], LOW);
    if(ledPins[i] == actualPin){
      digitalWrite(ledPins[i], HIGH);
    }
  }

  if(arTime-reset > wait){
    reset = millis();
    if(actualPin < 10){
      actualPin++;
    }else{
      actualPin = 2;
    }
  }
}

/*
 * Simulate 9 bits binary counter (correct positions)
 * 
 */
void binaryLED(){

  for(unsigned int i=0;i<9;i++){
    if(bitRead(binaryCounter,i) == 0){
      digitalWrite(ledPins[i], LOW);
    }else if(bitRead(binaryCounter,i) == 1){
      digitalWrite(ledPins[i], HIGH);
    }
  }

  if(arTime-reset > wait){
    reset = millis();
    if(binaryCounter < 512){
      binaryCounter++;
    }else{
      binaryCounter = 0;
    }
  }
}

/*
 * Simulate 9 bits binary counter (randomized positions)
 * 
 */
void binaryLEDRand(){

  for(unsigned int i=0;i<9;i++){
    if(bitRead(binaryCounter,i) == 0){
      digitalWrite(ledPinsRAND[i], LOW);
    }else if(bitRead(binaryCounter,i) == 1){
      digitalWrite(ledPinsRAND[i], HIGH);
    }
  }

  if(arTime-reset > wait){
    reset = millis();
    if(binaryCounter < 512){
      binaryCounter++;
    }else{
      binaryCounter = 0;
    }
  }
}

/*
 * A tribute to David Lynch
 * 
 */
void lynchLED(){
   for(unsigned int i=0;i<9;i++){
    if(lynchON){
      digitalWrite(ledPins[i], HIGH);
    }else{
      digitalWrite(ledPins[i], LOW);
    }
  }

  if(arTime-reset > waitLynch){
    reset = millis();
    lynchON = !lynchON;
  }
}

/*
 * As the function name says...
 * 
 */
void sendMorseMessage(){
  for(unsigned int i=0; i<=message.length(); i++){
    switch( message[i] ){
      case '.': // dit
        for(unsigned int i=0;i<9;i++){
          digitalWrite(ledPins[i], HIGH);
        }
        delay( MORSE_UNIT_TIME );
        for(unsigned int i=0;i<9;i++){
          digitalWrite(ledPins[i], LOW);
        }
        delay( MORSE_UNIT_TIME );
          
        break;

      case '-': // dah
        for(unsigned int i=0;i<9;i++){
          digitalWrite(ledPins[i], HIGH);
        }
        delay( MORSE_UNIT_TIME*3 );
        for(unsigned int i=0;i<9;i++){
          digitalWrite(ledPins[i], LOW);
        }
        delay( MORSE_UNIT_TIME );
          
        break;

      case ' ': //gap
        delay( MORSE_UNIT_TIME );
    }
  }
}

/*
 * Read LDR value (light/darkness detection)
 * 
 */
void readLDR(){
  LDR = analogRead(A0);

  if(LDR < ldrMinLight){
    weAreTheNight = true;
  }else{
    weAreTheNight = false;
  }
  
  if(DEBUG){
    Serial.println(LDR);
    delay(1);
  }
}

/*
 * Voltage reading
 * 
 */
long readVcc() { 
  long result; // Read 1.1V reference against AVcc 
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
  delay(2); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Convert 
  while (bit_is_set(ADCSRA,ADSC)); 
  result = ADCL; 
  result |= ADCH<<8; result = 1126400L / result; // Back-calculate AVcc in mV 
  return result; 
}

/*
 * Randomize LED pins
 * 
 */
void shuffleLEDS(){
  for (int a=0; a<9; a++){
    int r = random(a,8);
    int temp = ledPinsRAND[a];
    ledPinsRAND[a] = ledPinsRAND[r];
    ledPinsRAND[r] = temp;
  }
}

