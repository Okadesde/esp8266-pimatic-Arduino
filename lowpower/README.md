# esp8266-ds18b20-lp

This is a minimal barebones ESP8266 ino for pimatic. It tries to be as low power as possible (hence the -lp extension). 
This one is made for a ds18b20 temperature sensor but you can just as easily convert it to DHT11/DHT22.

What you have to change inside the code before loading it into your ESP:
- Your wlan ssid and password (ssid & password)
- Your pimatic posting user and password (Username & Password)
- The variable name you use inside pimatic (TempVarName)
- Your pimatic ip address and port (host & httpPort)
- The pin your sensor is connected to (and disable pinoff for that pin in the setup)
- Optional: extra pimatic hosts you want to send to
