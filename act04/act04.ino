// https://github.com/me-no-dev/AsyncTCP
// https://github.com/me-no-dev/ESPAsyncWebServer
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

char ssid[64] = "xenex-ap";
char pass[64] = "6626754555";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.println("WiFi Connecting...");
  while (WiFi.status() != WL_CONNECTED) { // รอจนกว่าจะต่อ WiFi ได้
    Serial.print(".");
    delay(500);
  }
  IPAddress myIP = WiFi.localIP();
  Serial.print("IP=");
  Serial.println(myIP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><body>Welcome</body></html>");
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}
