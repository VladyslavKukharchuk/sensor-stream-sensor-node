#include "DHT.h"
#include <WiFi.h>
#include "secrets.h"

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 start!");
  dht.begin();
  connectToWiFi();
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
