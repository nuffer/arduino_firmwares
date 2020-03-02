#include <Wire.h>
 #include <ESP8266WiFi.h>
#include "SHTSensor.h"
#include <ESP8266HTTPClient.h>

//#define SERIAL_DEBUG // need to be define otherwise there is a bug...

//#define MODULE_A
//#define MODULE_B
//#define MODULE_C
#define MODULE_D
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
#define STATUS_OK 0
#define STATUS_NOT_USED -2
#define STATUS_WIFI_ERROR -1
int connection_status = STATUS_NOT_USED;

/* 600'000 every 10 minutes */
//unsigned long send_data_periode = 60000L;

const unsigned long deepSleepMin = 5;   
unsigned long deeoSleepMicroSecs = 60 * deepSleepMin * 1000000; //microseconds 

SHTSensor sht;
float humidity = 0.0f;
float temperature = 0.0f;
bool no_sht_init_error;
bool no_sht_read_error;

void setup() {

  /* init i2c */
  Wire.begin();
  delay(1000);

  /* initialize sensors */
  no_sht_init_error = sht.init();
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH); // only supported by SHT3x

  /* read sensor */
  no_sht_read_error = sht.readSample();
  if (no_sht_read_error)
  {
      humidity = sht.getHumidity();
      temperature = sht.getTemperature();
      connection_status = sendToNetwork();
  }
      
  
  #ifdef SERIAL_DEBUG
    Serial.begin(9600);
    delay(1000); // let serial console settle
    
    Serial.println("");
    if (no_sht_init_error) {
        Serial.println("SHT init(): success");
    } else {
        Serial.println("SHT init(): failed");
    }
    
    if (no_sht_read_error) {
        Serial.print("SHT read(): H: ");
        Serial.print(humidity, 2);
        Serial.print(" T: ");
        Serial.println(temperature, 2);
    } else {
        Serial.println("SHT read(): failed");
    }

    Serial.print("Network status: ");
    Serial.println(connection_status);
  #endif

  /* only possible if sleep pin is connected */
  ESP.deepSleep(deeoSleepMicroSecs);
}

int sendToNetwork()
{
  int status_code = STATUS_OK;
  
  //restart client to get a valid IP => solve issues of dynamic IP...
  WiFi.begin(wifi_ssid, wifi_password);

  /* connection to wifi */
  int connection_tentative = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    connection_tentative++;
    if(connection_tentative > 10){
      status_code = STATUS_WIFI_ERROR;
      break;
    }
  }

  /* exit in case of error */
  if(status_code != STATUS_OK){
    return status_code;
  }

  /* initialize request */
  WiFiClient client;
  HTTPClient http;
  
  http.begin("http://api.beebotte.com/v1/data/write/mlchls_sensors");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Auth-Token", "token_JeoOfS6oVwSIPa6D");
  
  String data = "{\"records\":[{\"resource\":\"" + tempertature_resource + "\",\"data\":"+ String(temperature) + 
                "},{\"resource\":\""+ humidity_resource +"\",\"data\":" + String(humidity) + "}]}";
  
  int httpCode = http.POST(data);
  String payload = http.getString(); //Get the response payload
  
  http.end();  //Close connection
  return httpCode;
}

void loop() {/* nothing because of deep sleep */}
