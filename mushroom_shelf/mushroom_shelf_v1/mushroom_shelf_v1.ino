/*
 * Firmware for mushroom shelf control
 * 
 * board: Espdunios
 * 
 * Instruction pour flasher la carte:
 * 
 * 1) Appuyer sur le bouton "flash" (garde le bouton enfoncé)
 * 2) Appuyer sur RESET 6second
 * 3) relaché le RESET
 * 4) "téléversé" depuis Arduino IDE 
 * 6) relaché le bouton flashs
 * 
 */
#include <EEPROM.h>
#include "AM2320.h"
#include <SPI.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

const char* ssid     = "N&N@Riex";
const char* password = "664R-FNRR-CTSY-JANA";


//objects
AM2320 thSensor;

//variables
float T_current = 0.00f;
float H_consigne = 95.00f;
float H_current = 0.00f;
bool serial_open = false;

//EEPROM address to start reading from
int eeAddress = 0; 

//timing
unsigned long lastThingspeakSending = 0;
unsigned long lastControllerHumidityTime = 0;

//for ESPDUINO
#define D16 LED_BUILTIN 
//pins
#define D0 2
#define D1 12
#define D2 3
#define D3 11
#define D4 4
#define D5 5
#define HUMIDITY_RELAY LED_BUILTIN



// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):

//EthernetClient client;
unsigned long incubatorChannel = 260851;
const char * myWriteAPIKey = "T22244IV9PPBVYZG";


void setup() {
    //get the data from memory
    //EEPROM.get(eeAddress,H_consigne);
    
    //initialize serial communications at 9600 bps:
    Serial.begin(9600);
    serial_open = true;
    delay(50);
    
         
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    delay(50);

    pinMode(HUMIDITY_RELAY,OUTPUT);
    delay(50);
    digitalWrite(HUMIDITY_RELAY, HIGH);
    delay(50);
}

void loop() {

    //get the data form sensor
    T_current = thSensor.getTemperature();
    H_current = thSensor.getHumidity();
    
    checkSerial();
    sendToThingSpeak();
    controlHumidity(H_current);

    delay(1000);   
}

void echoInfo()
{
    Serial.print("===============");
    Serial.print("T_current = ");
    Serial.println(T_current);
    Serial.print("H_consigne = ");
    Serial.println(H_consigne);
    Serial.print("H_current = ");
    Serial.println(H_current);   
}


/**
 * Manage serial port interaction
 */
void checkSerial() {
    if(serial_open)
    {
      if (Serial.available() > 0) {
          String data = Serial.readString();
          /* close serial connection */
          if( data == "c\n" )
          {          
              serial_open = false;
              Serial.println("Serial: closed");
              Serial.end();   
              return;
          }
          /* ask for echo data */
          if( data == "r\n" )
          {          
              echoInfo();
              return;   
          }
          /* if not the case above, then transform data into new T consigne */
          H_consigne = data.toFloat();
          //save in eeprom
          //EEPROM.put(eeAddress,H_consigne);      
        }
    } 
}


void sendToThingSpeak()
{
  // ThingSpeak will only accept updates every 15 seconds.
  unsigned long currentTime = millis();

  // 600'000 every 10 minutes
  unsigned long deltaTime = 600000;
  // if statement acording to: https://www.baldengineer.com/arduino-how-do-you-reset-millis.html
  if((unsigned long)(currentTime - lastThingspeakSending) >= deltaTime)
  {
        //restart client to get a valid IP => solve issues of dynamic IP...
        WiFiClient client;
        ThingSpeak.begin(client);
        delay(50);

       ThingSpeak.setField(1,T_current);
       ThingSpeak.setField(2,H_current);
       
       ThingSpeak.writeFields(incubatorChannel, myWriteAPIKey); 
       lastThingspeakSending = millis();
  }
}

void controlHumidity(float humidity)
{
  unsigned long currentTime = millis();

  // 60'000 every 1 minutes
  // 120'000 every 2 minutes
  // 300'000 every 5 minutes
  // 600'000 every 10 minutes
  unsigned long deltaTime = 60000;
 
  if((currentTime - lastControllerHumidityTime) > deltaTime)
  {
      /*
      * Apply logic of humidity control
      */
      if(H_consigne > humidity)
      {
          /* then humidify */
          humidification(true);
      }
      else
      {
          /* then stop humidication */
          humidification(false);
      }
      lastControllerHumidityTime = millis();
  }
}

void humidification(bool do_it)
{
  if(do_it)
  {
    digitalWrite(HUMIDITY_RELAY, HIGH);
  }
  else
  {
    digitalWrite(HUMIDITY_RELAY, LOW);
  }
}

