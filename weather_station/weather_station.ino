#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define DS18B20_PIN 3
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);

#define THERMISTOR_PIN A0
#define MQ2_PIN A1

void setup() {
  Serial.begin(9600);
  dht.begin();
  ds18b20.begin();
  Serial.println("Weather Station Starting...");
}

void loop() {
  // Read DHT11
  float dhtTemp = dht.readTemperature();
  float dhtHumidity = dht.readHumidity();

  // Read DS18B20
  ds18b20.requestTemperatures();
  float dsTemp = ds18b20.getTempCByIndex(0);

  // Read Thermistor (analog raw value)
  int thermistorValue = analogRead(THERMISTOR_PIN);

  // Read MQ-2 Gas Sensor (analog raw value)
  int mq2Value = analogRead(MQ2_PIN);

  // Print all values
  Serial.println("------ Weather Station Data ------");
  if (isnan(dhtTemp) || isnan(dhtHumidity)) {
    Serial.println("DHT11 read failed!");
  } else {
    Serial.print("DHT11 Temp: "); Serial.print(dhtTemp); Serial.println(" °C");
    Serial.print("DHT11 Humidity: "); Serial.print(dhtHumidity); Serial.println(" %");
  }
  Serial.print("DS18B20 Temp: "); Serial.print(dsTemp); Serial.println(" °C");
  Serial.print("Thermistor Raw: "); Serial.println(thermistorValue);
  Serial.print("MQ-2 Gas Sensor Raw: "); Serial.println(mq2Value);
  Serial.println("----------------------------------\n");

  delay(60000); // Wait one minute (60,000 milliseconds)
}
