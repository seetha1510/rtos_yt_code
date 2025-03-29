#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = 2;

const char msg[] = "SOmething here and there, maybe some more - long longer longest";

static TaskHandle_t taskHandle1 = NULL;
static TaskHandle_t taskHandle2 = NULL;

void task1(void *parameter) {
  int msg_len = strlen(msg);
  while(1) {
    Serial.println();
    for (int i=0; i<msg_len; i++){
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task2(void *parameter) {
  while(1){
    Serial.print("*");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(300);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  char buffer[40];
  Serial.println();
  sprintf(buffer, "Running on core %d with priority %d", xPortGetCoreID(), uxTaskPriorityGet(NULL));
  Serial.println(buffer);
  
  xTaskCreatePinnedToCore( 
    task1,
    "Task_1",
    1024, // stack size in bytes (words in FreeRTOS)
    NULL, // parameter to pass to function
    1, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    &taskHandle1, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );

  xTaskCreatePinnedToCore( 
    task2,
    "Task_2",
    1024, // stack size in bytes (words in FreeRTOS)
    NULL, // parameter to pass to function
    2, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    &taskHandle2, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i=0; i<5; i++){
    vTaskSuspend(taskHandle2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(taskHandle2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  if (taskHandle1 != NULL) {
    vTaskDelete(taskHandle1);
    taskHandle1 = NULL;
  }
}
