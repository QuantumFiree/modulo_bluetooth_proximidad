#include <Servo.h>
// Instancia objeto Servo para controlar el servomotor
Servo servoMotorA;
// Puertos para el sensor 1
const int triggerPort1 = 3;
const int echoPort1 = 4;
// Puertos para el sensor 2
const int triggerPort2 = 6;
const int echoPort2 = 7;
// Puerto para el servomotor
const int servoPort = 8; //
// Velocidad de transmisino
const int serialBegin = 9600;
const int proxyPulse = 10;
const int delayServAct = 100;
const int initPos = 0;
const int finalPos = 100;
const int velServ = 15;
const int defaultProx = 200;
const int proxDistance = 10;
int currentPos = 0;
int proxAct = 0;
int proxAct1 = 0;
int proxAct2 = 0;
int servAct = 0;
long t1;
long d1;
long t2;
long d2;
int lastProxAct1 = 0;
int lastProxAct2 = 0;
 
void setup() {
  // Inicializa el puerto serial 
  Serial.begin(serialBegin);
  // Inicializa el puerto del servomotor
  servoMotorA.attach(servoPort);
  // Se definen los puertos de entrada y salida para lo sensores de proximidad
  // Se desactiva el puerto que permite el envio del pulso del sensor
  pinMode(triggerPort1, OUTPUT);
  pinMode(echoPort1, INPUT);
  digitalWrite(triggerPort1, LOW);
  pinMode(triggerPort2, OUTPUT);
  pinMode(echoPort2, INPUT);
  digitalWrite(triggerPort2, LOW);
  // Se desactivan todos los servicios
  setAll(0);
}
 
void loop() {
  // El arduino ha sido configurado para establecer una conexion con el puerto COM6
  // Se verifica si hay informacion en el puerto
  if (Serial.available()) {
  // El valor que esta siendo leido llega desde la interfaz a travez de la comunicacion del puerto serial
    switch(Serial.read()){
      
      case 'a':
        // Set proxAct1 true
        setProxAct1(1);
      break;

      case 'b':
        // Set proxAct1 false
        setProxAct1(0);
      break;
      
      case 'c':
        // Set proxAct2 true
        setProxAct2(1);
      break;

      case 'd':
        // Set proxAct2 false
        setProxAct2(0);
      break;

      case 'e':
        // Set servAct true
        setServAct(1);
      break;

      case 'f':
        // Set servAct false
        setServAct(0);
      break;

      case 'g':
        // Open door
        setServAct(0);
        setProxAct(0);
        setProxAct1(0);
        setProxAct2(0);
        openDoor();
      break;

      case 'h':
        // Close Door
        setServAct(0);
        setProxAct(0);
        setProxAct1(0);
        setProxAct2(0);
        closeDoor();
      break;

      case 'm':
        // Set init pos
        setServAct(0);
        setProxAct(0);
        initDoor();
      break;

      case 'n':
        // Show door state
        showDoorState();
      break;

      case 'o':
        // Set proxAct true
        setProxAct(1);
      break;

      case 'p':
        // Set proxAct false
        setProxAct(0);
      break;

      case 'q':
        // Set Prox and Serv true
        setAll(1);
      break;

      case 'r':
        // Set Prox and Serv false
        setAll(0);
      break;
      
    }
    
  }
  
  servProx1();
  servProx2();
  
}

void setAll(int val){
  // Funcon que permite activar todos los servicios(sensor de entrada y salida)
  setProxAct1(val);
  setProxAct2(val);
  setServAct(val);
  setProxAct(val);
  if(val == 1){
    Serial.print("Servicios activados.");
    Serial.println();
  } else {
    Serial.print("Servicios desactivados.");
    Serial.println();
    initDoor();
  }
}

void servProx1(){
  // Si las variables que determinan si el servicio de proximidad esta activo (proxAct)
  // y el servicio del sensor 1 esta activado, comenzara la captura de datos por parte del sensor 1
  if(proxAct1 == 1 && proxAct == 1) {
    detectProx1();
    // Si el la distancia medida por el sensor de entrada es menor al umbral de activacion
    // la puerta del servomotor sera abierta
    if(d1 <= proxDistance){
      lastProxAct2 = 0;
      notifyProx1();
    } else {
      lastProxAct2 = 1;
    }
    if(servAct == 1 && lastProxAct1 == 1){
      if(d1 <= proxDistance) {
        openDoor();
      } else {
        closeDoor();
      }
    }
  }
}

void servProx2(){
  // Si las variables que determinan si el servicio de proximidad esta activo (proxAct)
  // y el servicio del sensor 2 esta activado, comenzara la captura de datos por parte del sensor 2
  if(proxAct2 == 1 && proxAct == 1) {
    detectProx2();
    // Si el la distancia medida por el sensor de salida es menor al umbral de activacion
    // la puerta del servomotor sera abierta
    if(d2 <= proxDistance){
      lastProxAct1 = 0;
      notifyProx2();
    } else {
      lastProxAct1 = 1;
    }
    if(servAct == 1 && lastProxAct2 == 1){
      if(d2 <= proxDistance) {
        openDoor();
      } else {
        closeDoor();
      }
    }
  }
}

