#include <WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "VEC-WiFi";
const char* password = "";

WiFiClientSecure client2;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) { // รอจนกว่าจะต่อ WiFi ได้
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected");
  doHotspotAuth();
}


void loop() {
  if (false) {
    if (!client.connect("iot.xenex.iot", 443)) {
      return;
    }
    
    client.printf("GET /lotto/%d HTTP/1.1\r\nHost: iot.xenex.io\r\nConnection: close\r\n\r\n", cnt);
    char data[100];
    sprintf(data, "id=%d&temp=%d&humi=%d&submit=Submit", 1, 25, 60);
    int len = strlen(data);
    client.printf("POST /upload.php HTTP/1.0
Host: iot.xenex.io
Content-Type: application/x-www-form-urlencoded
Content-Length: %d
Connection: Close

%s", len, data);

    unsigned long timeout = millis();
    while (client.available() == 0) {// รอจนกว่าจะได้ข้อมูล แต่ไม่เกิน 5 วินาที
      if (millis() - timeout > 5000) {
        client.stop();
        return;
      }
    }
    while(client.available()){ // อ่านข้อมูลที่ได้รับจนหมด
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
}


bool doHotspotAuth() {
  Serial.println("begin hotspot auth");
  if (!client2.connect("iot.xenex.iot", 443)) {
    Serial.println("Sorry!!");
    return false;
  }
  Serial.println("Checking...");
  client.print("GET / HTTP/1.1\r\nHost: iot.xenex.iot\r\nConnection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {// รอจนกว่าจะได้ข้อมูล แต่ไม่เกิน 5 วินาที
    if (millis() - timeout > 5000) {
      Serial.println("Timeout!!!");
      client.stop();
      return false;
    }
  }
  Serial.println("Got data");
  while(client.available()){ // อ่านข้อมูลที่ได้รับจนหมด
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  return true;
}

