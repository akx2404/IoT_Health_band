//json
#include "ArduinoJson.h"
StaticJsonDocument<200> doc;

//uart pins
#define RXp2 16
#define TXp2 17

#include "WiFi.h"
#include <FirebaseESP32.h>

const char* WIFI_SSID = "Akshad";
const char* WIFI_PASSWORD =  "internet";

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define API_KEY "AIzaSyCy3pK8YZkivrp49g52Cnu3FY5RfUGVIO0"
#define DATABASE_URL "https://uart-esp32-default-rtdb.firebaseio.com/"
#define USER_EMAIL "akx2404@gmail.com"
#define USER_PASSWORD "akxakx@giis"

//Define Firebase Data object
FirebaseData stream;
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

void streamCallback(StreamData data)
{
  Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  printResult(data); //see addons/RTDBHelper.h
  Serial.println();
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
}

void setup()
{

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  //Firebase.begin(DATABASE_URL, "12cOS7SDVROu16ekFqIzbc06k9En9xhfqgdkqiqj");
  

  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(stream, "/data"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);
}

void loop()
{
  if(Serial2.available()){

    deserializeJson(doc, Serial2);
    const char* sensor_name = doc["sensor"];
    const int val_count = doc["count"];

    if (Firebase.ready())
    {
      FirebaseJson json;
      json.add("count", val_count);
      Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, "/data/readings", json) ? "ok" : fbdo.errorReason().c_str());
      
    }
    
  }
}
