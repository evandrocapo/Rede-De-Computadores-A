#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0x90,0xA2,0XDA,0X00,0X29,0X02};

byte ip[] = {172, 16, 1, 143};

EthernetClient localClient;

int Celsius;
int Luminosidade;
int Umidade;
int signal;

void setup(){
  Serial.begin(9600);
  Ethernet.begin(mac);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop(){
  
  Serial.println("Connecting...");
  if(localClient.connect(ip, 5000)){
    Serial.println("Connection Successfull");
    while(1){
      catchTemperature();
      Serial.print("Temperatura: ");
      Serial.println(Celsius);
      Luminosidade = analogRead(4) / 10;
      Serial.print("Luminosidade: ");
      Serial.println(Luminosidade*10);
      Umidade = analogRead(3)/ 100;
      Serial.print("Umidade: ");
      Serial.println(Umidade*100);
      
      //Temperatura
      localClient.write(Celsius);
      while(localClient.available() <= 0){
         
      }
        
      signal = localClient.read();
        
      while(localClient.available() > 0){
          localClient.read();
      }
        
      Serial.println(signal);
      if(signal == 0){
        digitalWrite(8, HIGH);
        digitalWrite(12, LOW);
        digitalWrite(5, LOW);
      } else if(signal == 1){
        digitalWrite(8, LOW);
        digitalWrite(12, LOW);
        digitalWrite(5, HIGH);
      } else{
        digitalWrite(8, LOW);
        digitalWrite(12, HIGH);
        digitalWrite(5, LOW);
      }
      
      
      // Luminosidade
      localClient.write(Luminosidade);
      while(localClient.available() <= 0){
         
      }
        
      signal = localClient.read();
        
      while(localClient.available() > 0){
          localClient.read();
      }
        
      Serial.println(signal);
      if(signal == 0 || signal == 1){
        digitalWrite(7, LOW);
      } else{
        digitalWrite(7, HIGH);
      }
      
      // Umidade
      localClient.write(Umidade);
      while(localClient.available() <= 0){
         
      }
        
      signal = localClient.read();
        
      while(localClient.available() > 0){
          localClient.read();
      }
      Serial.println(signal);
      
      if(signal == 0){
        digitalWrite(6, LOW);
      }else{
        digitalWrite(6, HIGH);
      }
      
      
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
