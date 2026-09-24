#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>


#define ciclosTimeoutRegar  15
#define ciclosTimeoutLluvia 10


// Constantes de tiempo en MS
//-------------------------------
#define min10 600000
#define seg10 10000
//-------------------------------


// Pin de sensores "Final de carrera" para el motor DC
//-------------------------------
#define sensorHallCerrado 14
#define sensorHallAbierto 13
//-------------------------------


// Pin de sensores "Principales"
//-------------------------------
#define sensorHumedad 7
#define sensorLluvia 4
#define sensorMovimiento 5
//-------------------------------


// Pin de actuadores
//-------------------------------
#define pinBuzzer 15
#define bombaDeAgua 6


// Pines para el motor DC
//-------------------------------
#define motorDCAbrir 16
#define motorDCCerrar 17
//-------------------------------


//Constantes de humedad para el sensor de humedad
//-------------------------------
#define estaSeco 2800
#define estaHumedo 1200
//-------------------------------


// En la estructura SensoresEvent el atributo tipo usa la siguiente nomenclatura
//-------------------------------
#define tipoSensorMovimiento 0
#define tipoSensorHumedad 1
#define tipoSensorLluvia 2
//-------------------------------
#define SSID "SSID"
#define PASSWORD "PASSWORD"




struct tm tiempoReal;


struct SensorEvent {
  uint8_t tipo;
  bool estado;
  struct tm time;
};


QueueHandle_t colaEventos;




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
  // Serial.begin(115200);


  colaEventos = xQueueCreate(
    5,
    sizeof(SensorEvent)
  );


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




void tareaRegar(void *pvParameters) {
  volatile static uint8_t estadoRegar = 0;


  uint8_t contadorTimeoutRegar = 0; //bandera de ciclos antes de entrar en modod reposo


  SensorEvent evento;


  while (true) {
    switch (estadoRegar) {
      case 0:
        if (analogRead(sensorHumedad) > estaSeco) { // Verifica si el suelo esta seco, si es asi activa la bomba de agua


          digitalWrite(bombaDeAgua, HIGH);
          // Serial.println("Se activo la bomba de agua");
/*
          Preparar estructura para cargarlo en el Queue
          false es "esta seco"
*/
          evento.tipo = tipoSensorHumedad;
          evento.estado = false;
          getLocalTime(&evento.time);
          xQueueSend(colaEventos, &evento, 0);


          estadoRegar = 1; // Cambia de Estado
        }
        else { // Si el suelo no esta seco, espera 10 minutos para volver a verificar
          // Serial.println("El suelo no esta seco");
          vTaskDelay(pdMS_TO_TICKS(min10));
        }
        break;


      case 1:
        vTaskDelay(pdMS_TO_TICKS(seg10)); // Riega durante 10 segundos y luego verifica si el suelo esta humedo, si es asi apaga la bomba de agua y vuelve al estado 0, sino espera 10 segundos mas y vuelve a verificar hasta que se cumpla el timeout


        if (analogRead(sensorHumedad) < estaHumedo) { // Verifica si el suelo esta humedo, si es asi apaga la bomba de agua y vuelve al estado 0


          digitalWrite(bombaDeAgua, LOW);
           // Serial.println("Se desactivo la bomba de agua");


          /*
          Preparar estructura para cargarlo en el Queue
          true es "esta humedo"
          Manda un evento por cada cambio de estado del sensor de humedad, ya sea que se active o se desactive la bomba de agua
          */
       //----------------------------------------------
          evento.tipo = tipoSensorHumedad;
          evento.estado = true;
          getLocalTime(&evento.time);
          xQueueSend(colaEventos, &evento, 0);
       //----------------------------------------------


          estadoRegar = 0;
          contadorTimeoutRegar++; //Aumento de ciclo del contador
        }
        else if (contadorTimeoutRegar == ciclosTimeoutRegar) { //Si supera la cantidad ciclos pasa al siguiente estado
          estadoRegar = 2; //Siguiente estado
        }


        break;


      case 2:
        vTaskSuspend(NULL); //Se mantiene en esta tarea hasta que termine (NO HACE NADA)
        break;
    }
    break;
  }
}




