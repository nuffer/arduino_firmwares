#include <Wire.h>
#include <ESP8266WiFi.h>
#include "SHTSensor.h"
#include <ESP8266HTTPClient.h>

#define SERIAL_DEBUG

#define MODULE_A
//#define MODULE_B
//#define MODULE_C
//#define MODULE_D
//#define MODULE_E
//#define MODULE_F

/* define all the configurations */
#ifdef MODULE_A
  String tempertature_resource = "A_Temp";
  String humidity_resource = "A_Humi";
#endif

#ifdef MODULE_B
  String tempertature_resource = "B_Temp";
  String humidity_resource = "B_Humi";
#endif

#ifdef MODULE_C
  String tempertature_resource = "C_Temp";
  String humidity_resource = "C_Humi";
#endif

#ifdef MODULE_D
  String tempertature_resource = "D_Temp";
  String humidity_resource = "D_Humi";
#endif

#ifdef MODULE_E
  String tempertature_resource = "E_Temp";
  String humidity_resource = "E_Humi";
#endif

#ifdef MODULE_F
  String tempertature_resource = "F_Temp";
  String humidity_resource = "F_Humi";
#endif



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
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); 

  read_sensor();

  /* only possible if sleep pin is connected */
  ESP.deepSleep(sleepSeconds * 1000000);
}

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
        
        sendToThingSpeak();
        
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

void loop() {}

