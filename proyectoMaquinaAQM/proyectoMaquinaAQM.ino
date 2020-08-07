#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


//
#include "ThingSpeak.h" // Include ThingSpeak Library
#include <ESP8266WiFi.h> // Include ESP8266wifi Library


char ssid[] = "Telecentro-E578"; // Nombre del WiFi (nombre del router)
char pass[] = "08674630"; // WiFi router password
unsigned long myChannelNumber = ; // Thingspeak número de canal
const char * myWriteAPIKey = ""; // ThingSpeak write API Key

int status = WL_IDLE_STATUS;

WiFiClient client;
//



// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
const uint16_t kRecvPin = 14;

IRrecv irrecv(kRecvPin);

decode_results results;

//Variables para controlar el encendido/apagado de los LEDs
//unsigned long TiempoActual = 0;
int periodoApagarLucesControl = 2; //minuto

int BorrarDatosIR=0;
int PasoDelTiempoSegundos = 0;

int periodoGuardadoServidor = 60; //un minuto
//unsigned long TiempoAhora = 0;

//SENSOR COLOCADOR
 const int sensorEstadoColocador=3;
 bool estadoColocador;
 const int ledInfrarrojoColocador=12;
 const int alarmaLuzColocador=15;

bool SopladoDobleEstacion = false;
const int pinledSopladoEstacion=2;

 //TIEMPOS DE ALARMA
 int valorColocador;
 int contadorPosColocador;
 int contadorNegColocador;
 int tiempoAlarmaColocador=40;
 int tiempoAlarmaColocadorUnaEstacion=80;
 bool AlarmaColocador;


 //CONTADORES SERVIDOR
 int contadorParadaServidor;
 int contadorArranqueServidor;
 int contadorColocadorServidor;

// CONTROL REMOTO
int semaforo, arranque, parada, arranqueder, paradader;
const int pinarranque = 5;
const int pinparada= 0;
const int pinsemaforo = 4;
const int pinparadader = 13;
const int pinarranqueder = 1;

void setup(){
    
Serial.begin(115200); // Velocidad puerto serie

WiFi.begin(ssid, pass); // Inicia WiFi
ThingSpeak.begin(client); // Inicia ThingSpeak

Serial.println();
Serial.print("Conectando a ");
Serial.print(ssid);
/*
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}*/

Serial.println();
Serial.println("Conectado a WiFi");
Serial.println("Dirección IP: ");
Serial.println(WiFi.localIP());
//

irrecv.enableIRIn();  // Start the receiver

pinMode(sensorEstadoColocador, INPUT);

pinMode(ledInfrarrojoColocador, OUTPUT);
pinMode(alarmaLuzColocador, OUTPUT);

pinMode(pinsemaforo, OUTPUT);
pinMode(pinparada, OUTPUT);
pinMode(pinarranque, OUTPUT);
pinMode(pinparadader, OUTPUT);
pinMode(pinarranqueder, OUTPUT);

pinMode(pinledSopladoEstacion, OUTPUT);
}

/*
4 amarillo "ok"

5 rojo izq "*"
6 verde izq "<"

7 rojo der "#"
8 verde der ">"
*/


