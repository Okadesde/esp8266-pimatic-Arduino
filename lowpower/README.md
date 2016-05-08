# esp8266-ds18b20-lp

This is a minimal barebones ESP8266 ino for pimatic. It tries to be as low power as possible (hence the -lp extension). 
This one is made for a ds18b20 temperature sensor but you can just as easily convert it to DHT11/DHT22.<br>
It uses no webgui and no telnet access, so you can not access it via your network and niether uses it mDNS.<br>
It is based on a static ip address you assign inside the code. Note that an ip reservation inside your router for the macaddress of your ESP will not work).<br>
A web server, telnet access, mDNS and/or a static ip address are all power requiring options.

What you have to change inside the code before loading it into your ESP:
- Your wlan ssid and password (ssid & password)
- Your pimatic posting user and password (Username & Password)
- The variable name you use inside pimatic (TempVarName)
- Your pimatic ip address and port (host & httpPort)
- The ip address your ESP has (you have to assign a static ip address inside the code)
- The pin your sensor is connected to (and disable pinoff for that pin in the setup)
- Optional: extra pimatic hosts you want to send to


If you want a lot of options use either [ESPimatic](https://github.com/koffienl/ESPimatic) or [ESPeasy](http://www.esp8266.nu/index.php/ESPEasy).
