#include <Wire.h>
#include <ESP8266WiFi.h>
#include "SHTSensor.h"
#include <ESP8266HTTPClient.h>

// note: Do not power with apple charger, deep sleep not working

//#define SERIAL_DEBUG

/* ALL IoT REALEATED */
String sensor_id = "sht_module_C";
char* wifi_ssid     = "N&N@Riex";
char* wifi_password = "664R-FNRR-CTSY-JANA";

unsigned long sleepSeconds = 5*60;

SHTSensor sht;
float humidity = 0.0f;
float temperature = 0.0f;

void setup() {
  Wire.begin();
  delay(1000);

  bool init_res = sht.init();

  #ifdef SERIAL_DEBUG
  Serial.begin(115200);
  delay(1000); // let serial console settle
  if (init_res) {
      Serial.print("init(): success\n");
  } else {
      Serial.print("init(): failed\n");
  }
  #endif

  // only supported by SHT3x
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); 

  read_sensor();
  sendData();

  /* only possible if sleep pin is connected */
  ESP.deepSleep(sleepSeconds * 1000000);
}

void sendData()
{
    //restart client to get a valid IP => solve issues of dynamic IP...
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    
   WiFiClient client;
   HTTPClient http;
 
   http.begin(client,"http://10.0.0.15:8888");
   http.addHeader("Content-Type", "application/json");
  
   String data = "{\"sensor_id\":\"" + sensor_id + "\",\"temperature\":"+ String(temperature) + ",\"humidity\":"+ String(humidity) + "}";
   int httpCode = http.POST(data);
   String payload = http.getString(); //Get the response payload

   #ifdef SERIAL_DEBUG
   Serial.print("POST:");
   Serial.println(data);
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
   #endif
   http.end();  //Close connection
}

void read_sensor()
{
  if (sht.readSample())
    {
        humidity = sht.getHumidity();
        temperature = sht.getTemperature();
        #ifdef SERIAL_DEBUG
        Serial.print("H: ");
        Serial.print(humidity, 2);
        Serial.print(" T:  ");
        Serial.println(temperature, 2);
        #endif
    }
    else
    {
       #ifdef SERIAL_DEBUG
       Serial.print("Error in readSample()\n");
       #endif
    }
}

void loop() {
  /*
  read_sensor();
  sendData();
  delay(10000);
  */
  }
