#include <Time.h>
#include <ESP8266mDNS.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Base64.h>
#include <EEPROM.h>
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        !!PLEASE CHANGE THESE!!
String ssid		= "WiFi SSID";
String password	= "WiFi Password";

String espName		= "esp01";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        NETWORK
ESP8266WebServer	server(80);
MDNSResponder		mdns;

const char* APssid = "ESPap";
const char* APpassword = "123456789";



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         GLOBAL VARIABLES
long		lastInterval	= 0;
const int	httpPort		= 80;
float		temperature		= 0.0;
float		humidity		= 0.0;

String deviceType		= "DHT22";
long sendInterval		= 60000; //in millis

String Username			= "admin";
String Password			= "fussel";

String host		= "192.168.178.1";


String ClientIP;
// send data
WiFiClient client;


//uint8_t DHTPIN = 2;  //data pin, GPIO2
//uint8_t DHTTYPE = DHT22;

DHT dht(2, DHT22, 20);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         HTML SNIPPLETS
String header				=  "<html lang='en'><head><title>ESP8266 Pimatic Client</title><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='stylesheet' href='http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js'></script><script src='http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js'></script></head><body>";
String navbar				=  "<nav class='navbar navbar-default'><div class='container-fluid'><div class='navbar-header'><a class='navbar-brand' href='/'>ESP8266 Pimatic Client</a></div><div><ul class='nav navbar-nav'><li class='active'><a href='/'><span class='glyphicon glyphicon-dashboard'></span> Status</a></li><li class='dropdown'><a class='dropdown-toggle' data-toggle='dropdown' href='#'><span class='glyphicon glyphicon-cog'></span> Configure<span class='caret'></span></a><ul class='dropdown-menu'><li><a href='/cliconf'>Client</a></li><li><a href='/serconf'>Server</a></li></ul></li></ul></div></div></nav>  "; 
String navbarNonActive		= "<nav class='navbar navbar-default'><div class='container-fluid'><div class='navbar-header'><a class='navbar-brand' href='/'>ESP8266 Pimatic Client</a></div><div><ul class='nav navbar-nav'><li><a href='/'><span class='glyphicon glyphicon-dashboard'></span> Status</a></li><li class='dropdown'><a class='dropdown-toggle' data-toggle='dropdown' href='#'><span class='glyphicon glyphicon-cog'></span> Configure<span class='caret'></span></a><ul class='dropdown-menu'><li><a href='/cliconf'>Client</a></li><li><a href='/serconf'>Server</a></li></ul></li></ul></div></div></nav>  ";
String containerStart		=  "<div class='container'><div class='row'>";
String containerEnd			=  "<div class='clearfix visible-lg'></div></div></div>";
String siteEnd				=  "</body></html>";
  
String panelHeaderName		=  "<div class='col-md-4'><div class='page-header'><h1>";
String panelHeaderEnd		=  "</h1></div>";
String panelEnd				=  "</div>";
  
String panelBodySymbol		=  "<div class='panel panel-default'><div class='panel-body'><span class='glyphicon glyphicon-";
String panelBodyName		=  "'></span> ";
String panelBodyValue		=  "<span class='pull-right'>";
String panelBodyEnd			=  "</span></div></div>";

String inputBodyStart		=  "<form action='' method='POST'><div class='panel panel-default'><div class='panel-body'>";
String inputBodyName		=  "<div class='form-group'><div class='input-group'><span class='input-group-addon' id='basic-addon1'>";
String inputBodyPOST		=  "</span><input type='text' name='";
String inputBodyClose		=  "' class='form-control' aria-describedby='basic-addon1'></div></div>";
String inputBodyEnd			=  "</div><div class='panel-footer clearfix'><div class='pull-right'><button type='submit' class='btn btn-default'>Send</button></div></div></div></form>";


