#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

float temperature;
float humidity;
DHT dht14(D4, DHT11);

const char* writeKey = "WBMM50BF5HVYXR7H";
unsigned long channelID = 2339419;
unsigned long timeDelay = 15000;

WiFiClient client;

void init_wifi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - startTime > 30000) { // 30 seconds timeout
      Serial.println("\nFailed to connect to WiFi. Please check credentials");
      return;
    }
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  init_wifi("Feezoh", "123456Ff");
  dht14.begin();
  ThingSpeak.begin(client);
}

void getSensor() {
  float newHumidity = dht14.readHumidity();
  float newTemperature = dht14.readTemperature();
  if (isnan(newHumidity) || isnan(newTemperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  humidity = newHumidity;
  temperature = newTemperature;
  ThingSpeak.setField(1, humidity);
  ThingSpeak.setField(2, temperature);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    getSensor();
    int x = ThingSpeak.writeFields(channelID, writeKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  } else {
    Serial.println("WiFi not connected. Attempting reconnection.");
    init_wifi("iPhone_ohm", "1234567890");
  }
  delay(timeDelay);
}
