#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>


// Set the PORT for the web server
ESP8266WebServer server(80);

// The WiFi details 
const char* ssid = "ESSE";
const char* password =  "QsxthnIk,65rfv"; 

// Initialise the switch pin
// The variable to store the switch value
const int switch_pin = D1;
int switch_value;

// Initialise the potentiometer pin
// The value coming from the potentiometer
const int potentiometer_pin  = A0;
int poteValue;

// Initialise the minimum and maximum temperature of the fridge
int minTemp = -2;
int maxTemp = 6;

// Initialise the critical minimum and maximum temperature of the fridge
int criticalMinTemp = 2;
int criticalMaxTemp = 4;


// Initialise the RGB pins
const int blue_led_pin = D2;

// Initialise the Temperature and Humidity pin
const int temp_hum_pin = D6;

// Initialise variables to store the temperature and humidity values
int temperature = 0;
int humidity = 0;

// Initialise the DHT11 component
DHT dht(temp_hum_pin, DHT11);

String hostname = "esp32nodetemperature";


// put your setup code here, to run once:
void setup() {
  WiFi.mode(WIFI_STA);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str());

  //Connect to the WiFi network
  WiFi.begin(ssid, password);  
  
  // Set the switch and potentiometer pins to INPUT
  // you want to read the state here
  pinMode(switch_pin, INPUT);
  pinMode(potentiometer_pin, INPUT);


   // LEDs as OUTPUT
  pinMode(blue_led_pin, OUTPUT);
  

  // Start the serial to debug the values
  Serial.begin(9600);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.println("Waiting to connect...");
  }

  //Print the board IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  

  server.on("/", get_index); // Get the index page on root route 
  
  server.begin(); //Start the server
  Serial.println("Server listening");

  // Start the dht component reading
  dht.begin();

}

// put your main code here, to run repeatedly:
void loop() {

  // This will keep the server and serial monitor available 
  Serial.println("-----Server is running-----");

  //Handling of incoming client requests
  server.handleClient(); 

  // Only execute if the fridge is ON
  if (fridgeOn()){

    // Print the fridge temperature on the serial monitor
    Serial.println(fridgeTemperature());
    
    // Signal temperature status
    temperatureStatus(fridgeTemperature(), fridgeOn());
    
    // Read the temperature and humidity
    readTempHum();
    
  }else{
    // Signal temperature status (OFF)
    temperatureStatus(fridgeTemperature(),fridgeOn());
    // No sound, fridge is off
  }
}

// Utility function to check whether the fridge is ON
bool fridgeOn(){
  
  // read the switch pin value
  switch_value = digitalRead(switch_pin);
  //Serial.println(switch_value);
  
  // check the status and return either true or false
  if(switch_value == 0){
    return false;
  }
  return true;
  
}

// Utility function to set the fridge temperature
int fridgeTemperature(){
  
  // Read the value of the potentiometer (0--1023)
  poteValue = analogRead(potentiometer_pin); 

  // Map the potentiometer value in a range of minTemp - maxTemp
  poteValue = map(poteValue, 0, 1023, minTemp, maxTemp);

  return poteValue;
}


// Utility function to sinal the temperature status
void temperatureStatus(int fridgeTemp, bool fridgeOn){
   
   // Turn off the RGB led (inverse mapping)
   if(!fridgeOn){
     analogWrite(blue_led_pin, 255);
     return;
   }

   // Turn on the red value of the RGB led (inverse mapping)
   if (fridgeTemp >= 0){
     int red_value = map(fridgeTemp,0,maxTemp,255,0); // mapping the red value 0 - maxTemp to 255 - 0
     analogWrite(blue_led_pin, 255);
   }else{
     int blue_value =map(fridgeTemp,minTemp,0,0,255);  // mapping the blue value minTemp - 0 to 0 - 255
     analogWrite(blue_led_pin,blue_value );
   }
    
}


void get_index() {

  String html ="<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>The Smart Fridge Dashboard</h1>";
  html +="<p> Welcome to the smart fridge dashboard</p>";
  html += "<div> <p> <strong> The temperature preference is: ";
  html += fridgeTemperature();
  html +="</strong> degrees. </p>";
  html += "<div> <p> <strong> The temperature reading is: ";
  html += temperature;
  html +="</strong> degrees. </p>";
  html += "<div> <p> <strong> The humidity reading is: ";
  html += humidity;
  html +="</strong> % </p>";
  html += "<p> <strong> Buzzer Component ";
  html +="</strong> </p> </div>";
  
  html += "<a href=\"/setBuzzerStatus?s=0\" target=\"_blank\"\"\"><button>Turn Off </button></a>";
  html += "<a href=\"/setBuzzerStatus?s=1\" target=\"_blank\"\"\"><button>Turn On </button></a>";
  
  html +="</body> </html>";
  
  //Print a welcoming message on the index page
  server.send(200, "text/html", html);
  
}


// Read the temperature and humidity values
void readTempHum(){
  
  temperature = dht.readTemperature(0);
  humidity = dht.readHumidity();
  Serial.println(temperature);
  Serial.println(humidity);
  
}
