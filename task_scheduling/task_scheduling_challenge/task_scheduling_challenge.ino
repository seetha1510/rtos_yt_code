#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = 2;
int delay_count = 1500;

const byte numChars = 40;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

static TaskHandle_t taskHandle1 = NULL;
static TaskHandle_t taskHandle2 = NULL;

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void task1(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(delay_count / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(delay_count / portTICK_PERIOD_MS);
  }  
}

void task2(void *parameter) {
  while(1) {
    Serial.println("in task 2 now");
    recvWithEndMarker();

    if(newData == true) {
      delay_count = atoi(receivedChars);
      char buf[25];
      sprintf(buf, "Updated delay to %d", delay_count);
      Serial.println(buf);
      newData = false;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin, OUTPUT);
  Serial.begin(300);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("Delay program");
  
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
    1, // task priority (0 to configMAX_PRIORITIES - 1 (24)) // 0 is lowest priority
    &taskHandle2, // task handle
    app_cpu // run on core 1 (only esp32, not FREERTOS)    
  );
}

void loop() {
  // put your main code here, to run repeatedly:

}
