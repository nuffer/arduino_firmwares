#include <Wire.h>
#include "SHTSensor.h"

#define PIN_FAN 6
#define PIN_HUMIDIFIER 7


SHTSensor sht;
boolean activate = false;
float humidity = 0.0f;
float temperature = 0.0f;

float humidity_low = 85.0f;
float humidity_high = 90.0f;


void setup() {

  Wire.begin();
  delay(1000);
  
  Serial.begin(115200);
  delay(1000);

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
