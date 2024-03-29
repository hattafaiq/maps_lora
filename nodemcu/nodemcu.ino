//==========send web===========

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>
#include <Wire.h>
 
const char *ssid = "RML-MLK";  
const char *password = "riung007";
 
//Web/Server address to read/write from 
const char *host = "http://riung-melak.com/api/v1/data";   //your IP/web server address


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

int xgyro_sum;
int ygyro_sum;
int speed_sum;

int xgyro_rerata;
int ygyro_rerata;
int speed_rerata;
int count_rerata =0;
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

String latitude_prev = "";
String longitude_prev = "";
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
        Serial.println(latitude);

        count_rerata++;
        xgyro_sum+= xgyro.toInt();
        ygyro_sum+= ygyro.toInt();
        speed_sum+= speeds.toInt();

//        Serial.print(longitude);
//        Serial.print("-");
//        Serial.println(longitude_prev);
//        
//        Serial.print(latitude);
//        Serial.print("-");
//         Serial.print(latitude.toFloat());
//        Serial.print("-");
//        Serial.print(latitude_prev.toFloat()+0.02);
//        Serial.print("-");
//        Serial.println(latitude_prev);
        if(latitude_prev != NULL){
          if(latitude.toFloat() > latitude_prev.toFloat()+0.02 || latitude.toFloat() < latitude_prev.toFloat()-0.02){
            Serial.println("replay la");
            latitude = latitude_prev;
          }
          else{
            latitude_prev = latitude;
          }
        }
        else{
          latitude_prev = latitude;
        }
        if(longitude_prev !=NULL){
          if(longitude.toFloat() > longitude_prev.toFloat()+0.02 || longitude.toFloat() < longitude_prev.toFloat()-0.02){
            longitude = longitude_prev;
            Serial.println("replay long");
          }
          else{
            longitude_prev = longitude;
          }
        }
        else{
           longitude_prev = longitude;
        }


        if(runEvery(10000)){
           
          // get rerata
          // for(int iteration=1; iteration <= count_rerata ; iteration++ ){
          //   int sum_xgyro += xgyro_sum[iteration];
          //   int sum_ygyro += ygyro_sum[iteration];
          // }
          xgyro_rerata = xgyro_sum/count_rerata;
          ygyro_rerata = ygyro_sum/count_rerata;
          speed_rerata =  speed_sum/count_rerata;

          send();
          xgyro_rerata = 0;
          ygyro_rerata = 0;
          speed_rerata =0;
          xgyro_sum =0;
          ygyro_sum =0;
          speed_sum = 0;
          count_rerata = 0;

        }
          
        
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
        + "&xgyro=" +String(xgyro_rerata)
        + "&ygyro=" +String(ygyro_rerata)
        + "&speed=" +String(speed_rerata)
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
