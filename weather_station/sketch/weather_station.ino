#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DHT setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// DS18B20 setup
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// MQ-2 and Thermistor
#define MQ2_PIN 34
#define THERMISTOR_PIN 35

void setup() {
  Serial.begin(9600);
  dht.begin();
  ds18b20.begin();
}

void loop() {
  // Read DHT values
  float dhtTemp = dht.readTemperature();
  float dhtHumid = dht.readHumidity();

  // Read DS18B20
  ds18b20.requestTemperatures();
  float dsTemp = ds18b20.getTempCByIndex(0);

  // Read Thermistor (dummy calculation for now)
  int thermRaw = analogRead(THERMISTOR_PIN);
  float thermistorTemp = (thermRaw / 4095.0) * 100.0;  // dummy Celsius

  // Read MQ-2
  int mq2Raw = analogRead(MQ2_PIN);
  float mq2ppm = mq2Raw * (1000.0 / 4095.0);  // dummy ppm con*

  // Print to Serial Monitor (Human Readable)
  // Serial.println("=== Sensor Readings ===");
  // Serial.print("DHT Temp (°C): "); Serial.println(dhtTemp);
  // Serial.print("DHT Humidity (%): "); Serial.println(dhtHumid);
  // Serial.print("DS18B20 Temp (°C): "); Serial.println(dsTemp);
  // Serial.print("Thermistor Temp (°C): "); Serial.println(thermistorTemp);
  // Serial.print("MQ-2 Raw: "); Serial.println(mq2Raw);
  // Serial.print("MQ-2 PPM (approx): "); Serial.println(mq2ppm);
  // Serial.println();

  // Send CSV line to serial (Python app will read and log it)
  Serial.print(0); Serial.print(",");
  Serial.print(dhtTemp); Serial.print(",");
  Serial.print(dhtHumid); Serial.print(",");
  Serial.print(dsTemp); Serial.print(",");
  Serial.print(thermistorTemp); Serial.print(",");
  Serial.println(mq2ppm);  // ends the line

}