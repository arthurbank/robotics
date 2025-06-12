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
  // Read DHT11
  float dhtTemp = dht.readTemperature();
  float dhtHumidity = dht.readHumidity();

  // Read DS18B20
  ds18b20.requestTemperatures();
  float dsTemp = ds18b20.getTempCByIndex(0);

  // Read Thermistor (analog raw value)
  int rawADC = analogRead(THERMISTOR_PIN);
  float thermistor_voltage = rawADC / 1023.0 * 5.0;
  float thermistor_resistance = (5.0 * seriesResistor / thermistor_voltage) - seriesResistor;

  float steinhart;
  steinhart = thermistor_resistance / nominalResistance;
  steinhart = log(steinhart);
  steinhart /= bCoefficient;
  steinhart += 1.0 / (nominalTemperature + 273.15);
  steinhart = 1.0 / steinhart;
  float thermistorC = steinhart - 273.15;

  int mq2ADC = analogRead(MQ2_PIN);
  float gas_voltage = mq2ADC / 1023.0 * 5.0;
  float rs = (5.0 - gas_voltage) / gas_voltage * 10.0; // RS = (Vc - Vrl) / Vrl * RL
  float ratio = rs / R0;

  // LPG ppm (approximate log formula from MQ-2 datasheet)
  float ppm = pow(10, ((-log10(ratio) + 0.21) / 0.47)); 

  // Print all values
  Serial.println("------ Weather Station Data ------");
  if (isnan(dhtTemp) || isnan(dhtHumidity)) {
    Serial.println("DHT11 read failed!");
  } else {
    Serial.print("DHT11 Temp: "); Serial.print(dhtTemp); Serial.println(" °C");
    Serial.print("DHT11 Humidity: "); Serial.print(dhtHumidity); Serial.println(" %");
  }
  Serial.print("DS18B20 Temp: "); Serial.print(dsTemp); Serial.println(" °C");
  Serial.print("Thermistor Temp: ");
  Serial.print(thermistorC);  
  Serial.println(" °C");
  Serial.print("Gas Concentration: ");
  Serial.print(ppm);
  Serial.println(" ppm");
  Serial.println("----------------------------------\n");

  delay(60000); // Wait one minute (60,000 milliseconds)
}
