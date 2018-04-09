#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long timer = 0;
bool mqttReady = false;

void setup() {
  Serial.begin(115200);
  SPI.begin(5, 19, 27, 18); // SCK, MISO, MOSI, CS
  LoRa.setPins(18, 14, 26); // SS, RST, D0
  if (!LoRa.begin(433E6)) { // 433MHz หรือ 915E6 = 915MHz
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.enableCrc(); // เพิ่มการตรวจสอบความถูกต้องข้อมูล

  WiFi.begin("VEC-WiFi", "");
  Serial.println("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) { // รอจนกว่าจะต่อ WiFi ได้
    Serial.print(".");
    delay(500);
  }
//  client.setServer("mqtt.iot.xenex.io", 1883);
  client.setServer("192.168.14.38", 1883);
  client.setCallback(mqttCallback);
}

void loop() {
  unsigned long now = millis();
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    char loraData[128];
    int idx = 0;
    while (LoRa.available()) {
      loraData[idx++] = LoRa.read();
      if (idx >= 127) {
        break;
      }
    }
    loraData[idx] = 0;
    Serial.printf("Received packet '%s' with RSSI %d\n", loraData, LoRa.packetRssi());
    if (mqttReady) {
      client.publish("lora", loraData, true);
    }
  }
  if (now - timer > 5000) {
    timer = now;
    LoRa.beginPacket(); 
    LoRa.print("name=Somsak"); //
//    LoRa.print(65); // พิมพ์ตัวเลขเป็นข้อความได้ "65"
//    LoRa.write(65); // พิมพ์ ascii 65 จะได้ "A"
    LoRa.endPacket();
  }
  if (!client.connected()) {
    mqttReady = false;
    while (!client.connected()) {
      char mac[20];
      WiFi.macAddress().toCharArray(mac, sizeof(mac));
      if (client.connect(mac, "user", "password")) {
        // เชื่อมต่อสำเร็จ ทำ subscribe ที่นี่
        client.subscribe("iot-cmd");
        client.subscribe("node-0001");
      } else { delay(5000); }
    } // while
  } else {
    mqttReady = true;
  }
  client.loop();
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  if (strcmp(topic, "iot-cmd") == 0) {
    payload[len] = 0; // R=1\0dfdfdsfsdf, 3
    if (strcmp("R=1", (char *)payload) == 0) {
      Serial.println("LED=ON");
      digitalWrite(25, 1);
    } else {
      Serial.println("LED=OFF");
      digitalWrite(25, 0);      
    }
  }
}

