#include <Arduino.h>
#include <WiFi.h>

// Ciclos de verificacion de estado
//------------------------------
#define ciclosTimeoutRegar  15
#define ciclosTimeoutLluvia 10
//------------------------------

// tiempos en millis
//--------------------
#define min10 600000
#define seg10 10000
//--------------------

// Actuadores "Sensores Hall"
//-------------------------------
#define sensorHallCerrado 14
#define sensorHallAbierto 13
//-------------------------------

// Sensores "Principales"
//-------------------------------
#define sensorHumedad 7
#define sensorLluvia 4
#define sensorMovimiento 5
//-------------------------------

// Actuadores Continuos
//-------------------------------
#define pinBuzzer 15
#define bombaDeAgua 6
//-------------------------------

// Motores (pines)
//-------------------------------
#define motorDCAbrir 16
#define motorDCCerrar 17
//-------------------------------

// Valores de variables de Sensores Principales
//-------------------------------
#define estaSeco 2800
#define estaHumedo 1200
//-------------------------------


// Valores de variables Wi-fi
#define SSID "SSID"
#define PASSWORD "PASSWORD"


/*
#define sensorHumedad 2900
#define sensoLLuvia ¿?
*/




struct tm tiempoReal;



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
 


}



void tareaRegar(void *pvParameters){
 volatile static uint8_t estadoRegar = 0;
 
 uint8_t contadorTimeoutRegar = 0 ;                       //bandera de ciclos
/*
 uint8_t banderaRegar = min10 ;                           //bandera de tiempo de funcion regar  
 uint8_t sensorHumedad = 1200 ; 
 */

 /*
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
         contadorTimeoutRegar++;                                   
         }          
          else if(contadorTimeoutRegar >= ciclosTimeoutRegar){       
            estadoRegar = 2;                                       
          }
         
     break;

   case 2:
      vTaskSuspend(NULL);
  }
 }
 */
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
         contadorTimeoutRegar++;                                   //aumento de ciclo
         }          
          if(contadorTimeoutRegar >= ciclosTimeoutRegar){     //si supera la cantidad ciclos pasa al siguiente estado  
            estadoRegar = 2;                                       //siguiente estado 
          }
         
     break;

     case 2: 
      vTaskSuspend(NULL);                                          //se mantiene en esta tarea hasta que termine (NO HACE NADA)

     break;
   }
 }
}


void detectarLluvia(void *pvParameters){

 /*
 uint8_t contadorDeCiclosLluvia = 10 ;
 uint8_t sensorHallCerrado = LOW
 uint8_t sensorLluvia = 500 ;
 bool calibracionCompleta = true ; 
 */
 uint8_t contadorDeCiclosLluvia = 0 ;       //bandera de ciclos para cuando halla lluvia lluvia 

 uint8_t estadoLluvia = 0;


/*
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

     contadorDeCiclosLluvia++;
        if(ciclosTimeoutLluvia >= 10){
         estadoLluvia = 2;
     }
   }
     break;

     case 1:
      if(digitalRead(sensorHallAbierto) == HIGH){
        digitalWrite(motorDCAbrir, LOW);
        estadoLluvia = 2;
        contadorDeCiclosLluvia++;
          if(contadorDeCiclosLluvia >= 10){
          estadoLluvia = 4;
     }
    }
     break;

     case 2:
        vTaskSuspend(NULL);
 }
}
*/


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

     contadorDeCiclosLluvia++;
     if(ciclosTimeoutLluvia >= 10){
      estadoLluvia = 4;
     }
   }



   break;
   case 1:
   if(digitalRead(sensorHallAbierto) == HIGH){
     digitalWrite(motorDCAbrir, LOW);
     estadoLluvia = 2;
     contadorDeCiclosLluvia++;
     if(contadorDeCiclosLluvia >= 10){
      estadoLluvia = 4;
     }

    }


   }
   break;
   case 2:
   if(analogRead(sensorLluvia) > 2000){
     digitalWrite(motorDCCerrar, HIGH);
     estadoLluvia = 3;

     contadorDeCiclosLluvia++;
     if(contadorDeCiclosLluvia >= 10){
      estadoLluvia = 4;
     }
   }



   break;
   case 3:
   if(digitalRead(sensorHallCerrado) == HIGH){
     digitalWrite(motorDCCerrar, LOW);
     estadoLluvia = 0;

     contadorDeCiclosLluvia++;
     if(contadorDeCiclosLluvia >= 10){
      estadoLluvia = 4;
     }
   }

   break;

   case 4:
   vTaskSuspend(NULL);
   //Serial.printl("Reivisar el Proceso en cuestion") se queda pausado el codigo esperando reaunadar luego de una repeticion continua
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


void estadoWifiYNTP(void *pvParameters){
 while(1){
 WiFi.begin(SSID, PASSWORD);
 vTaskDelay(pdMS_TO_TICKS(30000));


 if(WiFi.status() == WL_CONNECTED){
   configTime(-10800, 0, "pool.ntp.org");
   while (!getLocalTime(&tiempoReal)) {
     vTaskDelay(pdMS_TO_TICKS(1000));
   }


   while(WiFi.status() == WL_CONNECTED){
     vTaskDelay(pdMS_TO_TICKS(5000));
   }
 }
   WiFi.disconnect();
   vTaskDelay(pdMS_TO_TICKS(300000));
 }
}

