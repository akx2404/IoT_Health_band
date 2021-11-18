#include "ArduinoJson.h"
int i = 0;
const char *JSON;
StaticJsonDocument<200> doc;

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  /*
  char JSONMessage[] = "{\"SensorType\": \"Temperature\", \"Value\": 29}";

  StaticJsonBuffer<300> JSONBuffer;
  JsonObject& root = JSONBuffer.parseObject(JSONMessage);
 
  root.prettyPrintTo(Serial);
  */

  //sprintf(JSON, "{\"sensor\": \"temp (MAX30100)\",\"reading\": %d}", i);


  
  doc["sensor"] = "temp";
  doc["count"] = i;

  Serial.println();
  //serializeJson(doc, Serial);
  serializeJsonPretty(doc, Serial);
  i++;
  delay(1000);

}
