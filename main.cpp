#include <Arduino.h>

#define min10 3000
#define seg10 10000
#define bombaDeAgua 6
#define sensorHumedad 7
#define motorDCAbrir 16
#define motorDCCerrar 17
#define sensorHallCerrado 14
#define sensorHallAbierto 13
#define estaSeco 2800
#define estaHumedo 1200
#define sensorMovimiento 5
#define sensorLluvia 4

void tareaRegar(void *pvParameters);
void detectarLluvia(void *pvParameters);

void setup() {
  pinMode(sensorHallCerrado, INPUT_PULLUP);
  pinMode(sensorHallAbierto, INPUT_PULLUP);
  pinMode(motorDCCerrar, OUTPUT);
  pinMode(motorDCAbrir, OUTPUT);
  pinMode(sensorMovimiento, INPUT);
  pinMode(sensorHumedad, INPUT);
  pinMode(bombaDeAgua, OUTPUT);
  pinMode(sensorLluvia, INPUT);
  xTaskCreate(
    tareaRegar,
    "Regar",
    2048,
    NULL,
    1,
    NULL
  );
  xTaskCreate(
    detectarLluvia,
    "detectarLluvia",
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

void detectarLluvia(void *pvParameters){
  uint8_t estadoLluvia = 0;
  while(1){
  switch(estadoLluvia){
    case 0:
    if(analogRead(sensorLluvia) < 500){
      digitalWrite(motorDCAbrir, HIGH);
      estadoLluvia = 1;
    }
    break;
    case 1:
    if(digitalRead(sensorHallAbierto) == HIGH){
      digitalWrite(motorDCAbrir, LOW);
      estadoLluvia = 2;
    }
    break;
    case 2:
    if(analogRead(sensorLluvia) > 2000){
      digitalWrite(motorDCCerrar, HIGH);
      estadoLluvia = 3;
    }
    break;
    case 3:
    if(digitalRead(sensorHallCerrado) == HIGH){
      digitalWrite(motorDCCerrar, LOW);
      estadoLluvia = 0;
    }
    break;
  }
  vTaskDelay(pdMS_TO_TICKS(10));
}
}