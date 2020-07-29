/*
 Version 0.4 - April 26 2019
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> 
#include <StreamString.h>
#include <Servo.h>


int pos =0;
Servo myservo;

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "479dfbc8-73d4-4d1e-85cd-390038f47b5d" 
#define MySSID "Vijay" 
#define MyWifiPassword "9966413858" 
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;
String prevValue="";

void dispense()
{
  //Serial.println("Backward");
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  //Serial.println("Forward");
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from NCR Hackathon!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to NCR Hackathon at url: %s\n", payload);
      Serial.printf("Waiting for commands from NCR Hackathon ...\n");        
      }
      break;
    case WStype_TEXT: {
        
        String str = (char*)payload;
        if(str != prevValue)
        {
          Serial.printf("Dispensing");
          Serial.printf("[WSc] get text: %s\n", payload);
          prevValue= str;
          dispense();
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
      default:
      Serial.printf("[WSc] Case else: %u\n", length);
  }
}

void setup() {
  Serial.begin(115200);
  myservo.attach(2);
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("104.215.158.8", 8082, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      webSocket.sendTXT("H");
      delay(2000);
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night.
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
}
