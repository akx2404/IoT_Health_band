#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const uint8_t msg_queue_len = 5;

//globals
static QueueHandle_t msg_queue;
static const int led_pin = 2;

void send_data(void *parameters) {

  int inp;
  int led_delay;

  while (1) {
    if (Serial.available()){
      inp = Serial.parseInt();
      led_delay = inp;
      Serial.println(led_delay);

    if (xQueueSend(msg_queue, (void *)&led_delay, 10) != pdTRUE) {
          Serial.println("ERROR: Could not put item on delay queue.");
        }
    }
  }
}

void blink_led(void *parameters) {

  int led_delay = 500;
  pinMode(led_pin, OUTPUT);

  while (1) {
    if (xQueueReceive(msg_queue, (void *)&led_delay, 0) == pdTRUE) {
      //do nothing
    }

    Serial.print(led_delay);
    
    // Blink
    digitalWrite(led_pin, HIGH);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  lcd.begin(16,2);
  lcd.backlight();
  lcd.setCursor(0,0);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("RTOS to control blink led delay using queue");
  Serial.println("LED blink delay time in milliseconds");

  // Create queues
  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));

  xTaskCreatePinnedToCore(blink_led,
                          "Blink data task",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  xTaskCreatePinnedToCore(send_data,
                          "send data task",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);

  
  vTaskDelete(NULL);

}

void loop() {

}