//String landingNav			=	"<nav class='navbar navbar-default'> <div class='container-fluid'> <div class='navbar-header'> <a class='navbar-brand' href='/'>ESP8266 Pimatic Client</a> </div></div></nav><br><br><br>";
//String landingStartPartA	=	"<div class='container'> <div class='row' > <div class='col-md-offset-3 col-md-6'> <div class='panel panel-default'> <div class='panel-heading'> Please login to your WiFi Network </div><div class='panel-body'> <form method='post' action=''> <div class='form-group'><div class='input-group'><span class='input-group-addon' id='basic-addon1'>Wifi Name </span><input type='text' class='form-control' placeholder='Enter SSID' aria-describedby='basic-addon1' name='wifiname'></div></div>";
//String landingStartPartB    =   "<div class='form-group'><div class='input-group'><span class='input-group-addon' id='basic-addon1'>WiFi Pass </span><input type='password' class='form-control' placeholder='Password' aria-describedby='basic-addon1' name='wifipass'></div></div><div class='form-group'><div class='input-group'><span class='input-group-addon' id='basic-addon1'>Device Name ";
//String landingStartPartC      =    "</span><input type='text' class='form-control' placeholder='Name your device e.g. kitchen' aria-describedby='basic-addon1' name='devicename'></div></div><a class='btn btn-primary' data-toggle='collapse' href='#collapseExample' aria-expanded='false' aria-controls='collapseExample'> Available Networks </a> <div class='pull-right'> <button type='submit' class='btn btn-default'>Send</button> </div></form> <div class='collapse' id='collapseExample'> <div class='well'>";
//String landingEnd			=	"</div></div></div></div></div></div></body></html>";


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         ROOT 
void handle_root() {

  Serial.println(server.args());
  
  // get IP
  IPAddress ip = WiFi.localIP();
  ClientIP = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  delay(500);
  
  String title1			= panelHeaderName + String("Sensor Data") + panelHeaderEnd;
  String Humidity		= panelBodySymbol + String("tint") + panelBodyName + String("Humidity") + panelBodyValue + humidity + String("%") + panelBodyEnd;
  String Temperature    = panelBodySymbol + String("fire") + panelBodyName + String("Temperature") + panelBodyValue + temperature + String("°C") + panelBodyEnd + panelEnd;
  
  String title2			= panelHeaderName + String("Client Settings") + panelHeaderEnd;
  String IPAddClient    = panelBodySymbol + String("globe") + panelBodyName + String("IP Address") + panelBodyValue + ClientIP + panelBodyEnd;
  String DeviceType		= panelBodySymbol + String("scale") + panelBodyName + String("Device Type") + panelBodyValue + deviceType + panelBodyEnd;
  String ClientName		= panelBodySymbol + String("tag") + panelBodyName + String("Client Name") + panelBodyValue + espName + panelBodyEnd;
  String Interval		= panelBodySymbol + String("hourglass") + panelBodyName + String("Interval") + panelBodyValue + sendInterval + String(" millis") + panelBodyEnd;
  String Uptime			= panelBodySymbol + String("time") + panelBodyName + String("Uptime") + panelBodyValue + hour() + String(" h ") + minute() + String(" min ") + second() + String(" sec") + panelBodyEnd + panelEnd;
  
  String title3			= panelHeaderName + String("Server Settings") + panelHeaderEnd;
  String IPAddServ		= panelBodySymbol + String("globe") + panelBodyName + String("IP Address") + panelBodyValue + host + panelBodyEnd;
  String User			= panelBodySymbol + String("user") + panelBodyName + String("Username") + panelBodyValue + Username + panelBodyEnd + panelEnd;
  
  
  //String data = title1 + Humidity + Temperature + title2 + IPAddClient + DeviceType + ClientName + Interval + Uptime + title3 + IPAddServ + User;
  //server.send ( 200, "text/html", header + navbar + containerStart + data + containerEnd + siteEnd );
   server.send ( 200, "text/html", header + navbar + containerStart + title1 + Humidity + Temperature + title2 + IPAddClient + DeviceType + ClientName + Interval + Uptime + title3 + IPAddServ + User + containerEnd + siteEnd);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         CONFIG - Client
void handle_cliconf() {

  String payload=server.arg("name");
  if (payload.length() > 0 ) {
    espName = payload;
  }
  Serial.println(payload);

  payload=server.arg("type");
  if (payload.length() > 0 ) {
    deviceType = payload;
  }
  Serial.println(payload);

  payload=server.arg("interval");
  if (payload.length() > 0 ) {
    sendInterval = payload.toInt();
  }
  Serial.println(payload);
  
  String title1 = panelHeaderName + String("Client Configuration") + panelHeaderEnd; 
  
  String data = title1 + inputBodyStart + inputBodyName + String("Name") + inputBodyPOST + String("name") + inputBodyClose + inputBodyName + String("Device Type") + inputBodyPOST + String("type") + inputBodyClose + inputBodyName + String("Interval in seconds") + inputBodyPOST + String("interval") + inputBodyClose + inputBodyEnd;
  server.send ( 200, "text/html", header + navbarNonActive + containerStart + data + containerEnd + siteEnd );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         CONFIG - Server
void handle_serconf() {
  
  String payload=server.arg("server");
  if (payload.length() > 0 ) {
    host = payload;
  }
  Serial.println(payload);

  payload=server.arg("user");
  if (payload.length() > 0 ) {
    Username = payload;
  }
  Serial.println(payload);

  payload=server.arg("password");
  if (payload.length() > 0 ) {
    Password = payload.toInt();
  }
  Serial.println(payload);
  
  String title1 = panelHeaderName + String("Server Configuration") + panelHeaderEnd;
  
  String data = title1 + inputBodyStart + inputBodyName + String("Pimativ Server") + inputBodyPOST + String("server")  + inputBodyClose + inputBodyName + String("Username") + inputBodyPOST + String("user") + inputBodyClose + inputBodyName + String("Password") + inputBodyPOST + String("password") + inputBodyClose + inputBodyEnd;
  
  server.send ( 200, "text/html", header + navbarNonActive + containerStart + data + containerEnd + siteEnd);
  
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         LANDING
/*
void landing() {
  
  String payload=server.arg("wifiname");
  if (payload.length() > 0 ) {
    ssid = payload;
  }
  Serial.println(payload);

  payload=server.arg("wifipass");
  if (payload.length() > 0 ) {
    password = payload;
  }
  Serial.println(payload);
  
  payload=server.arg("devicename");
  if (payload.length() > 0 ) {
    espName = payload;
  }
  Serial.println(payload);
  
  String landing = header + landingNav + landingStartPartA + landingStartPartB + landingStartPartC + landingEnd;
  server.send ( 200, "text/html", landing);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         SEND DATA
void send_data() {
  
  String yourdata;
  
  // get data
  float h	= dht.readHumidity();
  float t	= dht.readTemperature();
  
  //check erros
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  temperature	= t;
  humidity		= h;
  
  
  
  
  if (!client.connect(host.toInt(), httpPort)) {
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         AP
void setupAP(void) {
  delay(100);
  WiFi.softAP(APssid,APpassword,6);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////         SETUP 
void setup(void)
{
  Serial.begin(115200);
 
   
      WiFi.begin(ssid.c_str(), password.c_str());
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

      //server.on("/landing", landing);
  
      if (!mdns.begin(espName.c_str(), WiFi.localIP())) {
        Serial.println("Error setting up MDNS responder!");
        while(1) { 
          delay(1000);
        }
      }
      server.begin();
      Serial.println("HTTP server started");
      dht.begin();
      



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








