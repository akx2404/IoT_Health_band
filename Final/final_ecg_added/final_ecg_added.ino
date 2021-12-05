#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include "ArduinoJson.h"
StaticJsonDocument<200> doc;

#define RXp2 16
#define TXp2 17

double hr_sum;
double avg_bpm;
double sp_sum;
double avg_sp;
int counthr = 1;
int countsp = 1;

#define ir 14

#include "WiFi.h"
#include <Firebase_ESP_Client.h>
#include <driver/adc.h>
#include "time.h"
#include<TimeLib.h>
#include<ESP32Time.h>

const char* WIFI_SSID = "Akshad";
const char* WIFI_PASSWORD =  "internet";

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define FIREBASE_HOST "https://uart-esp32-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyCy3pK8YZkivrp49g52Cnu3FY5RfUGVIO0"
#define USER_EMAIL "akx2404@gmail.com"
#define USER_PASSWORD "akxakx@giis"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
String newPath;


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



void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);

  pinMode(41, INPUT); // Setup for leads off detection LO +
  pinMode(40, INPUT); // Setup for leads off detection LO -

  mlx.begin();
  pinMode(ir, INPUT);

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
}

int bpm_arr[8];
int spo2_arr[8];
int body;
int ecg;
void loop()
{

  int j = 0;
  body = digitalRead(ir);
  
  if(Serial2.available() > 0){
    deserializeJson(doc, Serial2);    

  /*for (j = 0; j < 8; j++) {
    hr_sum = hr_sum + bpm_arr[j];
    sp_sum = sp_sum + spo2_arr[j];
    delay(20);
  }*/
    if((digitalRead(40) == 1)||(digitalRead(41) == 1)){
      Serial.println('!');
    }
    else{
      // send the value of analog input 0:
        ecg = analogRead(A0);
    }
  
    /*const double hr = doc["hr"];
    const double spo2 = doc["spo2"];*/
    double temp_obj = mlx.readObjectTempC();
    double temp_amb = mlx.readAmbientTempC();
    const double hr = doc["hr"];
    const double sp = doc["spo2"];

    if(body == 0){
    if (counthr<100 || countsp<100){
      hr_sum = hr_sum + hr;
      avg_bpm = hr_sum/counthr;
      sp_sum = sp_sum + sp;
      avg_sp = sp_sum/countsp;
      if (hr!=0){
        counthr++;
      }
      if (sp!=0){
        countsp++;
      }
    }
    }
    else if(body == 1){
      counthr = 1;
      countsp = 1;
      hr_sum = 0;
      sp_sum = 0;
      avg_bpm = 0;
      avg_sp = 0;
    }
    
    //Serial.print("hr - ");
    //Serial.println(hr);
    Serial.print("avg hr - ");
    Serial.println(avg_bpm);
    //Serial.print("spo2 - ");
    //Serial.println(sp);
    Serial.print("avg spo2 - ");
    Serial.println(avg_sp);
    Serial.print("temp_obj - ");
    Serial.println(temp_obj);
    //Serial.print("temp_amb - ");
    //Serial.println(temp_amb);
    Serial.print("ecg - ");
    Serial.println(ecg);
    Serial.println();

    FirebaseJson json;
    json.set("bpm", avg_bpm);
    json.set("spo2", avg_sp);
    json.set("body_temp", temp_obj);
    json.set("ECG", ecg);
    //json.set("ambient_temp", temp_amb);
    json.set("timestamp", printLocalTime());
    newPath = "/readings";
    Firebase.RTDB.push(&fbdo, newPath.c_str(), &json);
    //Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data/readings", json) ? "ok" : fbdo.errorReason().c_str());

  }
}
