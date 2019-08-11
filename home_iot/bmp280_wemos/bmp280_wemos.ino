#include <Wire.h>
#include <SPI.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP280.h"
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#define SENSOR_READ_PERIODE 3000

//#define SERIAL_DEBUG

//#define MODULE_A
#define MODULE_B
//#define MODULE_C

#ifdef MODULE_A
  #define FIELD_1 1
  #define FIELD_2 2
#endif

#ifdef MODULE_B
  #define FIELD_1 3
  #define FIELD_2 4
#endif

#ifdef MODULE_C
  #define FIELD_1 5
  #define FIELD_2 6
#endif

/*
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
*/

/* ALL IoT REALEATED */
char* wifi_ssid     = "N&N@Riex";
char* wifi_password = "664R-FNRR-CTSY-JANA";
/* can be found on ThingSpeak */
const char * APIKey = "0COOI047AQ2MZMC8";
unsigned long ThingspeakChannel = 614816;
/* 600'000 every 10 minutes */
unsigned long send_data_periode = 300000L;
unsigned long lastThingspeakSending = 0;


Adafruit_BMP280 bme; // I2C
int inByte = 0; 
float pressure = 0.0f;
float temperature = 0.0f;
const int sleepSeconds = 5*60;

void setup() {

  
  #ifdef SERIAL_DEBUG
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  #endif

  if (!bme.begin()) {  
    #ifdef SERIAL_DEBUG
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    #endif
    while (1);
  }

  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  #ifdef SERIAL_DEBUG
  Serial.println("connected to wifi");
  #endif

  delay(50); 

    pressure = bme.readPressure();
    temperature = bme.readTemperature();
    sendToThingSpeak();

    #ifdef SERIAL_DEBUG
    Serial.print(pressure);
    Serial.println(" Pa");
    Serial.print(temperature);
    Serial.println(" C");
    Serial.println("start sleep");
    #endif
    ESP.deepSleep(sleepSeconds * 1000000);
}
  
void loop() {

    
}


void sendToThingSpeak()
{
    //restart client to get a valid IP => solve issues of dynamic IP...
    WiFiClient client;
    ThingSpeak.begin(client);
    delay(50);

    ThingSpeak.setField(FIELD_1,pressure);
    ThingSpeak.setField(FIELD_2,temperature);
    ThingSpeak.writeFields(ThingspeakChannel, APIKey);
    lastThingspeakSending = millis();
}
