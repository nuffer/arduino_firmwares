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
float H_consigne = 90.00f;
float H_current = 0.00f;
bool serial_open = false;

/*
const int numReadings = 10;
int T_index = 0;
float T_total = 0.0;
float T_average = 0.0;
float T_readings[numReadings];

int H_index = 0;
float H_total = 0.0;
float H_average = 0.0;
float H_readings[numReadings];
*/

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

  
    //get the temperature form sensor
    T_current = thSensor.getTemperature();
    H_current = thSensor.getHumidity();
    //avereage sensor measurement
    //T_average = temperatureAverage(T_current);
    //H_average = humidityAverage(H_current);
    
    checkSerial();
    sendToThingSpeak();
    controlHumidity(H_current);

    
    delay(200);
  
   
}

/*
float temperatureAverage(float T_curr)
{
  T_total = T_total - T_readings[T_index];
  T_readings[T_index] = T_curr;
  T_total = T_total + T_readings[T_index];
  T_index = T_index + 1;
  if (T_index >= numReadings) {
    T_index = 0;
  }
  return T_total/numReadings;
}


float humidityAverage(float H_curr)
{
  H_total = H_total - H_readings[H_index];
  H_readings[H_index] = H_curr;
  H_total = H_total + H_readings[H_index];
  H_index = H_index + 1;
  if (H_index >= numReadings) {
    H_index = 0;
  }
  return H_total/numReadings;
}
*/
void echoInfo()
{
    Serial.print("===============");
    Serial.print("T_current = ");
    Serial.println(T_current);
    //Serial.print("T_average = ");
    //Serial.println(T_average);
    Serial.print("H_consigne = ");
    Serial.println(H_consigne);
    Serial.print("H_current = ");
    Serial.println(H_current); 
    //Serial.print("H_average = ");
    //Serial.println(H_average);       
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

void controlHumidity(float humidity)
{
  // ThingSpeak will only accept updates every 15 seconds.
  unsigned long currentTime = millis();

  // 12'000 every 2 minutes
  unsigned long deltaTime = 12000;
 
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

