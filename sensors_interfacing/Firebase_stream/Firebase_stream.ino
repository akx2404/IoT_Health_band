//Firebase + wifi + time libraries
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <driver/adc.h>
#include "time.h"
#include<TimeLib.h>
#include<ESP32Time.h>

//Firebase credentials
/* 1. Define the WiFi credentials */
const char* WIFI_SSID = "Akshad";
const char* WIFI_PASSWORD =  "internet";

/* 2. Define the Firebase project host name and API Key */
#define FIREBASE_HOST "https://uart-esp32-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyCy3pK8YZkivrp49g52Cnu3FY5RfUGVIO0"

/* 3. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "akx2404@gmail.com"
#define USER_PASSWORD "akxakx@giis"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

FirebaseJson json;
String newPath;

//Sensor libraries 
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

//For timestamp
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

struct tm timeinfo;
String printLocalTime()
{
  char s[51];
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  strftime(s, 50, "%A, %B %d %Y %H:%M:%S", &timeinfo);
  return String(s);
}


void setup()
{
    Serial.begin(115200);



    analogReadResolution(10); // Default of 12 is not very linear. Recommended to use 10 or 11 depending on needed resolution.
    analogSetAttenuation(ADC_6db); // Default is 11db which is very noisy. Recommended to use 2.5 or 6.
  
    adc1_config_width(ADC_WIDTH_BIT_10);
    
    adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_DB_0);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  
    /* Assign the project host and api key (required) */
    config.host = FIREBASE_HOST;
    config.api_key = API_KEY;
  
    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
  
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
  
    #if defined(ESP8266)
      //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
      fbdo.setBSSLBufferSize(1024, 1024);
    #endif
  
    //Set the size of HTTP response buffers in the case where we want to work with large data.
    fbdo.setResponseSize(1024);
  
    //Set database read timeout to 1 minute (max 15 minutes)
    Firebase.RTDB.setReadTimeout(&fbdo, 1000 * 60);
    //tiny, small, medium, large and unlimited.
    //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
    Firebase.RTDB.setwriteSizeLimit(&fbdo, "tiny");
  
    //optional, set the decimal places for float and double data to be stored in database
    Firebase.setFloatDigits(2);
    Firebase.setDoubleDigits(6);
  
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    //printLocalTime();
  
  
      
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
    } 
    else {
        Serial.println("SUCCESS");
    }
     pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}


void loop()
{
    double hr,sp,temp_object,temp_ambient;
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      
    
      hr = pox.getHeartRate();
      sp = pox.getSpO2();
      //if (hr>50 && sp>0){
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");

        
      //}
        pox.shutdown();

        temp_object = mlx.readAmbientTempC();
        temp_ambient = mlx.readObjectTempC();
        Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
        Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
        Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
        Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
        Serial.println();
               
        
        FirebaseJson json1;
        json1.set("sp", sp);
        json1.set("bpm", hr);
        json1.set("temp_object", temp_object);
        json1.set("temp_ambient", temp_ambient);
        json1.set("timestamp", printLocalTime());
        newPath = "/readings";
        Firebase.RTDB.push(&fbdo, newPath.c_str(), &json1);

        pox.resume();
        tsLastReport = millis();

    }
}
