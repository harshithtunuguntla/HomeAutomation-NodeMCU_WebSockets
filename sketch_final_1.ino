#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h> 
#include <ArduinoJson.h>
#include<Servo.h>

Servo servo;
  

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701


StaticJsonDocument<200> doc;

ESP8266WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

#include "sketch_final_1.h"

char* ssid = "TVDS";
char* password = "HARSHITH13";


Ticker timer;

#define bulb1_pin D1
#define bulb2_pin D2
#define bulb3_pin D3
#define bulb4_pin D4

#define fan1_pin D5

#define sensor2_pin D7
#define sensor3_pin D8



void setup()
{

  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);

  pinMode(D7,INPUT);
  pinMode(D8,INPUT);

  

  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);  
  });
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  timer.attach(0.5,send_sensor);

  servo.attach(10);
  servo.write(0);

}

void loop()
{
  webSocket.loop();
  server.handleClient();

  
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if(type == WStype_TEXT){

    String message = String((char*)( payload));
    Serial.println(message);
    DeserializationError error = deserializeJson(doc, message);

      if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
    }



    int bulb1_status = doc["Bulb1"];
    int bulb2_status = doc["Bulb2"];
    int bulb3_status = doc["Bulb3"];
    int bulb4_status = doc["Bulb4"];
    int fan1_status = doc["Fan1"];
    int servo_rotate = doc["servo"];

//    Serial.println(bulb1_status);
//    Serial.println(bulb2_status);
//    Serial.println(bulb3_status);
//    Serial.println(bulb4_status);
//    Serial.println(fan1_status);


    digitalWrite(bulb1_pin, bulb1_status);
    digitalWrite(bulb2_pin, bulb2_status);
    digitalWrite(bulb3_pin, bulb3_status);
    digitalWrite(bulb4_pin, bulb4_status);
    digitalWrite(fan1_pin, fan1_status);

    servo.write(servo_rotate);
      


  }
  
}

void send_sensor()
{
//   int reading = analogRead(temp_hum_sens_pin);
//   Serial.print(reading);
//   float voltage = reading * 5.0;
//   voltage /= 1024.0; 
//   float temperature_value = (voltage - 0.5) * 100 ;

  int sensor2_value = digitalRead(D7);
  int sensor3_value = digitalRead(D8);



  


  String JSON_Data = "{\"temperature\":";
         JSON_Data += 38;
         JSON_Data += ",";
         JSON_Data += "\"humidity\":";
         JSON_Data += 30;
         JSON_Data += ",";
         JSON_Data += "\"sensor2pin\":";
         JSON_Data += sensor2_value;
         JSON_Data += ",";
         JSON_Data += "\"sensor3pin\":";
         JSON_Data += sensor3_value;
         JSON_Data += "}";
  // Serial.println(JSON_Data);     
//  Serial.println(temperature_value);
  webSocket.broadcastTXT(JSON_Data);
}
