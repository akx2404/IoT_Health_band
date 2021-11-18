#define THINGER_SERIAL_DEBUG
#include <ThingerESP32.h>
#include "keys.h"
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
float celc;

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

PulseOximeter pox; 
float h_rate, spO2;

void onBeatDetected()
{
    Serial.println("Beat Detected!");
}

void setup() {
  // open serial for debugging
  Serial.begin(115200);
  thing.add_wifi(SSID, SSID_PASSWORD);

  mlx.begin();

  /*if (!pox.begin())
    {
         Serial.println("FAILED");
         for(;;);
    }
    else
    {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }*/

  thing["MLX90614"] >> [](pson& out){   
      out["temperature"] = celc;
  };
  
  thing["MAX30100"] >> [](pson& out){
      out["Heart_rate"] = h_rate;
      out["SP02"] = spO2;
      
  };
}

void loop() {
  thing.handle();
  celc = mlx.readAmbientTempC();
  pox.update();
  h_rate = pox.getHeartRate();
  spO2 = pox.getSpO2();
}
