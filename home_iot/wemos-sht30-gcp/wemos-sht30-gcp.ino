#include <Wire.h>
#include <ESP8266WiFi.h>
#include "SHTSensor.h"
#include <ESP8266HTTPClient.h>

#define SERIAL_DEBUG



/* ALL IoT REALEATED */
char* wifi_ssid     = "N&N@Riex";
char* wifi_password = "664R-FNRR-CTSY-JANA";

/* 600'000 every 10 minutes */
unsigned long send_data_periode = 600000L;
unsigned long sleepSeconds = 10*60;

SHTSensor sht;
float humidity = 0.0f;
float temperature = 0.0f;

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  delay(1000);
  
  #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    delay(1000); // let serial console settle
  
    if (sht.init()) {
        Serial.print("init(): success\n");
    } else {
        Serial.print("init(): failed\n");
    }
  #endif

  // only supported by SHT3x
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH); 

  //read_sensor();

  /* only possible if sleep pin is connected */
  //ESP.deepSleep(sleepSeconds * 1000000);
}

/*
void sendToThingSpeak()
{
    //restart client to get a valid IP => solve issues of dynamic IP...
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    WiFiClient client;

   HTTPClient http;
 
   http.begin("http://api.beebotte.com/v1/data/write/home");
   http.addHeader("Content-Type", "application/json");
   http.addHeader("X-Auth-Token", "token_P8NvCi7NauJdWVun");
  
   String data = "{\"records\":[{\"resource\":\"" + tempertature_resource + "\",\"data\":"+ String(temperature) + 
                  "},{\"resource\":\""+ humidity_resource +"\",\"data\":" + String(humidity) + "}]}";
   
   int httpCode = http.POST(data);
   //int httpCode = http.POST("{\"data\":22.5}");//Send the request
   String payload = http.getString();                  //Get the response payload
 
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection
}
*/

void read_sensor()
{
  if (sht.readSample())
    {
        humidity = sht.getHumidity();
        temperature = sht.getTemperature();
        
        #ifdef SERIAL_DEBUG
          Serial.print("SHT:\n");
          Serial.print("  RH: ");
          Serial.print(humidity, 2);
          Serial.print("\n");
          Serial.print("  T:  ");
          Serial.print(temperature, 2);
          Serial.print("\n");
        #endif

        #ifdef SERIAL_DEBUG
          Serial.println("prepare send to data");
        #endif
        
        //sendToThingSpeak();
        
        #ifdef SERIAL_DEBUG
          Serial.println("sent...");
        #endif

        //delay(send_data_periode);

    }
    else
    {
        #ifdef SERIAL_DEBUG
          Serial.print("Error in readSample()\n");
        #endif
    }
}

void loop() {
  read_sensor();
  delay(1000);
 }
