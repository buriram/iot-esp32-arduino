#include <Wire.h>
#include "SSD1306.h"

#define MAX_BUFFER 64

char cmdBuffer[MAX_BUFFER];
char text[MAX_BUFFER];
bool showLogo = false;

SSD1306 display(0x3c, 4, 15); // Address, Data, Clock

int cnt = 0;
int idx = 0;
unsigned long timer = 0;
int scrollPos = 0;
int 

void setup() {
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);
  delay(50);
  display.init(); // setup
  display.flipScreenVertically();
  Serial.println("ESP READY");
}

void loop() {
  idx = 0;
  unsigned long t = millis();
  if (t - timer > 1000) {
    cnt++;
    timer = t;
  }
  if (Serial.available()) {
    while (Serial.available()) {
      cmdBuffer[idx++] = Serial.read();
      if (idx >= MAX_BUFFER - 1) {
        break;
      }
    }
    cmdBuffer[idx] = 0;
    if (cmdBuffer[0] == 'P') {
      strcpy(text, cmdBuffer + 1);
    } else if (cmdBuffer[0] == 'C') {
      text[0] = 0;      
    } else if (cmdBuffer[0] == 'L') {
      showLogo = true;
    }
  }
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Somsak Sae-Lim");
  char cntText[20];
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  sprintf(cntText, "%d", cnt);
  display.drawString(128, 0, cntText);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(64, 32, cmdBuffer + 1);  
  display.display();
}


