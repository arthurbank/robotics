#include <SoftwareSerial.h>

// Sensor TX → Arduino pin 6
// Sensor RX → Arduino pin 7
SoftwareSerial co2(6, 7); // RX, TX

byte requestCO2[9] = {0xFF, 0x01, 0x86, 0, 0, 0, 0, 0, 0x79};

void setup() {
  Serial.begin(9600);
  co2.begin(9600);
  delay(1000);
  Serial.println("MH-Z19C starting...");
}

void loop() {
  co2.write(requestCO2, 9);
  delay(50);

  if (co2.available() >= 9) {
    byte response[9];
    co2.readBytes(response, 9);

    int ppm = (256 * response[2]) + response[3];

    Serial.print("CO2: ");
    Serial.print(ppm);
    Serial.println(" ppm");
  } else {
    Serial.println("No response...");
  }

  delay(2000);
}
