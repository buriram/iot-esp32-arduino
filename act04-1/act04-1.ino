// https://github.com/nhatuan84/esp32-webserver
#include <WiFi.h>
#include <ESP32WebServer.h>
#include <FS.h>
#include "SPIFFS.h"

char ssid[64] = "VEC-WiFi";
char pass[64] = "";

ESP32WebServer server(80);

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  // read config
  File configFile = SPIFFS.open("/config.txt", "r");
  if (!configFile || configFile.size() == 0) {
    Serial.println("CONFIG: SPIFFS INIT");
    SPIFFS.format();
    Serial.println("CONFIG: SPIFFS FORMAT DONE");
    // TODO: save config file
    configFile = SPIFFS.open("/config.txt", "w");
    configFile.printf("%s\n", ssid);
    configFile.printf("%s\n", pass);
    configFile.close();
    configFile = SPIFFS.open("/config.txt", "r");
  }
  // cjson
  Serial.printf("size=%d\n", configFile.size());
  configFile.readBytesUntil('\n', ssid, sizeof(ssid));
  configFile.readBytesUntil('\n', pass, sizeof(pass));
  configFile.close();
  Serial.printf("ssid=%s\n", ssid);
  Serial.printf("pass=%s\n", pass);
  WiFi.mode(WIFI_AP_STA);
//  WiFi.mode(WIFI_AP);
//  WiFi.mode(WIFI_STA);
  IPAddress Ip(192, 168, 100, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  WiFi.softAP("IoT Naja", "12345678");

  WiFi.begin(ssid, pass);
  Serial.println("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) { // รอจนกว่าจะต่อ WiFi ได้
    Serial.print(".");
    delay(500);
  }
  IPAddress myIP = WiFi.localIP();
  Serial.print("\nIP=");
  Serial.println(myIP);
 
  server.on("/", getHomePage);
  server.on("/save", saveConfig);
  server.begin();
}

void loop() {
  server.handleClient();
}

void getHomePage() {
  server.send(200, "text/html", "<html>\
<body>\
  <form method=get action=\"/save\">\
    <div>SSID: <input type=text name=\"ssid\" value=\"\"></div>\
    <div>PASS: <input type=text name=\"pass\" value=\"\"></div>\
    <input type=\"submit\" value=\"SAVE\">\
  </form>\
  <h1>Hello World</h1>\
</body>\
</html>");
}

void saveConfig() {
  server.arg(0).toCharArray(ssid, sizeof(ssid));
  server.arg(1).toCharArray(pass, sizeof(pass));
  Serial.printf("new ssid=%s\n", ssid);
  Serial.printf("new pass=%s\n", pass);
  // TODO: save to file
  // Redirect to /
  server.send(200, "text/html", "<script>window.location='/'</script>");
}

