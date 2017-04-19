
#include <EEPROM.h>
#include "AM2320.h"
//Pin mapping
const int PELTIER_1_A = 3;
const int PELTIER_1_B = 2;

//const int PELTIER_2_A = 8;
//const int PELTIER_2_B = 9;

//const int THERMAL_SENSOR_ALIM = 12;

//const int FAN = 12;
const int LED = 13;//built in

//constant
/* peltier status */
const int NO_CURRENT = 0;
const int COOLING = 1;
const int HEATING = 2;

//objects
AM2320 thSensor;


//variables
float T_current = 0;
float T_consigne = 0.00f;
//EEPROM address to start reading from
int eeAddress = 0; 
int peltierStatus = NO_CURRENT;
int regulatorPower = 0;
bool serial_open = false;


void setup() {
    //get the data from memory
    EEPROM.get(eeAddress,T_consigne);
    
    //initialize serial communications at 9600 bps:
    Serial.begin(9600);
  
    //wait 5 sec to check if user input
    /*
    delay(5000);
    if (Serial.available() > 0)
    {
        String data = Serial.readString();
        if( data == "o\n" )
        {          
            serial_open = true;
            Serial.println("Serial: opened"); 
        }
    }
    else
    {
        serial_open = false;
    } 
    */

    serial_open = true;
  
    //setup PIN
    //pinMode(FAN, OUTPUT);
    pinMode(PELTIER_1_A,OUTPUT);
    pinMode(PELTIER_1_B,OUTPUT);
    //pinMode(THERMAL_SENSOR_ALIM,OUTPUT);
    pinMode(LED,OUTPUT);
    
    //set first value on pin
    //digitalWrite(FAN, LOW);

    /* turn off peltier on init */
    digitalWrite(PELTIER_1_A,LOW);
    digitalWrite(PELTIER_1_B,LOW);
    
    //digitalWrite(THERMAL_SENSOR_1_ALIM,LOW);
}

void loop() {

  
    //get the temperature form sensor
    //updateTemperature();
    T_current = thSensor.getTemperature();
        
    //temperature regulatoin
    //regulator();
    regulatorTOR();

    //controle peltier power
    //peltierControl();
    peltierSimpleControl();

    checkSerial();


  
    delay(1000);
    
}

/**
 * regulateur tout ou rien
 */
void regulatorTOR()
{
  float margin = 1.0;
  //positif if heating needed, negatif is cooling needed
  float deltaT = T_consigne - T_current;

  float error = 0;
  
  if(deltaT > 0)
      error = T_consigne + margin - T_current;

  if(deltaT <= 0)
      error = T_consigne - margin - T_current;


  if(error > margin)
  {
    //heating
    regulatorPower = 100;
    return;
  }
     
  if(error < -margin)
  {
    //cooling
    regulatorPower = -100; 
    return;
  }
  //stop power  
  regulatorPower = 0; 
}

/*
void regulatorPID()
{
  float P = 100;
  float I = 0;
  float D = 0;
  
  //positif if heating needed, negatif is cooling needed
  float error = T_consigne - T_current;

  regulatorPower = error*P;

  //maximum power output
  if(regulatorPower > 100)
      regulatorPower = 100;
  if(regulatorPower < -100)
      regulatorPower = -100;
}
*/


/*
 * Get the current temperature form sensors
 *
void updateTemperature()
{
    //turn on thermal sensor
    digitalWrite(THERMAL_SENSOR_ALIM, HIGH);
    //wait voltage stabilization
    delay(50);
    // read the analog in value:
    int sensor = temperatureSensor.getTemperature();
    //turn of thermal sensor
    digitalWrite(THERMAL_SENSOR_1_ALIM, LOW);

    //convert to temperature in ceclius
    float sensor1Deg = sensorToTemperature(sensor1);

    //make mean with all the sensors
    T_current = sensor1Deg;
}
*/


/*
 * convert 0-1024 analogue read in celcuis deg.
 */
float sensorToTemperature(int sensorValue)
{
    return sensorValue*0.4883 - 273.15;
}

void echoInfo()
{
    Serial.print("temperature = ");
    Serial.println(T_current);
    Serial.print("T_consigne = ");
    Serial.println(T_consigne);
    
    if(peltierStatus == COOLING)
        Serial.println("COOLING");
    if(peltierStatus == HEATING)
        Serial.println("HEATING");
    if(peltierStatus == NO_CURRENT)
        Serial.println("NO CURRENT");
        
    Serial.print("Power = ");
    Serial.println(regulatorPower);
      
}


void peltierSimpleControl()
{
  int newPeltierStatus = NO_CURRENT;
  
  if(regulatorPower > 0)
  {
    newPeltierStatus = HEATING;
  }
  if(regulatorPower < 0)
  {
    newPeltierStatus = COOLING;
  }

  /* compar status new and previous */
  if(newPeltierStatus == peltierStatus)
  {
    /* do nothing...same status */ 
    return;
  }
  peltierStatus = newPeltierStatus;
  //act as function of the status
  switch(peltierStatus)
  {
      case HEATING:
          //turn on peltier
          digitalWrite(PELTIER_1_A,HIGH);
          digitalWrite(PELTIER_1_B,LOW);
          break;
      case COOLING:
          //turn on peltier
          digitalWrite(PELTIER_1_A,LOW);
          digitalWrite(PELTIER_1_B,HIGH);
          break;
      case NO_CURRENT:
          digitalWrite(PELTIER_1_A,LOW);
          digitalWrite(PELTIER_1_B,LOW);
          break;
  }
  
}

/*
 * status: HEATING or COOLING
 * power: from 0 to 100
 * 
 * the PWM is base on 10 seconds periode 
 *
void peltierControl()
{
    if(regulatorPower < 0)
    {
        regulatorStatus = COOLING;
        regulatorPower = -regulatorPower;
    }
    else
    {
        regulatorStatus = HEATING;
    }
    int periode = 3000;
    float power = (float)regulatorPower;
    int onDuration = (power/100)*periode;
    int offDuration = periode - onDuration;
    //act as function of the status
    switch(regulatorStatus)
    {
        case HEATING:
            //turn on peltier
            digitalWrite(PELTIER_1_A,HIGH);
            digitalWrite(PELTIER_1_B,LOW);
            delay(onDuration);
            break;
        case COOLING:
            //turn on peltier
            digitalWrite(PELTIER_1_A,LOW);
            digitalWrite(PELTIER_1_B,HIGH);
            delay(onDuration);
            break;
    }
    //turn of peltier
    digitalWrite(PELTIER_1_A,LOW);
    digitalWrite(PELTIER_1_B,LOW);
    delay(offDuration);
}
*/
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
          T_consigne = data.toFloat();
          //save in eeprom
          EEPROM.put(eeAddress,T_consigne);      
        }
    } 
}

