#define sensorHallFinal 14
#define sensorHallInicio 13
#define motorDCsentidoHorario 16 //Cambiar segun el pin que se use para controlar el motor DC en sentido horario
#define motorDCsentidoAntiHorario 17 //Cambiar segun el pin que se use para controlar el motor DC en sentido antihorario
//Se tomara como true para un estado alto del pin y false para un estado bajo del pin
bool calibracionCompleta = false;


void calibrarMotorDC(){
  if(calibracionCompleta) return;  // Ya calibrado, salir. A esto se le llama Esrly Return, es una buena práctica para evitar anidar código innecesariamente y mejorar la legibilidad del código.


  static int8_t estadocalibrarMotorDC = 1;//Static para que estadocalibrarMotorDC no se reasigne a 1 cada vez que se llame a la función calibrarMotorDC, sino que mantenga su valor entre llamadas a la función
  switch(estadocalibrarMotorDC)
  {


  case 1://Estado buscando Final
    if(!digitalRead(sensorHallFinal)){
      estadocalibrarMotorDC = 2;
    }
    else{
        digitalWrite(motorDCsentidoAntiHorario, HIGH);
      }
  break;


  case 2://Esta en el final
    digitalWrite(motorDCsentidoAntiHorario, LOW);
    calibracionCompleta = true;
  break;


  default:
    break;
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
}



