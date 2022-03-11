#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_SCD30.h>
#include <ESP8266HTTPClient.h>

// note: Do not power with apple charger, deep sleep not working

//#define SERIAL_DEBUG

/* ALL IoT REALEATED */
String sensor_id = "sdc30_module_1";
char* wifi_ssid     = "N&N@Riex";
char* wifi_password = "664R-FNRR-CTSY-JANA";

unsigned long sleepSeconds = 1*60;

Adafruit_SCD30  scd30;

float temperature = 0.0f;
float humidity = 0.0f;
float co2 = 0.0f;

void readSensor()
{
  Wire.begin();
  delay(1000);

  if (!scd30.begin()) {
    #ifdef SERIAL_DEBUG
    Serial.println("Failed to find SCD30 chip");
    #endif
    return;
  }

  // Set a small interval to get 3 data quickly
  if (!scd30.setMeasurementInterval(2)){
     Serial.println("Failed to set measurement interval");
     return;
  }
  delay(200); // Let time to act
  
  #ifdef SERIAL_DEBUG
  Serial.print("Measurement Interval: "); 
  Serial.print(scd30.getMeasurementInterval()); 
  Serial.println(" seconds");
  #endif

  

  // Read multiple time the sensor in order to get stabilized data
    int counter = 0;

    while(counter < 3){
      // wait new data ready
      while(!scd30.dataReady()) { delay(10); }

      if (!scd30.read()){ 
        #ifdef SERIAL_DEBUG
        Serial.println("Error reading sensor data");
        #endif
        return;
      }

      #ifdef SERIAL_DEBUG
      Serial.print("Temperature: ");
      Serial.print(scd30.temperature);
      Serial.println(" degrees C");
      
      Serial.print("Relative Humidity: ");
      Serial.print(scd30.relative_humidity);
      Serial.println(" %");
      
      Serial.print("CO2: ");
      Serial.print(scd30.CO2, 3);
      Serial.println(" ppm");
      Serial.println("");
      #endif

      counter++; 
    }
    temperature = scd30.temperature;
    humidity = scd30.relative_humidity;
    co2 = scd30.CO2;

    // Set a bit interval to save data
    if (!scd30.setMeasurementInterval(1800)){
       Serial.println("Failed to set measurement interval");
       return;
    }
    delay(200); // Let time to act
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
  
   String data = "{\"sensor_id\":\"" + sensor_id + 
                 "\",\"temperature\":"+ String(temperature) + 
                 ",\"co2\":"+ String(co2) + 
                 ",\"humidity\":"+ String(humidity) + "}";
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

void setup() {

  #ifdef SERIAL_DEBUG
  Serial.begin(115200);
  delay(1000); // let serial console settle
  #endif

  readSensor();
  sendData();
  

  /* only possible if sleep pin is connected */
  ESP.deepSleep(sleepSeconds * 1000000);
}


void loop() {}
