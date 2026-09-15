#include <Arduino.h>

#define min10 600000
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
#define pinBuzzer 15

void tareaRegar(void *pvParameters);
void detectarLluvia(void *pvParameters);
void detectarMovimiento(void *pvParameters);

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
    xTaskCreate(
    detectarMovimiento,
    "detectarMovimiento",
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
  bool calibracionCompleta = false;
  while(!calibracionCompleta){
  if (digitalRead(sensorHallCerrado) == LOW){
  digitalWrite(motorDCCerrar, HIGH);
  }
  else {
    digitalWrite(motorDCCerrar, LOW);
    calibracionCompleta = true;
  }
  vTaskDelay(pdMS_TO_TICKS(50));
  }
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
  vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void detectarMovimiento(void *pvParameters){
  uint8_t estadoMovimiento = 0;
  while(1){
    switch (estadoMovimiento) {
        case 0:
          if (digitalRead(sensorMovimiento)) {
            estadoMovimiento = 1;
            ledcWrite(pinBuzzer, 128); // Ahora se usa el pin directamente, no el canal
          }
          vTaskDelay(pdMS_TO_TICKS(200));
          break;
        case 1:
          vTaskDelay(pdMS_TO_TICKS(2000));
            ledcWrite(pinBuzzer, 0);
            estadoMovimiento = 0;
          break;
    }
  }
}
