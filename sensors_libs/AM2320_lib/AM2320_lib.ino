/*
 * Il y a des erreur dans cette librairie
 * 
 * A comparer avec Adafruit am2315
 */

#include <Wire.h>
#include "AM2320.h"
AM2320 th;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("---------------");
  Serial.print("humidity: ");
  Serial.print(th.getHumidity());
  Serial.print("%");
  Serial.print("|| temperature: ");
  Serial.print(th.getTemperature());
  Serial.println("*C");
  Serial.println("---------------");
  delay(2000);

}
