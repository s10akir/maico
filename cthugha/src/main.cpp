#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHTesp.h>
#include <nvs_flash.h>
#include <ssid_define.h>
#include <ip_define.h>

AsyncWebServer server(80);

DHTesp dht;
const int DHT_PIN = 32;
const DHTesp::DHT_MODEL_t DHT_TYPE = DHTesp::AM2302;

boolean connectWiFi() {
  Serial.print("WiFi Connecting");

  WiFi.begin(MY_SSID, MY_SSID_PASS);

  int count = 1;
  while (WiFi.status() != WL_CONNECTED) {
    if(count >= 100) { // 10sec
      WiFi.disconnect(true, true);
      Serial.println("");
      Serial.println("WiFi Connection Error.");
      Serial.println(WiFi.status());
      return false;
    }

    Serial.print(".");
    count++;
    delay(100);
  }

  Serial.println("");
  Serial.println("WiFi Connected.");

  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet: ");
  Serial.println(WiFi.subnetMask());

  return true;
}

void setup() {
  Serial.begin(115200);

  dht.setup(DHT_PIN, DHT_TYPE); 

  WiFi.config(ip, gateway, subnet);

  /* create WiFi Connection*/
  int count = 1;
  while(!connectWiFi()) {
    if (count >= 3) {
      Serial.println("WiFi Error. Reset nvs flash and Restart ESP32.");
      ESP_ERROR_CHECK(nvs_flash_erase());
      nvs_flash_init();
      ESP.restart();
    }

    Serial.println("Retry after 15 seconds.");
    count++;
    delay(15000);
  }

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    /* fetch temp and humi from DHT22 */
    // NOTE: 2秒程度待機しないとNaNになる可能性あり
    TempAndHumidity values = dht.getTempAndHumidity();
    float t = values.temperature;
    float h = values.humidity;

    request->send(200, "application/json", "{\"data\": {\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + "}}");
  });

  server.begin();
}

void loop() {
  // TODO: WiFiが切断された場合にリセットする
}