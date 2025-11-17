#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"
#include <time.h>

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7200; // UTC+2
const int daylightOffset_sec = 0;

String deviceId;

void setupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

String getTimestamp() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buf);
}

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

  deviceId = WiFi.macAddress();
  Serial.print("Device MAC: ");
  Serial.println(deviceId);
}

void sendData(float temperature, float humidity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(SERVER_URL) + "/api/v1/measurements");
    http.addHeader("Content-Type", "application/json");

    String timestamp = getTimestamp();

    String payload = "{\"device_id\":\"" + deviceId + "\"" +
                     ",\"temperature\":" + String(temperature, 2) +
                     ",\"humidity\":" + String(humidity, 2) + 
                     ",\"timestamp\":\"" + timestamp + "\"}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 start!");
  dht.begin();
  connectToWiFi();
  setupTime();
}

unsigned long lastSendTime = 0;

void loop() {
  unsigned long now = millis();

  if (now - lastSendTime >= SEND_INTERVAL_SEC * 1000UL) {
    lastSendTime = now;

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

      sendData(t, h);
    }
  }
}
