#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <driver/adc.h>
#include "DHT.h"
#include "time.h"
#include<TimeLib.h>
#include<ESP32Time.h>

#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);  

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

int led135 = 13;
int buzzer = 26;

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

  pinMode(led135, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(DHTPIN, INPUT);

  dht.begin();
  Serial.println("setup  done.");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  randomSeed(analogRead(0));
}

void loop() {
  
/*
  float humid = dht.readHumidity();
  float temp = dht.readTemperature();

  Serial.println("Humidity: "+String(humid));
  Serial.println("Temperature: "+String(temp));
  Serial.println("-----------------------------------------");
  // Check if any reads failed and exit early (to try again).
  if (isnan(humid) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  */

  float temp = random(10, 40);
  float humid = random(10, 100);

//  FirebaseJson json1;
//  json1.set("humidValue", humid);
//  json1.set("timeStamp", printLocalTime());
//
//  FirebaseJson json2;
//  json2.set("temperatureValue", temp);
//  json2.set("timeStamp",printLocalTime());

//  newPath = "/Chamber1/room/temp";
//  Firebase.RTDB.setInt(&fbdo, newPath.c_str(), temp);
//  newPath = "/Chamber1/room/humid";
//  Firebase.RTDB.setInt(&fbdo, newPath.c_str(), humid);
//  newPath = "/Chamber1/room/mq135";
//  Firebase.RTDB.setInt(&fbdo, newPath.c_str(), humid+42);
//  newPath = "/Chamber1/room/mq2";
//  Firebase.RTDB.setInt(&fbdo, newPath.c_str(), humid-11);
//  newPath = "/Chamber1/room/mq9";
//  Firebase.RTDB.setInt(&fbdo, newPath.c_str(), humid+16);

  FirebaseJson json1;
  json1.set("value", humid);
  json1.set("timestamp", printLocalTime());
  newPath = "/Chamber1/room/humid";
  Firebase.RTDB.push(&fbdo, newPath.c_str(), &json1);

  FirebaseJson json2;
  json2.set("value", temp);
  json2.set("timestamp",printLocalTime());
  newPath = "/Chamber1/room/temp";
  Firebase.RTDB.pushJSON(&fbdo, newPath.c_str(), &json2);

  FirebaseJson json3;
  json3.set("value", humid+42.00);
  json3.set("timestamp",printLocalTime());
  newPath = "/Chamber1/room/mq135";
  Firebase.RTDB.pushJSON(&fbdo, newPath.c_str(), &json3);

  FirebaseJson json4;
  json4.set("value", humid-11.00);
  json4.set("timestamp",printLocalTime());
  newPath = "/Chamber1/room/mq2";
  Firebase.RTDB.pushJSON(&fbdo, newPath.c_str(), &json4);

  FirebaseJson json5;
  json5.set("value", humid+16.00);
  json5.set("timestamp",printLocalTime());
  newPath = "/Chamber1/room/mq9";
  Firebase.RTDB.pushJSON(&fbdo, newPath.c_str(), &json5);
}
