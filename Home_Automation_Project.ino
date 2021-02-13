#include <Wire.h>
#include <MAX44009.h>
#include <dht11.h>  
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
MAX44009 light;
#define FIREBASE_HOST "Enter firebase host address"
#define FIREBASE_AUTH "Enter Firebase authentication ID"
#define WIFI_SSID "***"
#define WIFI_PASSWORD "*****"


 dht11 DHT11;
const int Light1 = D0;
const int Socket1 = D6;
const int Socket2 = D7;
const int PIRpin = D8;
const int WifiIndicator = D3;


String HomePath = "HomeData";
String path = HomePath + "/States";

  void setupPins(){
    pinMode(Light1, OUTPUT);
    pinMode(Socket1, OUTPUT);
    pinMode(Socket2, OUTPUT);
    pinMode(PIRpin,INPUT);
    pinMode(WifiIndicator,OUTPUT);
    }

    void setupWifi(){
      WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
      Serial.println("Hey I 'm connecting...");
      while(WiFi.status()!= WL_CONNECTED ){
        Serial.println(".");
        digitalWrite(WifiIndicator,HIGH);
        delay(500);
        }
        Serial.println();
        Serial.println("I am connected and my IP address is ");
        Serial.println(WiFi.localIP()); 
        digitalWrite(WifiIndicator,LOW);
    }

      void setupFirebase()
      {
        Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
        if(Firebase.failed()){
          Serial.print("setting/number failed:");
          Serial.println(Firebase.error());
          return;
          } 
      }


void setup() {
  // put your setup code here, to run once:
light.begin();
Serial.begin(9600);
digitalWrite(WifiIndicator,LOW);
setupWifi();
setupFirebase();
DHT11.attach(D5);
Serial.println("DHT11 TEST PROGRAM ");  
Serial.print("LIBRARY VERSION: ");  
Serial.println(DHT11LIB_VERSION);

}

  void Switching()
  {
    
    
   FirebaseObject object = Firebase.get(path);
  if(Firebase.failed()){
    Serial.print("Failed to access path");
    digitalWrite(WifiIndicator,HIGH);
    delay(100);
    digitalWrite(WifiIndicator,LOW);
    delay(100);
    return;
    }
  
   String LightState = Firebase.getString("HomeData/States/LightState");
   String Socket1State = Firebase.getString("HomeData/States/Socket1State");
   String Socket2State = Firebase.getString("HomeData/States/Socket2State");
  Serial.println(LightState);
  Serial.println(Socket1State);
  Serial.println(Socket2State);
  digitalWrite(WifiIndicator,LOW);
  if(LightState == "true"){
  digitalWrite(Light1,HIGH);
  Firebase.setInt("HomeData/States/LightStateConfirm",1);
  }
  else{
  digitalWrite(Light1,LOW);
  Firebase.setInt("HomeData/States/LightStateConfirm",0);
  }
   if(Socket1State == "true"){
  digitalWrite(Socket1,HIGH);
  Firebase.setInt("HomeData/States/Socket1StateConfirm",1);
   }
  else{
  digitalWrite(Socket1,LOW);
  Firebase.setInt("HomeData/States/Socket1StateConfirm",0);
  }
   if(Socket2State == "true"){
  digitalWrite(Socket2,HIGH);
Firebase.setInt("HomeData/States/Socket2StateConfirm",1);
   }
  else{
  digitalWrite(Socket2,LOW);
  Firebase.setInt("HomeData/States/Socket2StateConfirm",0);
  }
  }

    float LightReading()
    {
      float x = light.get_lux();
      return x;
    }

void loop() {
  // put your main code here, to run repeatedly:
  setupPins();
  int x = digitalRead(PIRpin);
  Serial.println(x);
Switching();
float lightLevel = LightReading();
int tempValue = 0;
int humValue = 0;
Serial.println("\n"); 
int chk = DHT11.read();  
  switch (chk)  
  {  
   case 0:  
   Serial.print("Humidity (%): "); 
   humValue = (DHT11.humidity);
   tempValue = (DHT11.temperature);  
   break;  
   case -1: Serial.println("Checksum error"); break;  
   case -2: Serial.println("Time out error"); break;  
   default: Serial.println("Unknown error"); break;  
  }  
Switching();
Firebase.setFloat("HomeData/States/Light Intensity", lightLevel);
Switching();
Firebase.setInt("HomeData/States/Temperature Value",tempValue);
Switching();
Firebase.setInt("HomeData/States/Humidity Value", humValue);
Firebase.setInt("HomeData/States/Human presence",x);
Serial.print("Light Level : ");
Serial.println(lightLevel);
Serial.print("Temperature value: ");
Serial.println(tempValue);
Serial.print("Humidity value: ");
Serial.println(humValue);
Switching();
delay(100);
}


