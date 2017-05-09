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

const char* host = "data.sparkfun.com";
const char* streamId   = "....................";
const char* privateKey = "....................";


//objects
AM2320 thSensor;

//variables
float T_current = 0.00f;
float T_consigne = 0.00f;
float H_current = 0.00f;
bool serial_open = false;

//EEPROM address to start reading from
int eeAddress = 0; 

//timing
unsigned long lastThingspeakSending = 0;


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):

//EthernetClient client;
unsigned long incubatorChannel = 260851;
const char * myWriteAPIKey = "T22244IV9PPBVYZG";


void setup() {
    //get the data from memory
    //EEPROM.get(eeAddress,T_consigne);
    
    //initialize serial communications at 9600 bps:
    Serial.begin(9600);
    serial_open = true;
    delay(50);
    
         
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }

    
   

    /*
    // start the Ethernet connection:
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
    }

    ThingSpeak.begin(client);
    */
    delay(50);
}

void loop() {

  
    //get the temperature form sensor
    //updateTemperature();
    T_current = thSensor.getTemperature();
    H_current = thSensor.getHumidity();
    checkSerial();

    sendToThingSpeak();

    
    delay(200);
  
   
}


void echoInfo()
{
    Serial.print("T_current = ");
    Serial.println(T_current);
    Serial.print("T_consigne = ");
    Serial.println(T_consigne);
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
          T_consigne = data.toFloat();
          //save in eeprom
          EEPROM.put(eeAddress,T_consigne);      
        }
    } 
}


void sendToThingSpeak()
{
  // ThingSpeak will only accept updates every 15 seconds.
  unsigned long currentTime = millis();

  // 600'000 every 10 minutes
  unsigned long deltaTime = 600000;
 
  if((currentTime - lastThingspeakSending) > deltaTime)
  {
        /*
        WiFi.begin(ssid, password);

        delay(500);
        while (WiFi.status() != WL_CONNECTED) {
          /* exit function if not connected *
          return;
        }
        */
        //restart client to get a valid IP => solve issues of dynamic IP...
        WiFiClient client;
        ThingSpeak.begin(client);
        delay(50);

    
       //Serial.println("send data");
       ThingSpeak.setField(1,T_current);
       ThingSpeak.setField(2,H_current);
       
       ThingSpeak.writeFields(incubatorChannel, myWriteAPIKey); 
       lastThingspeakSending = millis();
       //Serial.println("send data");
  }
}

