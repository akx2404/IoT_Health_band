#include "ArduinoJson.h"
StaticJsonDocument<200> doc;

#define RXp2 16
#define TXp2 17

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
}
void loop() {
    if(Serial2.available()){
     deserializeJson(doc, Serial2);
     const char* sensor_name = doc["sensor"];
     const int val_count = doc["count"];
     Serial.println(sensor_name);
     Serial.println(val_count);
     Serial.println();
   }
}
