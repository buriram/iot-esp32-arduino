#include <WiFi.h>

char ssid[64] = "VEC-WiFi";
char pass[64] = "";
char hotspotUser[64] = "top37";
char hotspotPass[64] = "7997";
unsigned long uploadTimer = 0;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.println("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) { // รอจนกว่าจะต่อ WiFi ได้
    Serial.print(".");
    delay(500);
  }
  IPAddress myIP = WiFi.localIP();
  Serial.print("\nIP=");
  Serial.println(myIP);
  bool ok = doHotspotAuth();
}

void loop() {
  unsigned long now = millis();
  if (now - uploadTimer > 5000) {
    uploadTimer = now;
    Serial.println("Start upload...");
    doUpload();
  }
}

bool doHotspotAuth() {
  if (!client.connect("hotspot.bpcd.go.th", 80)) {
    return false;
  }
  char postData[200];
  sprintf(postData, "dst2=http://google.co.th&popup2=true&username=%s&password=%s", hotspotUser, hotspotPass);
  int len = strlen(postData);
  client.printf("POST /login HTTP/1.0\r\nHost: hotspot.bpcd.go.th\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\nConnection: Close\r\n\r\n%s", len, postData);
  unsigned long timeout = millis();
  while (client.available() == 0) {// รอจนกว่าจะได้ข้อมูล แต่ไม่เกิน 5 วินาที
    if (millis() - timeout > 5000) {
      client.stop();
      Serial.println("Hotspot Authen FAILED!!!");
      return false;
    }
  }
  while(client.available()){ // อ่านข้อมูลที่ได้รับจนหมด
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println("Hotspot Authen SUCCESS!!!");
  return true;
}

void doUpload() {
  if (!client.connect("iot.xenex.io", 80)) {
    return;
  }
  int num = random(0, 9);
  client.printf("GET /lotto/%d HTTP/1.0\r\nHost: iot.xenex.io\r\nConnection: Close\r\n", num);
  unsigned long timeout = millis();
  while (client.available() == 0) {// รอจนกว่าจะได้ข้อมูล แต่ไม่เกิน 5 วินาที
    if (millis() - timeout > 5000) {
      client.stop();
      Serial.println("Upload FAILED!!!");
      return;
    }
  }
  while(client.available()){ // อ่านข้อมูลที่ได้รับจนหมด
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println("Upload SUCCESS!!!");
  return;
}

