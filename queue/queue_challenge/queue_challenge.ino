#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
static const int led_pin = 2;
static const uint8_t queue_size = 5;
static const char blinkedStr[] = "Blinked";

static QueueHandle_t queue1;
static QueueHandle_t queue2;

void taskA(void *parameters) {
  char item[20];
  char matchStr[] = "delay ";
  char delay_str[7];
  bool parseNum = false;
  int delay_count = 0;
  uint8_t matchIdx = 0;
  uint8_t parseIdx = 0;
  while(1) {

    while (xQueueReceive(queue2, (void*)&item, 0) == pdTRUE) {
      Serial.println(item);
      memset(item, 0, strlen(item));
    }
    
    while(Serial.available() > 0) {
      char c = Serial.read();
      Serial.print(c);

      if (parseNum == true) {
        if (c == '\n') {
          delay_count = atoi(delay_str);
          parseNum = false;
          parseIdx = 0;
          memset(delay_str, 0, strlen(delay_str));

          // Serial.print("Adding to queue 1: ");
          // Serial.println(delay_count);
          
          if (xQueueSend(queue1, (void*)&delay_count, 0) != pdTRUE) {
            Serial.println("Queue 1 full");
          }

        } else {
          // Serial.print(c);
          delay_str[parseIdx++] = c;
        }
      }
      else if (c == matchStr[matchIdx]) {
        matchIdx++;
        if (matchIdx == strlen(matchStr)){
          matchIdx = 0;
          parseNum = true;
        }
      }
      else {
        matchIdx = 0;
      }
    }
  }
}


void taskB(void *parameters) {
  int blinkCount = 0;
  int t = 1000;
  while(1) {
    while(xQueueReceive(queue1, (void *)&t, 0) == pdTRUE){
    }
  
    digitalWrite(led_pin, HIGH);
    vTaskDelay(t / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(t / portTICK_PERIOD_MS);
    blinkCount++;

    if ((blinkCount % 100) == 0) {
      if (xQueueSend(queue2, (void *)&blinkedStr, 0) != pdTRUE) {
        // don't print here as it might get preempted and disturb the serial output of taskA
      }
    }
  }  
}

void setup() {
  pinMode(led_pin, OUTPUT);

  // put your setup code here, to run once:
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("Queue program");

  queue1 = xQueueCreate(queue_size, sizeof(int));
  queue2 = xQueueCreate(queue_size, sizeof(blinkedStr));

  xTaskCreatePinnedToCore( 
    taskA,
    "Task_A",
    1024, // stack size in bytes (words in FreeRTOS)
    NULL, // parameter to pass to function
    1, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    NULL, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );

  xTaskCreatePinnedToCore( 
    taskB,
    "Task_B",
    1024, // stack size in bytes (words in FreeRTOS)
    NULL, // parameter to pass to function
    1, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    NULL, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
