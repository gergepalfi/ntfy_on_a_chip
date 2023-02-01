# ntfy_on_a_chip
ESP8266 and ESP32 client code to communicate with NTFY.sh server

This code is meant to be a basic example or starting point for using NTFY with ESP boards. Turning the on-board LED on/off is not very useful unless it is expanded with a relay to control something. The basic 2 way communication is all there so it can be used for much more complex tasks.

# What is this code for?

This code allows a person to safely connect an ESP device to the internet and control it from anywhere. It does not require a private server, MQTT broker, port forwarding or any other "advanced" networking. This also means that people behind a NAT (shared public IP) can use it.

https://ntfy.sh is free and open source, ideal for a maker to just get things working.

# Private vs Public
If you run a private ntfy instance or have a private sponsor account on ntfy.sh you can set username and password protection on the channel. In the public channels you need to choose a complex topic name (essentially the topic name is your password).

# Features:
- HTTPS using Root certificate (certs have around 20 year lifespan)
- username/password login (for private servers and supporters with accounts on ntfy.sh)
- listen for incoming commands on a topic (simply "0" or "1" for this example)
- execute something based on commands (turn LED on/off in this case)
- send a reply message

# Get Root Certificate (ESP32)
Firefox
1) open the website (ntfy.sh or your own private ntfy server URL)
2) click the lock in the URL bar > connection secure > more information
3) In the new pop-up window select secutity tab > view certificate
4) This will open a tab with the certificates. On top select the root CA (right side)
Note that the root certificate is valid for around 20 years, the code will not need recompiling until the certificate expires.
5) Find the section Miscellaneous and download `PEM (cert)`

Chrome: https://techtutorialsx.com/2017/11/18/esp32-arduino-https-get-request/

You should have a `myurl.pem` file that contains the root cert:
```
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
...
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
```
For ESP32 the content of this file can be pasted directly into the code.

# Get Certificate Store (ESP8266)
The ESP8266 board library already has the `ESP8266WiFi/examples/BearSSL_CertStore/certs-from-mozilla.py`, but it is not visible within Arduino IDE.

Open the folder `~/.arduino15/packages/esp8266/hardware/esp8266/3.1.1/libraries/ESP8266WiFi/examples/BearSSL_CertStore`, copy the python script to your project and run `python3 certs-from-mozilla.py`. It will create the necessary data folder with the `certs.ar` file that will be uploaded.

Do not mix up LittleFS with SPIFFS. I made that mistake so you don't have to. SPIFFS is no longer supported and can be considered dead.

LittleFS tool install: https://microcontrollerslab.com/littlefs-introduction-install-esp8266-nodemcu-filesystem-uploader-arduino/

The ESP has flash memory (4MB in my case), you can select how much of that will be reserved for LittleFS with the tools > Flash Size tab. I use `4MB (FS:2MB OTA:~1019KB)`

The code (first part of flash) and the LittleFS certificates (second part of flash) can be uploaded separately, order does not matter.

# Code Functionality Note
The information is requested from the server based on UNIX time (poll every X seconds). If multiple messages arrived in that interval, all messages are processed in order, then the state is applied. (Example: the LED state is off. You send a message `1` followed by a message `0`. The board will process these two at the next poll and apply state 0. The LED will never turn on as the command was immediately overruled by the next message in the que)
