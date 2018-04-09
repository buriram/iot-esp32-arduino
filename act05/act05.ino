#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);
unsigned long t = 0;

void setup() {
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  // connect to wifi
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
  if (!client.connected()) {
    while (!client.connected()) {
      if (client.connect("sak", "user", "password")) {
        // เชื่อมต่อสำเร็จ ทำ subscribe ที่นี่
        client.subscribe("iot-cmd");
        client.subscribe("node-0001");
      } else { delay(5000); }
    } // while
  } else {
    if (now - t > 5000) {
      client.publish("sensor/sak", "T=25,H=60", true);
      t = now;
    }
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

