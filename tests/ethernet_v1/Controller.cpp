#include "Arduino.h"
#include "Controller.h"
#include <EthernetClient.h>
#include <Ethernet2.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>

Controller::Controller()
{
     
    _baseUrl = "Not_set_yet";
    _jsonData = "";
    _parameter = "";
}

void Controller::setIp(IPAddress ip)
{
    _ip = ip;
    _baseUrl = "http://"+(String)_ip[0]+"."+(String)_ip[1]+"."+(String)_ip[2]+"."+(String)_ip[3];
}

String Controller::getUrl()
{
    return _url;
}

String Controller::getBaseUrl()
{
    return "http://"+(String)_ip[0]+"."+(String)_ip[1]+"."+(String)_ip[2]+"."+(String)_ip[3];
}

bool Controller::handleRequest(EthernetClient client)
{
    //EthernetClient client = _server->available();
    
    if(!client)
        return false;
        
    _client = client;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String currentLine = "";
    while (_client.connected()) {
      if (_client.available()) {
        char c = _client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http Controller has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // end parsing header
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          if(hasText("Referer:",currentLine))
          {
              int baseUrlLenght = _baseUrl.length();
              int pos = findText(_baseUrl,currentLine);
              _url = currentLine.substring(pos+baseUrlLenght);
          }
          currentLineIsBlank = true;
          currentLine = "";
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          currentLine.concat((String)c);
        }
      }
    }
    
    return true;
}

void Controller::addJsonData(String key,String data)
{
    if(_jsonData != "")
    {
        _jsonData = _jsonData + ",";
    }
    _jsonData = _jsonData +  "{\""+ key + "\":\"" + data  +"\"}";
}

void Controller::sendJsonResponse()
{
    _client.println("HTTP/1.1 200 OK");
    _client.println("Content-Type: application/json");
    _client.println("Connection: close");  // the connection will be closed after completion of the response
    //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
    _client.println();

    _client.println("{\"data\": ["+ _jsonData +"]}");

    _jsonData = "";//reset data
  
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    _client.stop();
}

int Controller::findText(String needle, String haystack) {
  int foundpos = -1;
  for (int i = 0; i <= haystack.length() - needle.length(); i++) {
    if (haystack.substring(i,needle.length()+i) == needle) {
      foundpos = i;
      return foundpos;
    }
  }
  return foundpos;
}

bool Controller::hasText(String needle, String haystack)
{
    if(findText(needle,haystack) == -1)
        return false;
    return true;
}

/**
 * Check if referer url match with pattern
 * + extract parameter
 */
bool Controller::matchUrl(String pattern)
{
    String url = "";
    _parameter = "";
    url = this->getUrl();
    if(hasText("{",pattern))
    {  
        int pos = findText("{",pattern);
        _parameter = url.substring(pos);
        url = url.substring(0,pos-1);
    }
    if(hasText(url,pattern))
    {
        return true;
    }
    return false;
}


String Controller::getParameter()
{
    return _parameter;
}

