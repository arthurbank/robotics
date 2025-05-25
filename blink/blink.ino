/*
  Blink LED Example
  
  This sketch demonstrates how to blink an LED connected to a digital pin.
  The LED will turn on for one second, then off for one second, repeatedly.
  
  Hardware Required:
  - Arduino board
  - LED
  - 220-330 ohm resistor
  
  Circuit:
  - Connect the longer leg (anode) of the LED to digital pin 13 through a 220-330 ohm resistor
  - Connect the shorter leg (cathode) of the LED to GND (ground)
*/

// Pin 13 has an LED connected on most Arduino boards
const int ledPin = 13;

void setup() {
  // Initialize digital pin ledPin as an output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);   // Turn the LED on
  delay(1000);                  // Wait for one second
  digitalWrite(ledPin, LOW);    // Turn the LED off
  delay(1000);                  // Wait for one second
} 