void detectarLluvia(void *pvParameters) {


  uint8_t contadorDeCiclosLluvia = 0; //Bandera de ciclos para cuando llueva


  uint8_t estadoLluvia = 0;


  bool calibracionCompleta = false;


  while (!calibracionCompleta) {
    if (digitalRead(sensorHallCerrado) == LOW) {
      digitalWrite(motorDCCerrar, HIGH);
    }
    else {
      digitalWrite(motorDCCerrar, LOW);
      calibracionCompleta = true;
    }


    vTaskDelay(pdMS_TO_TICKS(50));
  }


  while (1) {
    switch (estadoLluvia) {


      case 0:
        if (analogRead(sensorLluvia) < 500) {
          digitalWrite(motorDCAbrir, HIGH);
          estadoLluvia = 1;


          contadorDeCiclosLluvia++;


          if (ciclosTimeoutLluvia == 10) {
            estadoLluvia = 4;
          }
        }


        break;


      case 1:
        if (digitalRead(sensorHallAbierto) == HIGH) {
          digitalWrite(motorDCAbrir, LOW);
          estadoLluvia = 2;
          contadorDeCiclosLluvia++;


          if (contadorDeCiclosLluvia == 10) {
            estadoLluvia = 4;
          }
        }


        break;


      case 2:
        if (analogRead(sensorLluvia) > 2000) {
          digitalWrite(motorDCCerrar, HIGH);
          estadoLluvia = 3;


          contadorDeCiclosLluvia++;


          if (contadorDeCiclosLluvia == 10) {
            estadoLluvia = 4;
          }
        }


        break;


      case 3:
        if (digitalRead(sensorHallCerrado) == HIGH) {
          digitalWrite(motorDCCerrar, LOW);
          estadoLluvia = 0;


          contadorDeCiclosLluvia++;


          if (contadorDeCiclosLluvia == 10) {
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




void detectarMovimiento(void *pvParameters) {
  uint8_t estadoMovimiento = 0;


  while (1) {
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




void estadoWifiYNTP(void *pvParameters) {
  while (1) {
    WiFi.begin(SSID, PASSWORD);
    vTaskDelay(pdMS_TO_TICKS(30000));


    if (WiFi.status() == WL_CONNECTED) {
      configTime(-10800, 0, "pool.ntp.org");


      while (!getLocalTime(&tiempoReal)) {
        vTaskDelay(pdMS_TO_TICKS(1000));
      }


      while (WiFi.status() == WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(5000));
      }
    }


    WiFi.disconnect();
    vTaskDelay(pdMS_TO_TICKS(300000));
  }
}


void procesarQueueYCrearJSON(void *pvParameters){
  SensorEvent evento; // Objeto que recibira los datos del queue
  bool llegoUnEvento = false; // Variable de control para cuando llega un evento
  while(1){ // Repetir siempre
    if(xQueueReceive(colaEventos, &evento, portMAX_DELAY) == pdTRUE){ // Si llega un elemento al Queue
      llegoUnEvento = true;


      JsonDocument doc; // Creo donde estara todo el arreglo de JSON, Crea un objeto de la clase JsonCocument
      JsonArray eventos = doc.to<JsonArray>(); // Adentro de doc habran 'eventos' que seran JSON independientes
      while(uxQueueMessagesWaiting(colaEventos) != 0 || llegoUnEvento){ // Mistras aun haya cola en el queue
        JsonObject jsonEvento = eventos.add<JsonObject>(); // Agrego a un 'evento' lineas de informacion que sera 'jsonEvento'

        // Crear Variable con el estandar ISO 8601 o el formato timestamptz
        //---------------------------------------------------
        char horaDelEvento[25];
        strftime(horaDelEvento, sizeof(horaDelEvento),"%Y-%m-%dT%H:%M:%S", &evento.time);
        //---------------------------------------------------
        // Crear datos del JSON 'jsonEvento'
        //---------------------------------------------------
        jsonEvento["tipo"] = evento.tipo;
        jsonEvento["estado"] = evento.estado;
        jsonEvento["time"] = horaDelEvento;
        //---------------------------------------------------
        if(llegoUnEvento){ // Ejecutar una vez y esperar 5 segundos a que hayan eventos
          vTaskDelay(pdMS_TO_TICKS(5000));
          llegoUnEvento = false;
        }
        xQueueReceive(colaEventos, &evento, 0); // Actualizar el objeto 'evento'


        // Agregar funcion de HTTPS
      }
      String json;
      serializeJson(doc, json);
       //Enviar HTTPS
    }
  }
}


