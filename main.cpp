#include <Arduino.h>

#define min10 3000
#define seg10 10000
#define bombaDeAgua 6
#define sensorHumedad 7
#define estaSeco 2800
#define estaHumedo 1200
void tareaRegar(void *pvParameters);


void setup() {
  pinMode(sensorHumedad,INPUT);
  pinMode(bombaDeAgua, OUTPUT);
  xTaskCreate(
    tareaRegar,
    "Regar",
    2048,
    NULL,
    1,
    NULL
  );
}


void loop() {
  // put your main code here, to run repeatedly:


}


void tareaRegar(void *pvParameters){
  volatile static uint8_t estadoRegar = 0;
  while(true){
    switch(estadoRegar){
      case 0:
        if(analogRead(sensorHumedad)>estaSeco){
          digitalWrite(bombaDeAgua, HIGH);
          estadoRegar = 1;
        }
        else{
          vTaskDelay(pdMS_TO_TICKS(min10));
        }
      break;


      case 1:
        vTaskDelay(pdMS_TO_TICKS(seg10));
        if(analogRead(sensorHumedad) < estaHumedo){
          digitalWrite(bombaDeAgua, LOW);
          estadoRegar = 0;
        }
      break;


    }
  }
}