void showDoorState(){
  // Verifica el estado de la puerta (cerrada o abierta)
  if(currentPos == initPos){
    Serial.print("Cerrada.");
    Serial.println();
  } else if(currentPos == finalPos){
    Serial.print("Abierta.");
    Serial.println();
  } else {
    Serial.print("Semi-abierta.");
    Serial.println();
  }
}

void setProxAct(int val){
  // Activa el servicio de proximidad
  proxAct = val;
  if(proxAct == 1){
    Serial.print("Servicio proximidad activado.");
    Serial.println();
  } else {
    Serial.print("Servicio proximidad desactivado.");
    Serial.println();
  }
}

void setProxAct1(int val){
  // Activa el sensor 1, sensor de entrada
  proxAct1 = val;
  if(proxAct1 == 1){
    Serial.print("Servicio proximidad entrada activado.");
    Serial.println();
  } else {
    Serial.print("Servicio proximidad entrada desactivado.");
    Serial.println();
  }
}

void setProxAct2(int val){
  // Activda sensor 2, sensor de salida
  proxAct2 = val;
  if(proxAct2 == 1){
    Serial.print("Servicio proximidad salida activado.");
    Serial.println();
  } else {
    Serial.print("Servicio proximidad salida desactivado.");
    Serial.println();
  }
}

void setServAct(int val){
  // Activa el servicio de la puerta del servomotor
  servAct = val;
  if(servAct == 1){
    Serial.print("Servicio puerta activado.");
    Serial.println();
  } else {
    Serial.print("Servicio puerta desactivado.");
    Serial.println();
  }
}

void notifyProx1() {
  // Se imprime un mensaje por consolo indicando la distancia a la que se detecto la proximidad
  // para el sensor de entrada
  Serial.print("Proximidad entrada detectada: ");
  Serial.print(d1);
  Serial.print(" cm.");
  Serial.println();
}

void notifyProx2() {
  // Se imprime un mensaje por consolo indicando la distancia a la que se detecto la proximidad
  // para el sensor de salida
  Serial.print("Proximidad salida detectada: ");
  Serial.print(d2);
  Serial.print(" cm.");
  Serial.println();
}

void detectProx1(){
  // Activa el puerto de salida conectado al sensor 1 de (puerta de entrada)
  // El sensor comenzara a emitir el pulso para identificar objetos
  digitalWrite(triggerPort1, HIGH);
  // Se establece un retrazo para dejar de emitir el pulso y se apaga el puerto de salida
  delayMicroseconds(proxyPulse);
  digitalWrite(triggerPort1, LOW);
  // La funcion pulseIn() nos permite determinar el tiempo de retorno del pulso
  t1 = pulseIn(echoPort1, HIGH);
  // Lo que permite determinar la distancia
  d1 = t1/59;
  delay(delayServAct);
}

void detectProx2(){
  // Activa el puerto de salida conectado al sensor 2 (puerta de salida)
  // El sensor comenzara a emitir el pulso para identificar objetos
  digitalWrite(triggerPort2, HIGH);
  // Se establece un retrazo para dejar de emitir el pulso y se apaga el puerto de salida
  delayMicroseconds(proxyPulse);
  digitalWrite(triggerPort2, LOW);
  // La funcion pulseIn() nos permite determinar el tiempo de retorno del pulso
  t2 = pulseIn(echoPort2, HIGH);
  // Lo que permite determinar la distancia
  d2 = t2/59;
  delay(delayServAct);
}

void openDoor(){
  // Si el angulo actual de la puerta del servomotor es 0, entonces se abrira la puerta
  if(currentPos == 0) {
    Serial.print("Abriendo puerta.");
    Serial.println();
    // Se define un ciclo para el cambio de angulo de la apertura de la puerta
    // Con esto se evita un movimiento brusco en la puerta del servomotor
    for (int i = initPos; i <= finalPos; i++)
    {
      currentPos = i;
      servoMotorA.write(i);
      delay(velServ);
    }
    Serial.print("Puerta abierta.");
    Serial.println();
  }
  
}

void closeDoor(){
  // Si el angulo de la puerta es mayor que cero, entonces sera posible cerrar la puerta.
  if(currentPos > 0) {
    Serial.print("Cerrando puerta.");
    Serial.println();
    // Se define un ciclo para el cambio de angulo de la apertura de la puerta
    // Con esto se evita un movimiento brusco en la puerta del servomotor
    for (int i = finalPos; i >= initPos; i--)
    {
      currentPos = i;
      servoMotorA.write(i);
      delay(velServ);
    }
    Serial.print("Puerta cerrada.");
    Serial.println();
  }
}

void initDoor(){
  // Establece la ubicacion inicial de la puerta
  // la puerta estara ubicada a 0 grados, osea, cerrada
  Serial.print("Ubicando puerta.");
  Serial.println();
  servoMotorA.write(initPos);
  currentPos = initPos;
  delay(velServ);
  Serial.print("Puerta ubicada.");
  Serial.println();
}
