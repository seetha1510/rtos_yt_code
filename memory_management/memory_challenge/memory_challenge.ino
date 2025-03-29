#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static TaskHandle_t taskHandleA = NULL;
static TaskHandle_t taskHandleB = NULL;
static char *ptr = NULL;
// static volatile uint8_t msg_flag = 0;
static int val =0;

void taskA(void *parameter) {
  uint8_t i = 0;
  char buf[255];

  while(1) {
    while(Serial.available() > 0) {
      char c = Serial.read();

      if(c != '\n'){
        buf[i] = c;
        i++;
      }
      else {
        buf[i] = '\0';
        i++;
        if (val == 0) {
          ptr = (char*)pvPortMalloc(i * sizeof(char));
          if (ptr == 0) {
            Serial.println("Failed to allocate heap");
            configASSERT(ptr);
          }
          else {
            memcpy(ptr, buf, i);
            xTaskNotifyGive(taskHandleB);
            // msg_flag = 1;
          }
        }
        memset(buf, 0, 255);
        i = 0;
      }
    }
  } 
}

void taskB(void *parameter) {
  while(1){
    val = ulTaskNotifyTake(pdFALSE, 100 / portTICK_PERIOD_MS);
    if (val != 0) {
      Serial.println("printing: ");
      Serial.print("Free heap (bytes): ");
      Serial.println(xPortGetFreeHeapSize());
      Serial.println(ptr);
      vPortFree(ptr);
      ptr = NULL;
      // msg_flag = 0;
      val = 0;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("Notify program");

  xTaskCreatePinnedToCore( 
    taskA,
    "Task_A",
    1024, // stack size in bytes (words in FreeRTOS)
    NULL, // parameter to pass to function
    1, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    &taskHandleA, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );

  xTaskCreatePinnedToCore( 
    taskB,
    "Task_B",
    1024, // stack size in bytes (words in FreeRTOS)
    NULL, // parameter to pass to function
    1, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    &taskHandleB, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
