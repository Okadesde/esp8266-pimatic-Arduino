#include <Time.h>
#include <ESP8266mDNS.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Base64.h>

//#define DHTPIN 2
//#define DHTTYPE DHT22
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        !!PLEASE CHANGE THESE!!
const char* ssid = "YOUR_WiFi_NAME";
const char* password = "YOUR_WiFi_PASSWORD";
const char* espName = "ESP_NAME";
long sendInterval = 60000; //in millis

String Username = "PIMATIC_USERNAME";
String Password = "PIMATIC_PASSWORD";

const char* host = "PIMATIC_IP";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        NETWORK
ESP8266WebServer server(80);
MDNSResponder mdns;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         GLOBAL VARIABLES
long lastInterval = 0;
const int httpPort = 80;
float temperature = 0.0;
float humidity  = 0.0;
String ClientIP;

DHT dht(2, DHT22, 20);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         HTML SNIPPLETS
String header            =  "<html lang='en'><head><title>ESP8266 Pimatic Client</title><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='stylesheet' href='http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js'></script><script src='http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js'></script></head><body>";
String navbar            =  "<nav class='navbar navbar-default'><div class='container-fluid'><div class='navbar-header'><a class='navbar-brand' href='/'>ESP8266 Pimatic Client</a></div><div><ul class='nav navbar-nav'><li class='active'><a href='/'><span class='glyphicon glyphicon-dashboard'></span> Status</a></li><li class='dropdown'><a class='dropdown-toggle' data-toggle='dropdown' href='#'><span class='glyphicon glyphicon-cog'></span> Configure<span class='caret'></span></a><ul class='dropdown-menu'><li><a href='/cliconf'>Client</a></li><li><a href='/serconf'>Server</a></li><li><a href='/help'>Help</a></li></ul></li></ul></div></div></nav>  "; 
String navbarNonActive = "<nav class='navbar navbar-default'><div class='container-fluid'><div class='navbar-header'><a class='navbar-brand' href='/'>ESP8266 Pimatic Client</a></div><div><ul class='nav navbar-nav'><li><a href='/'><span class='glyphicon glyphicon-dashboard'></span> Status</a></li><li class='dropdown'><a class='dropdown-toggle' data-toggle='dropdown' href='#'><span class='glyphicon glyphicon-cog'></span> Configure<span class='caret'></span></a><ul class='dropdown-menu'><li><a href='/cliconf'>Client</a></li><li><a href='/serconf'>Server</a></li><li><a href='/help'>Help</a></li></ul></li></ul></div></div></nav>  ";
String containerStart  =  "<div class='container'><div class='row'>";
String containerEnd   =  "<div class='clearfix visible-lg'></div></div></div>";
String siteEnd           =  "</body></html>";
  
String panelHeaderName    =  "<div class='col-md-4'><div class='page-header'><h1>";
String panelHeaderEnd    =  "</h1></div>";
String panelEnd      =  "</div>";
  
