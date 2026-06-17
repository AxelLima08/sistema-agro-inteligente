#include <Arduino.h>
#include <WiFi.h>


#define sensorHallFinal 14
#define sensorHallInicio 13
#define motorDCsentidoHorario 16 //Cambiar segun el pin que se use para controlar el motor DC en sentido horario
#define motorDCsentidoAntiHorario 17 //Cambiar segun el pin que se use para controlar el motor DC en sentido antihorario
bool calibracionCompleta = false;
const char* SSID = "TuSSID"; // Cambia esto por el nombre de tu red WiFi
const char* PASSWORD = "TuPassword"; // Cambia esto por la contraseña de tu red WiFi
bool wifiYntp = false; // Variable para controlar si se ha iniciado WiFi y NTP correctamente
struct tm tiempoReal;


void actualizarTiempoReal(){
  if(wifiYntp){
    if(getLocalTime(&tiempoReal)){
      // El tiempo se ha actualizado correctamente, puedes usar la variable tiempoReal para lo que necesites
    }
  }
}


void calibrarMotorDC(){
  if(calibracionCompleta) return;  // Ya calibrado, salir. A esto se le llama Early Return, es una buena práctica para evitar anidar código innecesariamente y mejorar la legibilidad del código.


 if (!digitalRead(sensorHallFinal)) {
    digitalWrite(motorDCsentidoAntiHorario, HIGH);
} else {
    digitalWrite(motorDCsentidoAntiHorario, LOW);
    calibracionCompleta = true;
}
}


void iniciarWiFiYNTP(){
  if(wifiYntp) return; // Ya iniciado, salir
 static bool wifiIniciado = false;
 if(!wifiIniciado){
   WiFi.begin(SSID, PASSWORD);
   wifiIniciado = true;
 }
  if(WiFi.status() == WL_CONNECTED){
    configTime(-10800, 0, "pool.ntp.org");
    wifiYntp = true;
    actualizarTiempoReal();
  }
  else {
  // Añadir un retraso de 1 segundo
  }
}


void setup() {
  pinMode(sensorHallFinal, INPUT);
  pinMode(sensorHallInicio, INPUT);
  pinMode(motorDCsentidoHorario, OUTPUT);
  pinMode(motorDCsentidoAntiHorario, OUTPUT);
}


void loop() {
  calibrarMotorDC();
  iniciarWiFiYNTP();
}





