#define ESP32_RTOS
#include "OTA.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  setupOTA("TemplateSketch", "Buffalo-G-9EC8", "j6pmvtvgfy48k");
  // Your setup code

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
}

void loop() {
  // Your code here
}