#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 start!");
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Error read from DHT22!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("%  |  Temperature: ");
    Serial.print(t);
    Serial.println("°C");
  }

  delay(2000);\
}
