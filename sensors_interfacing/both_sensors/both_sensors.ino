#include <Wire.h>  

#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

uint32_t tsLastReport = 0;
void onBeatDetected()
{
    Serial.println("Beat!");
}
 
void setup()
{
    Serial.begin(115200);
    mlx.begin(); 
    if (!mlx.begin()) {
      Serial.println("Error connecting to MLX sensor. Check wiring.");
      while (1);
    };
  
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}
 
void loop()
{
    
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      double hr = pox.getHeartRate();
      double sp = pox.getSpO2();
      //if (hr>50 && sp>0){
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
      //}
        tsLastReport = millis();

        Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
        Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
        Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
        Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
        Serial.println();
        
    }


}
