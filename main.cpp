#include <Arduino.h>
#include <WiFi.h>


// Definición de Pines
#define sensorHallFinal 14
#define sensorHallInicio 13
#define motorDCsentidoHorario 16
#define motorDCsentidoAntiHorario 17
#define sensorMovimiento 5
#define pinBuzzer 15
#define bombaDeAgua 6
#define sensorHumedad 7
#define sensorLluvia 4
#define SSID "Mahiro <3"
#define PASSWORD "hola1234"


#define min10 600000
#define estaSeco 3000
#define estaHumedo 1200
#define seg30 30000


volatile bool WiFiIniciadoYNTP = false;
// Variables de Control
volatile bool flagTimer = false;
uint8_t estadoMovimiento = 0;
uint8_t estadoRegar = 0;
uint8_t estadoLluvia = 0;
volatile uint16_t contadorMovimiento = 0;
volatile uint32_t contadorRegar = 0;
volatile uint16_t contadorRegar2 = 0;
volatile uint16_t contadorWiFi = 0;
volatile uint16_t contadorNTP = 0;
bool calibracionCompleta = false;


hw_timer_t *My_timer = NULL;


struct tm tiempoReal;


bool actualizarTiempoReal(){
  return getLocalTime(&tiempoReal);
}


void iniciarWifiYNTP(){
  if(WiFiIniciadoYNTP) return;
  static bool iniciado = true;
  if(iniciado){
    WiFi.begin(SSID,PASSWORD);
    iniciado = false;
  }
  if(contadorWiFi <= 30000){
    contadorWiFi++;
    return;
  }
  if(WiFi.status() == WL_CONNECTED){
    configTime(-10800, 0, "pool.ntp.org");
    if(contadorNTP <= 5000){
      contadorNTP++;
    }
    else{
    actualizarTiempoReal();
    WiFiIniciadoYNTP = true;
    }
  }
  else{
    iniciado = true;
    contadorWiFi = 0;
  }
}


// Rutina de Interrupción (ISR)
void IRAM_ATTR onTimer() {
  flagTimer = 1;
}


void regar(){
    switch(estadoRegar){
        case 0:
          if(contadorRegar > min10){
            if(analogRead(sensorHumedad) > estaSeco) {
              digitalWrite(bombaDeAgua, HIGH);
              estadoRegar = 1;
            }
            else contadorRegar = 0;
          }
          else{
            contadorRegar++;
          }
          break;
        case 1:
          if(contadorRegar2 > 10000){
            if(analogRead(sensorHumedad) < estaHumedo){
              digitalWrite(bombaDeAgua, LOW);
              contadorRegar = 0;
              estadoRegar = 0;
            }
            else contadorRegar2 = 0;
          }
          else{
            contadorRegar2++;
          }
          break;
    }
}


void calibrarMotorDC() {
  if (calibracionCompleta) return;
  if (digitalRead(sensorHallFinal) == LOW) {
    digitalWrite(motorDCsentidoAntiHorario, HIGH);
  } else {
    digitalWrite(motorDCsentidoAntiHorario, LOW);
    calibracionCompleta = true;
  }
}


void detectarMovimiento(){
    switch (estadoMovimiento) {
        case 0:
          if (digitalRead(sensorMovimiento)) {
            estadoMovimiento = 1;
            ledcWrite(pinBuzzer, 128); // Ahora se usa el pin directamente, no el canal
            contadorMovimiento = 0;
          }
          break;
        case 1:
          contadorMovimiento++;
         
          if (contadorMovimiento >= 2000) {
            ledcWrite(pinBuzzer, 0);
            estadoMovimiento = 0;
          }
          break;
    }
}


void detectarLluvia(){
  switch(estadoLluvia){
    case 0:
    if(analogRead(sensorLluvia) < 500){
      digitalWrite(motorDCsentidoHorario, HIGH);
      estadoLluvia = 1;
    }
    break;
    case 1:
    if(digitalRead(sensorHallInicio) == HIGH){
      digitalWrite(motorDCsentidoHorario, LOW);
      estadoLluvia = 2;
    }
    break;
    case 2:
    if(analogRead(sensorLluvia) > 2000){
      digitalWrite(motorDCsentidoAntiHorario, HIGH);
      estadoLluvia = 3;
    }
    break;
    case 3:
    if(digitalRead(sensorHallFinal) == HIGH){
      digitalWrite(motorDCsentidoAntiHorario, LOW);
      estadoLluvia = 0;
    }
    break;
  }
}


void setup() {
  pinMode(sensorHallFinal, INPUT_PULLUP);
  pinMode(sensorHallInicio, INPUT_PULLUP);
  pinMode(motorDCsentidoHorario, OUTPUT);
  pinMode(motorDCsentidoAntiHorario, OUTPUT);
  pinMode(sensorMovimiento, INPUT);
  pinMode(sensorHumedad, INPUT);
  pinMode(bombaDeAgua, OUTPUT);
  pinMode(sensorLluvia, INPUT);


  // ledcAttach(pin, frecuencia, resolución)
  ledcAttach(pinBuzzer, 2000, 8);


  digitalWrite(motorDCsentidoHorario, LOW);
  digitalWrite(motorDCsentidoAntiHorario, LOW);


  My_timer = timerBegin(1000000);
  timerAttachInterrupt(My_timer, &onTimer);
  timerAlarm(My_timer, 1000, true, 0);
}


void loop() {
  if (flagTimer) {
    flagTimer = false;


    calibrarMotorDC();
    if (calibracionCompleta) {
      detectarMovimiento();
      regar();
      detectarLluvia();
    }
  }
}



