#include <Arduino.h>
#include <WiFi.h>
#include <DHTesp.h>
#include <nvs_flash.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <ssid_define.h>
#include <ip_define.h>
#include <influxdb_define.h>

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point temperaturePoint("temperature");
Point humidityPoint("humidity");

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
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());

  return true;
}

void setup() {
  Serial.begin(115200);

  dht.setup(DHT_PIN, DHT_TYPE); 

  WiFi.config(ip, gateway, subnet, dns);

  temperaturePoint.addTag("device", "cthugha");
  humidityPoint.addTag("device", "cthugha");
}

void loop() {
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

  /* sync clock */
  timeSync(TZ_INFO, "ntp.nict.jp");

  /* validate influxDB connection */
  if (client.validateConnection()) {
    Serial.print("Connected to influxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  /* fetch temp and humi from DHT22 */
  // NOTE: 2秒程度待機しないとNaNになる可能性あり
  temperaturePoint.clearFields();
  humidityPoint.clearFields();
  temperaturePoint.addField("value", dht.getTemperature());
  humidityPoint.addField("value", dht.getHumidity());

  Serial.println(client.pointToLineProtocol(temperaturePoint));
  Serial.println(client.pointToLineProtocol(humidityPoint));

  /* post to influxDB */
  if (!client.writePoint(temperaturePoint)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  if (!client.writePoint(humidityPoint)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  WiFi.disconnect(true, true);

  Serial.println("Waiting 60 seconds...");
  delay(60000);
}