void loop(){
    
    if (irrecv.decode(&results)) {

    switch (results.value)
    {

  
     /* 
      *  AMARILLO
      */
      
      case 0xFF38C7: //Corresponde al boton ok
      case 0x488F3CBB: //Corresponde al boton ok
      if (semaforo == 0)
      {
      digitalWrite(pinsemaforo, HIGH);  //Si estaba apagado lo enciende
      semaforo = 1;
      }
      else
      {


    if (SopladoDobleEstacion == true)
    {
    digitalWrite(pinledSopladoEstacion, LOW);
    SopladoDobleEstacion = false;
    }
    else
    {
    digitalWrite(pinledSopladoEstacion, HIGH);
    SopladoDobleEstacion = true;
    }
  

        
      digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;    
      }
      break;


     /* 
      *  LADO IZQUIERDO
      */
      

      case 0x8C22657B: //Corresponde al boton <
      case 0xFF10EF: //Corresponde al boton <
      if (arranque == 0 and semaforo == 1)
      {
      digitalWrite(pinarranque, HIGH);  //Si estaba apagado lo enciende
      arranque = 1;      
      contadorArranqueServidor = contadorArranqueServidor + 1;
 
      digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;
      
      digitalWrite(pinparada, LOW); //apago el stop
      parada = 0;
      }
      break;

      case 0xFF6897: //Corresponde al boton *
      case 0xC101E57B: //Corresponde al boton *
      if (parada == 0)
      {
      digitalWrite(pinparada, HIGH);  //Si estaba apagado lo enciende
      parada = 1;
contadorParadaServidor =contadorParadaServidor +1;
digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;
      
      digitalWrite(pinarranque, LOW); //apago el stop
      arranque = 0;
      
      }
      else
      {
      digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;    
      }

      break;

      /* 
       *  LADO DERECHO
       */

      case 0xFF5AA5: //Corresponde al boton >
      case 0x449E79F: //Corresponde al boton >

      if (arranqueder == 0 and semaforo == 1)
      {
      digitalWrite(pinarranqueder, HIGH);  //Si estaba apagado lo enciende
      arranqueder = 1;      
 contadorArranqueServidor = contadorArranqueServidor + 1;
      digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;
      
      digitalWrite(pinparadader, LOW); //apago el stop
      paradader = 0;
      }
      break;

      case 0xFFB04F: //Corresponde al boton #
      case 0xF0C41643: //Corresponde al boton #
      if (paradader == 0)
      {
      digitalWrite(pinparadader, HIGH);  //Si estaba apagado lo enciende
      paradader = 1;
  contadorParadaServidor =contadorParadaServidor +1;
      digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;
      
      digitalWrite(pinarranqueder, LOW); //apago el stop
      arranqueder = 0;
      }
      else
      {
      digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;    
      }

      break;
    }   

    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume();    //Prepara el sensor infrarrojos para nuevas lecturas
  }


  
delay(100);   
PasoDelTiempoSegundos = PasoDelTiempoSegundos + 1;
BorrarDatosIR = BorrarDatosIR + 1;
 /*
  * COLOCADOR
  */



estadoColocador = digitalRead(sensorEstadoColocador);
 
 if (estadoColocador==0)
 {
 digitalWrite(ledInfrarrojoColocador,HIGH);
 contadorPosColocador++;
 contadorNegColocador=0;
 }
 else
 {
  digitalWrite(ledInfrarrojoColocador, LOW);
  contadorPosColocador=0;
  contadorNegColocador++;
  }


if (SopladoDobleEstacion == true)
{
if (contadorNegColocador >= tiempoAlarmaColocador or contadorPosColocador >= tiempoAlarmaColocador)
{
digitalWrite(alarmaLuzColocador, HIGH);
if (AlarmaColocador == false)
contadorColocadorServidor = contadorColocadorServidor + 1;
AlarmaColocador = true;
}
else
{
AlarmaColocador = false;
digitalWrite(alarmaLuzColocador, LOW);
}
}

if (SopladoDobleEstacion == false)
{
if (contadorNegColocador >= tiempoAlarmaColocadorUnaEstacion or contadorPosColocador >= tiempoAlarmaColocadorUnaEstacion)
{
digitalWrite(alarmaLuzColocador, HIGH);
if (AlarmaColocador == false)
contadorColocadorServidor = contadorColocadorServidor + 1;
AlarmaColocador = true;
}
else
{
AlarmaColocador = false;
digitalWrite(alarmaLuzColocador, LOW);
}
}

if (BorrarDatosIR == 20)
    {
      digitalWrite(pinarranqueder, LOW);  //Si estaba apagado lo enciende
      arranqueder = 0;      
      digitalWrite(pinparadader, LOW); //apago el stop
      paradader = 0;  
      digitalWrite(pinarranque, LOW);  //Si estaba apagado lo enciende
      arranque = 0;      
      digitalWrite(pinparada, LOW); //apago el stop
      parada = 0;  
      digitalWrite(pinsemaforo, LOW); //Apago el amarillo
      semaforo = 0;
      BorrarDatosIR =0; 
    }
    
if (PasoDelTiempoSegundos == 3000)
{
        ThingSpeak.setField(1, contadorColocadorServidor);
        ThingSpeak.setField(2, contadorParadaServidor);
        ThingSpeak.setField(3, contadorArranqueServidor);                
        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        PasoDelTiempoSegundos = 0;
}

}



   





    
    
