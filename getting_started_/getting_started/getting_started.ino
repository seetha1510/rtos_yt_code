#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// pins
static const int led_pin = 2;

void toggleLED(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin, OUTPUT);
  xTaskCreatePinnedToCore( // xTaskCreate() in FreeRTOS - works in esp_idf but it means it will run it in 
  //whatever core it wants
    toggleLED,
    "Toggle_LED",
    1024, // stack size in bytes (words in FreeRTOS)
    NULL, // parameter to pass to function
    1, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    NULL, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );

  // In arduino framework for esp32 - setup/loop exist in a separate task from the main program entry point
  // because of this, vTaskStartScheduler() has already been called
  // so xTaskCreate/PinnedToCore spawns a new task
  
  // if this was vanilla RTOS you want to call vTaskStartScheduler() in the main after setting up tasks 
  // for new tasks to start running
}

void loop() {
  // put your main code here, to run repeatedly:

}
