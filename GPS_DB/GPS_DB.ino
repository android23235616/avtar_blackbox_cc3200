#include<WiFi.h>
#include<TinyGPS++.h>

#define LED RED_LED;

char* ssid = "Pranshoo";
char* password = "password";
char* host = "android23235616.pythonanywhere.com";
int httpPort = 80;

// The TinyGPS++ object
TinyGPSPlus gps;

void setup() {
  // Initialize serial monitor:
  Serial.begin(9600);

  pinMode(RED_LED, OUTPUT);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi AP:
  WiFi.begin(ssid, password);

  // Check for successful connection:
  while (WiFi.status() != WL_CONNECTED){
      digitalWrite(RED_LED, HIGH);
      Serial.print(".");
      delay(500);
      digitalWrite(RED_LED, LOW);
    }

  // Show IP after successful connection:
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(RED_LED, HIGH);

  // Initialize GPS UART:
  Serial1.begin(9600);
  Serial.println();
  Serial.println("GPS Initialized.");
  
}

void loop() {
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Creating WiFi client for TCP connection:
  WiFiClient client;

  // Check for a proper connection to the host:
  if(!client.connect(host, httpPort)){
      Serial.println("Connection error");
      
      return;
    }

  
  
  
  // Get data from GPS:
  while (Serial1.available() > 0){
    char latitude[10];
    char longitude[10];
    int gpsHrI = 0;
    int gpsMinI = 0;
    int gpsSecI = 0;
    String gpsDate = "";
    String gpsTime = "";
    String gpsDay = "";
    String gpsMonth = "";
    String gpsYear = "";
    String gpsHrs = "";
    String gpsMin = "";
    String gpsSec = "";
   if (gps.encode(Serial1.read())){
    
      if(gps.location.isValid() && gps.date.isValid() && gps.time.isValid()){
            dtostrf(gps.location.lat(), 2, 6, latitude);
            dtostrf(gps.location.lng(), 2, 6, longitude);
            Serial.print("Location: ");
            Serial.print(latitude);
            Serial.print(", ");
            Serial.print(longitude);
            Serial.print(" | ");

            if(gps.date.day()<10)
              gpsDay = gpsDay + "0" + gps.date.day();
            else
              gpsDay += gps.date.day();
            if(gps.date.month()<10)
              gpsMonth = gpsMonth + "0" + gps.date.month();
            else
              gpsMonth += gps.date.month();
            if(gps.date.year()<10)
              gpsYear = gpsYear + "0" + gps.date.year();
            else
              gpsYear += gps.date.year();
          
            gpsDate = gpsDate + gpsDay+"-"+ gpsMonth+"-"+ gpsYear;
            Serial.print("Date: ");
            Serial.print(gpsDate);
            Serial.print(" | ");

            gpsMinI = gps.time.minute() + 30;
            if(gpsMinI > 60){
              gpsMinI = gpsMinI - 60;
              gpsHrI = 1;
              }
            
            gpsHrI += gps.time.hour() + 5;

            if(gpsHrI > 24){
                gpsHrI = gpsHrI - 24;
              }
              
            if(gpsHrI<10){
              gpsHrs = gpsHrs + "0" + gpsHrI;
            }
            else
              gpsHrs += gpsHrI;

            if(gpsMinI<10)
              gpsMin = gpsMin + "0" + gpsMinI;            
            else
              gpsMin += gpsMinI;
              
            if(gps.time.second()<10)
              gpsSec = gpsSec + "0" + gps.time.second();
            else
              gpsSec += gps.time.second();
          
            gpsTime = gpsTime + gpsHrs+":"+gpsMin+":"+ gpsSec;
            Serial.print("Time: ");
            Serial.print(gpsTime);
            Serial.println("");

            // Send data to the database:
            String url = "/vehicle_location?chesis=12345";
            
            // Attach time with url:
            //url += "?gpsTime=";
            //url += gpsTime;
            
            // Attach date with url:
            //url += "&gpsDate=";
            //url += gpsDate;

            // Attach latitde with url:
            url += "&lat=";
            url += latitude;

            // Attach longitude with url:
            url += "&lng=";
            url += longitude;
            
            Serial.print("Requesting URL: ");
            Serial.println(host+url);
          
            // Send request to the server:
            client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" + 
                               "Connection: close\r\n\r\n");
            unsigned long currentTime = millis();
            while (client.available() == 0){
              if (millis() - currentTime > 1000){
                  Serial.println("--------------------------------------------------------------------------------------------------------");
                  Serial.println("Error: Connection timed out!");
                  Serial.println("--------------------------------------------------------------------------------------------------------");
                  Serial.println();
                  client.stop();
                  return;
                }
              
              } 

            Serial.println("--------------------------------------------------------------------------------------------------------");
            Serial.println("Success: Data updated successfully.");
            Serial.println("--------------------------------------------------------------------------------------------------------");
            Serial.println();
        }
        else{
          
              Serial.println("INVALID LOCATION!");
              return;
          }      
      
      }

   }
      
/*
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
*/

  delay(10000);

  
}
