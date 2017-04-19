#include <Ethernet2.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
#include <SPI.h>
#include "Controller.h"


/*
 * to access by terminal :  curl http://192.168.1.177 --referer http://192.168.1.177/data
 *                          curl http://192.168.1.177 --referer http://192.168.1.177/temperature/33.4
 */

//defined on the sticker 
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0xF1, 0x63
};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);


float temperature = 20.0;

Controller webController;

void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.println("enter setup");

  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  */
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  delay(500);
  server.begin();
  webController.setIp(Ethernet.localIP());
  Serial.println(Ethernet.localIP());
  Serial.print("Base URL: ");
  Serial.println(webController.getBaseUrl());

  //webController.setAction(ACTION_GET_DATA,"/data");
  //webController.setAction(ACTION_SET_TEMPERATURE,"/temperature/{1}");
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if(webController.handleRequest(client)){
    Serial.println("client detected");
    //webController.handleRequest(client)
    
    //webController.handleRequest(client);

    Serial.print("URL:");
    Serial.println(webController.getUrl());

    if( webController.matchUrl("/data"))
    {
        Serial.println("match /data");
        webController.addJsonData("Tsensor1",String(analogRead(0)));
        webController.addJsonData("Tsensor1",String(analogRead(1)));
        webController.addJsonData("T",String(analogRead(2)));
        webController.addJsonData("temperature",String(temperature));
        webController.addJsonData("FAN",String(HIGH));
        webController.addJsonData("Status","heating");
    }
    if( webController.matchUrl("/temperature/{temperature}"))
    {
        Serial.println("match /temperature/{t}");
        temperature = webController.getParameter().toFloat();
        webController.addJsonData("temperature",(String)temperature);
        //Serial.println(webController.getParameter());
    }
      webController.sendJsonResponse();
      
      Serial.println("send response");
      
  }
  delay(100);

}

