#include "ArduinoJson.h"
int i = 0;
const char *JSON;
StaticJsonDocument<200> doc;

#include <Wire.h>  
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected()
{
}

void setup() {
  Serial.begin(115200);

  if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
  
}

void loop() {
  double  hr, sp;

  //sprintf(JSON, "{\"sensor\": \"temp (MAX30100)\",\"reading\": %d}", i);

  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    double hr = pox.getHeartRate();
    double sp = pox.getSpO2();

      doc["hr"] = hr;
      doc["spo2"] = sp;
    
      Serial.println();
      //serializeJson(doc, Serial);
      serializeJsonPretty(doc, Serial);
      
      tsLastReport = millis();
  }

}
