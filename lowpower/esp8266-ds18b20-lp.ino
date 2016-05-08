#include <OneWire.h> // http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
#include <DallasTemperature.h> // http://download.milesburton.com/Arduino/MaximTemperature/DallasTemperature_LATEST.zip
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Base64.h>

 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        !!PLEASE CHANGE THESE!!
String ssid		= "Your_WLAN_SSID";
String password	= "Your_WLAN_password";

String TempVarName = "Your_pimatic_variable";
int SleepTime = 10;  // SleepTime in minutes

String Username     = "your_pimatic_user";
String Password     = "your_pimatic_password";

char authVal[40];  
char authValEncoded[40];

String host   = "192.168.144.128";
const int httpPort    = 80;
// I send to two pimatic servers. Disable the below if you don't need it
String backuphost = "192.168.144.230";
const int backuphttpPort = 9001;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         GLOBAL VARIABLES
float   temperature   = 0.0;
String deviceType   = "DS18B20";

String ClientIP;
// send data
WiFiClient client;

// Define pins
#define ONE_WIRE_BUS 2 // DS18B20 PIN GPIO2

// Start includes
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature
DeviceAddress Sump;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         Turn pins off
void turnOff(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         SEND DATA
void send_data(String curHost, int curPort) {
  
  String yourdata;
  
  // Read DS18B20 and transmit value as sensor 1
  float t;

  sensors.begin(); //start up temp sensor
  delay(400); //Wait for newly restarted system to stabilize
  (!sensors.getAddress(Sump, 0));
  sensors.setResolution(Sump, 11);
 
  sensors.requestTemperatures(); // Get the temperature
  t = sensors.getTempCByIndex(0); // Get temperature in Celcius

  //check errors
  if (isnan(t)) {
    Serial.println("Failed to read from DS18B20 sensor!");
    return;
  }

  temperature = t;
     
  char host_char_array[curHost.length()+1];
  curHost.toCharArray(host_char_array,curHost.length()+1);
  
  if (!client.connect(host_char_array, curPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // calculate Base64Login
  memset(authVal,0,40);
  (Username + String(":") + Password).toCharArray(authVal, 40);
  memset(authValEncoded,0,40);
  base64_encode(authValEncoded, authVal, (Username + String(":") + Password).length());
  
  //Send Temperature
  yourdata = "{\"type\": \"value\", \"valueOrExpression\": \"" + String(temperature) + "\"}";
    
    client.print("PATCH /api/variables/");
    client.print(TempVarName);
    client.print(" HTTP/1.1\r\n");
    client.print("Authorization: Basic ");
    client.print(authValEncoded);
    client.print("\r\n");
    client.print("Host: " + curHost +"\r\n");
    client.print("Content-Type:application/json\r\n");
    client.print("Content-Length: ");
    client.print(yourdata.length());
    client.print("\r\n\r\n");
    client.print(yourdata);

  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         SETUP 
void setup(void)
{
  Serial.begin(115200);
  
      // disable all output to save power
      turnOff(0);
      //turnOff(2); //Not this one of course as it delivers our temperature
      turnOff(4);
      turnOff(5);
      turnOff(12);
      turnOff(13);
      turnOff(14);
      turnOff(15); 
   
      WiFi.begin(ssid.c_str(), password.c_str());
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      } 
      // Configure a static ip address; adapt for your network
      IPAddress ip(192,168,1,100);
      IPAddress gateway(192,168,1,1);
      IPAddress subnet(255,255,255,0);
      // When connected set static ip
      WiFi.config(ip, gateway, subnet);
      
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////           MAIN 
void loop(void)
{
  
  // To main pimatic
  send_data(host, httpPort);
  // to backup pimatic (bananapi)
  send_data(backuphost,backuphttpPort);
  
  Serial.println();
  Serial.println("closing connection. going to sleep...");
  delay(500);
  // go to deepsleep for "SleepTime" minutes
  ESP.deepSleep(SleepTime * 60 * 1000000);
  
} 
