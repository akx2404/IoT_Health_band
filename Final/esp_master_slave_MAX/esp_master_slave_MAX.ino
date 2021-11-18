#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include "ArduinoJson.h"
StaticJsonDocument<200> doc;

#define RXp2 16
#define TXp2 17

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
}
void loop()
{
  if(Serial2.available() > 0){
    deserializeJson(doc, Serial2);
    
    const double hr = doc["hr"];
    const double spo2 = doc["spo2"];

    Serial.println(hr);
    Serial.println(spo2);
    Serial.println();

    /*deserializeJson(doc, Serial2);
    const double hr = doc["hr"];
    const double spo2 = doc["spo2"];

    if (Firebase.ready())
    {
      FirebaseJson json;
      json.add("hr", hr);
      json.add("spo2", spo2);
      Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data/readings", json) ? "ok" : fbdo.errorReason().c_str());
      
    }

    Serial.println(hr);
    Serial.println(spo2);
    Serial.println();

    delay(1000);*/
    
  }
}
