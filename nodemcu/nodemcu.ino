//==========send web===========

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>
#include <Wire.h>
 
const char *ssid = "dann";  
const char *password = "danu12345";
 
//Web/Server address to read/write from 
const char *host = "http://192.168.43.60:8000/api/v1/data";   //your IP/web server address


String id_user;
String id_car;
String btn_empty;
String btn_filled;
String btn_loading;
String btn_trash;
String latitude;
String longitude;
String xgyro;
String ygyro;
String speeds;
String temp;
String cycle;
//=========================
int counting = 0;
int lock = 0;

#include <SoftwareSerial.h>
SoftwareSerial s(D6,D5);
String data;


int starts = 0;
int checksum = 0;
int bypass = 0;

String data_decode ="";
String content = "";
String content_check = "";


void setup() {
  
  s.begin(9600);
  Serial.begin(115200);
            
//  =======website=====    
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

//If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  
//  Wire.begin(4);
//  Wire.onReceive(receiveEvent);
  

}
 
void loop() {
  
  
   if (s.available()) {
    while (s.available()) {
      
      char data = (char)s.read() ;
//      Serial.print(data);//
      if(data == '|'){
        checksum = 1;
      }
      //start
      if(data=='*')
        starts =1;
       //end
      else if(data =='#'){
//        Serial.println(content);
//        Serial.println("check");
//        Serial.println(content_check);
        
        if(content != content_check){
          content = "";
          content_check = "";
        }
        data_decode = content;
        starts =0;
        checksum = 0;
        bypass =0;

        Serial.println(data_decode);
        
        parsing(data_decode);
        Serial.print("lt= ");
        if(runEvery(10000))
          send();
          
        Serial.println(latitude);
        clears();
        counting = 0;
        
        
        content = "";
        content_check = "";
        break;
        
      }
        
      else if(starts == 1){
        if(checksum == 0 ){
          content += data;
        }
        else{
          if( bypass == 0)
             bypass =1;
          else
            content_check += data; 
          
        }
      }
    }
  }
  
  
}


void parsing(String container){
  
  for (int i=0; i<container.length(); i++){
//    Serial.print("key");
//    Serial.println(container[i]);
    if (container[i] == ','){
      counting +=1;
      lock = 1;
//      Serial.println("+++++++++++++++++");
    }
    
    if(lock == 0){
//      Serial.print("counter");
//      Serial.println(counting);
      
      if(counting ==0){
//        Serial.println(">>>>>>>>>>>>>>>>>>>>");
        id_user += container[i];
      }
      
      else if(counting ==1){
//        Serial.println(">>>>>>>>>>>>>>>>>>>>");
        id_car += container[i];
      }
      
      else if(counting ==2){
        btn_empty += container[i];
      }
      else if(counting ==3){
        btn_filled += container[i];
      }
      else if(counting ==4){
        btn_loading += container[i];
      }
      else if(counting ==5){
        
        btn_trash += container[i];
      }
      else if(counting ==6){
//        Serial.println(container[i]);
        latitude += container[i];
      }
      else if(counting ==7){
        longitude += container[i];
      }
      else if(counting ==8){
        xgyro += container[i];
      }
      else if(counting ==9){
        ygyro += container[i];
      }
      else if(counting ==10){
        speeds += container[i];
      }
      else if(counting ==11){
        temp += container[i];
      }
      else if(counting ==12){
        cycle += container[i];
      }
    }
    if(lock == 1 ){
      lock =0;
    }
  }
}

void clears(){
  id_user = "";
  id_car = "";
  btn_empty ="";
  btn_filled = "";
  btn_loading = "";
  btn_trash = "";
  latitude = "";
  longitude = "";
  xgyro = "";
  ygyro = "";
  speeds = "";
  temp = "";
  cycle = "";
    
}

void send(){
//      Serial.println(container);
//    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(host);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String postData =
        "id_user=" + String(id_user)
        + "&id_car=" + String(id_car) 
        + "&btn_empty=" +String(btn_empty)
        + "&btn_filled=" +String(btn_filled)
        + "&btn_loading=" +String(btn_loading)
        + "&btn_trash=" +String(btn_trash)
        + "&latitude=" +String(latitude)
        + "&longitude=" +String(longitude)
        + "&xgyro=" +String(xgyro)
        + "&ygyro=" +String(ygyro)
        + "&speed=" +String(speeds)
        + "&temp=" +String(temp)
        + "&cycle=" +String(cycle);    
      int httpResponseCode = http.POST(postData);
      
       String payload = http.getString();//
 
      Serial.println(httpResponseCode);////
      Serial.println(payload);//
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
//    }
//    else {
//      Serial.println("WiFi Disconnected");
//    }
  
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

