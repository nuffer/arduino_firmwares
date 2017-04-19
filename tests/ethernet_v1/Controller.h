#ifndef Controller_h
#define Controller_h

#include "Arduino.h"
#include <EthernetClient.h>
#include <Ethernet2.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>

class Controller
{
  public:
    Controller();
    bool handleRequest(EthernetClient client);
    void sendJsonResponse();
    void setIp(IPAddress ip);
    String getUrl();
    String getBaseUrl();
    void addJsonData(String key,String data);
    String getParameter();
    bool matchUrl(String pattern);
    
  private:
    //EthernetServer* _server;
    EthernetClient _client;
    IPAddress _ip;
    String _url;
    String _baseUrl;
    String _jsonData;
    bool hasText(String needle, String haystack);
    int findText(String needle, String haystack);
    String _parameter;
    
};


#endif
