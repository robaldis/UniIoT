#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "TinyDHT.h"

// Set the port for the web server
ESP8266WebServer server(80);

const int switch_pin = D1;
int switch_value;

const int led_pin = D2;

const char* ssid = "ESSE";
const char* password = "QsxthnIk,65rfv";


// Temperature 
const int temp_pin = D6;

int temp = 0;
int hum = 0;

// Init DHT11, define what one we are using, DHT11 or DHT22
DHT dht(temp_pin, DHT11);

const int potentiometer_pin = A0;
int pote_value;

int min_temp = -2;
int max_temp = 6;

void setup() {
  Serial.begin(9600); 
  // Connect to wifi 
  WiFi.begin(ssid, password);
  dht.begin();

  // put your setup code here, to run once:
  pinMode(switch_pin, INPUT);
  pinMode(potentiometer_pin, INPUT);
  pinMode(led_pin, OUTPUT);


  // Wait for connection
  /**
  while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Watiing to connect...");
      delay(500);
  }
  */


  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());

  server.on("/", get_index);
  server.begin();
  Serial.println("Listening on port 80");


}


void loop() {
    readTempHum();

  //Serial.println("Server listening");
  server.handleClient();

  // put your main code here, to run repeatedly:
  if (fridgeOn()) {
      // Serial.println(fridgeTemp());
      delay(500);
      readTempHum();
  } else {
      // Brightness based on potentiometer
    pote_value = analogRead(potentiometer_pin);
    int b = map(pote_value, 0, 1023, 0, 253);
    analogWrite(led_pin, b);

  }

}

void get_index() {
    String html = "<!DOCTYPE html> <html> ";
    html += "<head><meta http-equiv=\"refresh\" content = \"2\"></head>";
    html += "<body>";
    html += "<p>";
    pote_value = analogRead(potentiometer_pin);
    pote_value = map(pote_value, 0, 1023, min_temp, max_temp);
    html += pote_value;
    html += "</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);

}

int fridgeTemp() {
    pote_value = analogRead(potentiometer_pin);
    pote_value = map(pote_value, 0, 1023, min_temp, max_temp);
    return pote_value;
}

bool fridgeOn() {
    switch_value = digitalRead(switch_pin);
    if (switch_value == 0) {
        return false;
    }
    return true;
}

void readTempHum() {
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    Serial.print("temp: ");
    Serial.print(temp);
    Serial.print(" hum: ");
    Serial.print(hum);
    Serial.println(" ");

}
