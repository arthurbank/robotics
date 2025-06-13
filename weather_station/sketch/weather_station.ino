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
const float seriesResistor = 10000.0; // 10k resistor in the voltage divider
const float nominalResistance = 10000.0; // resistance at 25°C
const float nominalTemperature = 25.0;
const float bCoefficient = 3950.0; // beta coefficient

#define MQ2_PIN A1
float R0 = 10.0;  // You should calibrate this in clean air


void setup() {
  Serial.begin(9600);
  dht.begin();
  ds18b20.begin();
  Serial.println("Weather Station Starting...");
}

void loop() {
  float dhtTemp = dht.readTemperature();
  float dhtHumidity = dht.readHumidity();
  ds18b20.requestTemperatures();
  float dsTemp = ds18b20.getTempCByIndex(0);

  int rawADC = analogRead(THERMISTOR_PIN);
  float thermistor_voltage = rawADC / 1023.0 * 5.0;
  float thermistor_resistance = (5.0 * seriesResistor / thermistor_voltage) - seriesResistor;
  float steinhart = thermistor_resistance / nominalResistance;
  steinhart = log(steinhart);
  steinhart /= bCoefficient;
  steinhart += 1.0 / (nominalTemperature + 273.15);
  steinhart = 1.0 / steinhart;
  float thermistorC = steinhart - 273.15;

  int mq2ADC = analogRead(MQ2_PIN);
  float gas_voltage = mq2ADC / 1023.0 * 5.0;
  float rs = (5.0 - gas_voltage) / gas_voltage * 10.0;
  float ratio = rs / R0;
  float ppm = pow(10, ((-log10(ratio) + 0.21) / 0.47)); 

  // Print CSV line: timestamp, DHT temp, humidity, DS temp, thermistor temp, gas ppm
  Serial.print(millis());
  Serial.print(",");
  Serial.print(dhtTemp);
  Serial.print(",");
  Serial.print(dhtHumidity);
  Serial.print(",");
  Serial.print(dsTemp);
  Serial.print(",");
  Serial.print(thermistorC);
  Serial.print(",");
  Serial.println(ppm);

  delay(60000);
}