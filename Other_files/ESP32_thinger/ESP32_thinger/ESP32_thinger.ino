#define THINGER_SERIAL_DEBUG
#include <ThingerESP32.h>
#include "keys.h"
#include "DHT.h"

#define Built_in_led 2;

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

#define DHTPIN 4 
#define DHTTYPE DHT11 
DHT dht11(DHTPIN, DHTTYPE); 
float temp,hum;


void setup() {
  // open serial for debugging
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(2, LOW);

  thing.add_wifi(SSID, SSID_PASSWORD);

  dht11.begin(); 
  thing["dht11"] >> [](pson& out){
      out["temperature"] = temp;
      out["humidity"] = hum;
  };

  thing["led"] << digitalPin(Built_in_led);
}

void loop() {
  thing.handle();

  //Temperature values obtained in the loop so that they keep updating every <delay> seconds/microseconds.
  //Since there is no delay, the time period defined in the thinger dashboard/widget will be considered and the values will be plotted accordingly.
  //Thinger is better than thinkspeak because of the feature of sampling the values at a rate less than <value> per 20 seconds.
  //thinger connection and coding involves using pre defined functions and hence does not give us the flexibility of changing the delay or changing even the display messages.
  //With RTOS, thinger could connect to the esp32 as task-1 while the other task(s) could handle collecting/publishing data to the same.
  //RTOS with firebase has to be done
  temp = dht11.readTemperature();
  hum = dht11.readHumidity();
}
