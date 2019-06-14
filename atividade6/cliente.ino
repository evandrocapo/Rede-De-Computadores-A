#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0x90,0xA2,0XDA,0X00,0X29,0X02};

byte ip[] = {172, 16, 1, 239};

EthernetClient localClient;

int Celsius;
int signal;

void setup(){
  Serial.begin(9600);
  Ethernet.begin(mac);
  pinMode(9, OUTPUT);
}

void loop(){
  Serial.println("Connecting...");
  if(localClient.connect(ip, 5000)){
    Serial.println("Connection Successfull");
    while(1){
      catchTemperature();
      Serial.print("Temperatura float: ");
      Serial.println(Celsius);
      localClient.write(Celsius);
      while(localClient.available() <= 0){
       
      }
      
      signal = localClient.read();
      
      while(localClient.available() > 0){
        localClient.read();
      }
      
      Serial.println(signal);
      digitalWrite(9, signal);
      delay(2000);
    }
    
  }else {
      Serial.println("Connection failed: ");
  }  
}

void catchTemperature(){
  Celsius = (5.0 * analogRead(2) * 100.0) / 1024.0;
  delay(2000);
}
