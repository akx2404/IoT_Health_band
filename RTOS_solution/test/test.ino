//core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

//sensors 12c lib
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>
#define I2C_SDA 21
#define I2C_SCL 22
#define REPORTING_PERIOD_MS 1000

//firebase lib
#include "WiFi.h"
#include <FirebaseESP32.h>

//wifi and keys for firebase
const char* WIFI_SSID = "Akshad";
const char* WIFI_PASSWORD =  "internet";
#define API_KEY ""
#define DATABASE_URL ""
#define USER_EMAIL ""
#define USER_PASSWORD ""

//Define Firebase Data object
FirebaseData stream;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

uint8_t bm280_address = 0x76;
uint8_t max30100_address = 0x57;
uint8_t irmlx90614_address = 0x5A;
uint32_t tsLastReport = 0;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PulseOximeter pox;

//global var
int amb_temp, obj_temp, bpm, spo2;
String path = "/Test";


// function for firebase ------------------
void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
}

//------------------------------------------

void send_firebase(void *parameters){
  
  FirebaseJson json;
  FirebaseData fd;
  //pushing data to firebase
  fd.setFloat(firebaseData, path + "/ambient_temp" + amb_temp)
  fd.setFloat(firebaseData, path + "/object_temp" + obj_temp)
  fd.setFloat(firebaseData, path + "/bpm_reading" + bpm)
  fd.setFloat(firebaseData, path + "/spo2_reading" + spo2)
  /*
  json.add("ambient_temp", amb_temp);
  json.add("object_temp", obj_temp);
  json.add("bpm_reading", bpm);
  json.add("spo2_reading", spo2);
  */
}

void print_vaues(void *parameters){
  Serial.println(amb_temp);
  Serial.println(obj_temp);
  Serial.println(bpm);
  Serial.println(spo2);
  
}

void setup() {
  Serial.begin(115200);

//firebase and wifi connect
//------------------------------
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

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(stream, "/test/stream/data"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());
//-------------------------------

  Wire.begin();
  mlx.begin();
  pox.begin();

  xTaskCreatePinnedToCore(send_firebase,
                        "firebase",
                        1024,
                        NULL,
                        1,
                        NULL,
                        app_cpu);

  xTaskCreatePinnedToCore(print_vaues,
                        "display",
                        1024,
                        NULL,
                        1,
                        NULL,
                        app_cpu);
                        
  //vTaskDelete(NULL);
}

void loop() {
  pox.update();
  amp_temp = mlx.readAmbientTempC();
  obj_temp = mlx.readObjectTempC();
  bpm = pox.getHeartRate();
  spo2 = pox.getSpO2();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
