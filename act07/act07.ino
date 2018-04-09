#include <WiFi.h>
#include <WiFiClientSecure.h>

char ssid[64] = "VEC-WiFi";
char pass[64] = "";
char hotspotUser[64] = "top37";
char hotspotPass[64] = "7997";
unsigned long uploadTimer = 0;
WiFiClient hotspotClient;
WiFiClientSecure client;

unsigned long t = 0;

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
  if (now - uploadTimer > 10000) {
    uploadTimer = now;
    Serial.println("Start upload...");
    doLineNoti();
  }
}

bool doHotspotAuth() {
  if (!hotspotClient.connect("hotspot.bpcd.go.th", 80)) {
    return false;
  }
  char postData[200];
  sprintf(postData, "dst2=http://google.co.th&popup2=true&username=%s&password=%s", hotspotUser, hotspotPass);
  int len = strlen(postData);
  hotspotClient.printf("POST /login HTTP/1.0\r\nHost: hotspot.bpcd.go.th\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\nConnection: Close\r\n\r\n%s", len, postData);
  unsigned long timeout = millis();
  while (hotspotClient.available() == 0) {// รอจนกว่าจะได้ข้อมูล แต่ไม่เกิน 5 วินาที
    if (millis() - timeout > 5000) {
      hotspotClient.stop();
      Serial.println("Hotspot Authen FAILED!!!");
      return false;
    }
  }
  while(hotspotClient.available()){ // อ่านข้อมูลที่ได้รับจนหมด
    String line = hotspotClient.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println("Hotspot Authen SUCCESS!!!");
  return true;
}

void doLineNoti() {
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("Sorry...");
    return;
  }
  Serial.println("Connnect OK");
  char text[1000] = "";
  sprintf(text, "message=Hello&stickerPackageId=1&stickerId=102");
  char notiToken[] = "uXxrIYHOn8cLzg3QimzMFRlnDhyKdTSEegJ4u0Swh7q";
  client.printf("POST /api/notify HTTP/1.0\r\nHost: notify-api.line.me\r\nAuthorization: Bearer %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s", notiToken, strlen(text), text);
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