String panelBodySymbol   =  "<div class='panel panel-default'><div class='panel-body'><span class='glyphicon glyphicon-";
String panelBodyName     =  "'></span> ";
String panelBodyValue      =  "<span class='pull-right'>";
String panelBodyEnd        =  "</span></div></div>";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         ROOT 
void handle_root() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  
  // get IP
  IPAddress ip = WiFi.localIP();
  ClientIP = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  
  // sendInterval to hours
  float intervalSeconds = sendInterval / 1000;
  float intervalMinutes = intervalSeconds / 60;
  
  String title1 = panelHeaderName + String("Sensor Data") + panelHeaderEnd;
  String Humidity    = panelBodySymbol + String("tint") + panelBodyName + String("Humidity") + panelBodyValue + humidity + String("%") + panelBodyEnd;
  String Temperature    = panelBodySymbol + String("fire") + panelBodyName + String("Temperature") + panelBodyValue + temperature + String("°C") + panelBodyEnd + panelEnd;
  
  String title2             = panelHeaderName + String("Client Settings") + panelHeaderEnd;
  String IPAddClient    = panelBodySymbol + String("globe") + panelBodyName + String("IP Address") + panelBodyValue + ClientIP + panelBodyEnd;
  String DeviceType    = panelBodySymbol + String("scale") + panelBodyName + String("Device Type") + panelBodyValue + String("DHT22") + panelBodyEnd;
  String ClientName    = panelBodySymbol + String("tag") + panelBodyName + String("Client Name") + panelBodyValue + espName + panelBodyEnd;
  String Interval          = panelBodySymbol + String("hourglass") + panelBodyName + String("Interval") + panelBodyValue + intervalMinutes + String(" min") + panelBodyEnd;
  String Uptime           = panelBodySymbol + String("time") + panelBodyName + String("Uptime") + panelBodyValue + hour() + String(" h ") + minute() + String(" min ") + second() + String(" sec") + panelBodyEnd + panelEnd;
  
  String title3             = panelHeaderName + String("Server Settings") + panelHeaderEnd;
  String IPAddServ       = panelBodySymbol + String("globe") + panelBodyName + String("IP Address") + panelBodyValue + host + panelBodyEnd;
  String User              = panelBodySymbol + String("user") + panelBodyName + String("Username") + panelBodyValue + Username + panelBodyEnd + panelEnd;
  
  
  String data = title1 + Humidity + Temperature + title2 + IPAddClient + DeviceType + ClientName + Interval + Uptime + title3 + IPAddServ + User;
  server.send ( 200, "text/html", header + navbar + containerStart + data + containerEnd + siteEnd );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         CONFIG - Client
void handle_cliconf() {
  
  String data = "WIP";
  server.send ( 200, "text/html", header + navbarNonActive + containerStart + data + containerEnd + siteEnd );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         CONFIG - Server
void handle_serconf() {
  
  String data = "WIP";
  server.send ( 200, "text/html", header + navbarNonActive + containerStart + data + containerEnd + siteEnd);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         HELP
void handle_help() {
  
  String data = "WIP";
  server.send ( 200, "text/html", header + navbarNonActive + containerStart + data + containerEnd + siteEnd);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         SEND DATA
void send_data() {
  
  String yourdata;
  
  // get data
  float h = dht.readHumidity();
  float t  = dht.readTemperature();
  
  //check erros
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  temperature  = t;
  humidity        = h;
  
  // send data
  WiFiClient client;
  
  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // calculate Base64Login
  
  
  
  char base64login[40];
  
  (Username + String(":") + Password).toCharArray(base64login, 40);
  
  //Send Humidity
  yourdata = "{\"type\": \"value\", \"valueOrExpression\": \"" + String(h) + "\"}";
  
  
    client.print("PATCH /api/variables/");
    client.print(espName);
    client.print("-hum HTTP/1.1\r\n");
    client.print("Authorization: Basic ");
    client.print(base64login);
    client.print("\r\n");
    client.print("Host: 192.168.178.57\r\n");
    client.print("Content-Type:application/json\r\n");
    client.print("Content-Length: ");
    client.print(yourdata.length());
    client.print("\r\n\r\n");
    client.print(yourdata);
    
  delay(500);
  
  //Send Temperature
  yourdata = "{\"type\": \"value\", \"valueOrExpression\": \"" + String(t) + "\"}";
    
    client.print("PATCH /api/variables/");
    client.print(espName);
    client.print("-tem HTTP/1.1\r\n");
    client.print("Authorization: Basic YWRtaW46ZnVzc2Vs\r\n");
    client.print("Host: 192.168.178.57\r\n");
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
  
  dht.begin();

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  
   
  server.on("/", handle_root);
  
  server.on("/cliconf", handle_cliconf);
  
  server.on("/serconf", handle_serconf);
  
  server.on("/help", handle_help);
  
  if (!mdns.begin(espName, WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  
  server.begin();
  Serial.println("HTTP server started");
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////           MAIN 
void loop(void)
{
  if (millis() - lastInterval > sendInterval) {
    send_data();
    lastInterval = millis();
  }
  server.handleClient();
} 
