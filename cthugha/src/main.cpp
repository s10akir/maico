#include <Arduino.h>
#include <WiFi.h>
#include <nvs_flash.h>
#include <ssid_define.h>
#include <ip_define.h>

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
}

void loop() {
}