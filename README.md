# ntfy_on_a_chip
ESP8266 and ESP32 client code to communicate with NTFY.sh server

This code is meant to be a basic example or starting point for using NTFY with ESP boards. Turning the on-board LED on/off is not very useful unless it is expanded with a relay to control something. The basic 2 way communication is all there so it can be used for much more complex tasks.

The information is requested based on UNIX time from the server (poll every X seconds). All messages are processed in order until the end of the list.

# Features:
- HTTPS             *fingerprints expire fast (better solution on the way)
- username/password login for private servers that have this feature enabled
- listen for incoming commands on a topic (simply "0" or "1" for this example)
- execute something based on commands (turn LED on/off in this case)
- send a reply message
