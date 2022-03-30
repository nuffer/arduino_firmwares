#include <Wire.h>
#include <EEPROM.h>
#include "SHTSensor.h"

#define PIN_FAN 6
#define PIN_HUMIDIFIER 7

//EEPROM address to start reading from
int eeAddressHigh = 0; 
int eeAddressLow = 4; 

SHTSensor sht;
boolean activate = false;
float humidity = 0.0f;
float temperature = 0.0f;

float humidity_low = 95.0f;
float humidity_high = 100.0f;


void setup() {

  Wire.begin();
  delay(1000);
  
  Serial.begin(115200);
  delay(1000);
  
  //get the data from memory
  EEPROM.get(eeAddressHigh,humidity_high);
  EEPROM.get(eeAddressLow,humidity_low);

  Serial.print("Loaded HIGH humidity from memory:");
  Serial.println(humidity_high);

  Serial.print("Loaded LOW humidity from memory:");
  Serial.println(humidity_low);

  if (sht.init()) {
      Serial.print("init(): success\n");
      // only supported by SHT3x
      sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH); 
  } else {
      Serial.print("init(): failed\n");
  }

  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_HUMIDIFIER, OUTPUT);

  // Unactivate fan and humidifier
  digitalWrite(PIN_FAN, HIGH);
  digitalWrite(PIN_HUMIDIFIER, HIGH);

}


void read_sensor()
{
  if (sht.readSample())
    {
        humidity = sht.getHumidity();
        temperature = sht.getTemperature();
        Serial.print("H: ");
        Serial.print(humidity, 2);
        Serial.print(",T:");
        Serial.print(temperature, 2);
        Serial.print(",A:");
        Serial.println(activate);
    }
    else
    {
        Serial.print("Error in readSample()\n");
    }
}

void loop() {

   if (Serial.available() > 0) {
      String data = Serial.readString();
      /* close serial connection */
      if( data.substring(0,1) == "H" )
      {          
        humidity_high = data.substring(1).toFloat();
        EEPROM.put(eeAddressHigh,humidity_high);
        Serial.print("Set HIGH value of humidity at: ");   
        Serial.println(humidity_high);
      }
      if( data.substring(0,1) == "L" )
      {          
        humidity_high = data.substring(1).toFloat();
        EEPROM.put(eeAddressLow,humidity_high);
        Serial.print("Set LOW value of humidity at: ");   
        Serial.println(humidity_high);
      }

    }

  
  read_sensor();

  if(humidity < humidity_low){
    digitalWrite(PIN_FAN, LOW);
    digitalWrite(PIN_HUMIDIFIER, LOW);
    activate = true;
  }

  if(humidity > humidity_high){
    digitalWrite(PIN_FAN, HIGH);
    digitalWrite(PIN_HUMIDIFIER, HIGH);
    activate = false;
  }

  delay(1000);
 }
