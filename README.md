# ntfy_on_a_chip
ESP8266 and ESP32 client code to interact with the [ntfy.sh](https://ntfy.sh) server, enabling remote communication and control.

## 🎯 Purpose

This code provides a foundational example for integrating **ntfy** with ESP devices, enabling remote control over the internet. It eliminates the need for private servers, MQTT brokers, or port forwarding, making it accessible even for users behind NAT (shared public IP) networks. hile the example demonstrates controlling an on-board LED, it can be expanded for more complex applications, such as using relays to control other devices. 

https://ntfy.sh is free and open source, ideal for a maker to just get things working.

## 🔐 Private vs Public Channels

- **Private Channels**: If you are using a private ntfy instance or a sponsor account, you can set username and password protection for your channel.
- **Public Channels**: For public channels, ensure your topic name is complex and acts as a password.

## 🚀 Features

- **HTTPS Support**: Secure communication using a Root certificate.
- **Authentication**: Username/password for private servers and sponsor accounts.
- **Command Handling**: Listen for commands and execute actions (e.g., turn LED on/off).
- **Response**: Send a reply message to the server.

## 🛠️ Getting Started

### 🗝️ Obtaining the Root Certificate (ESP32)

1. **Using Firefox**:
   - Open the **ntfy.sh** website or your private ntfy server URL.
   - Click the lock icon in the URL bar, then select "Connection secure" > "More information."
   - In the new window, go to the "Security" tab and click "View Certificate."
   - Select the root CA certificate and download it in PEM format.
   - You should have a file named `myurl.pem` that contains the root certificate. It will look similar to the following:

     ```
     -----BEGIN CERTIFICATE-----
     MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
     ...
     emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
     -----END CERTIFICATE-----
     ```

2. **Using Chrome**: Follow this [TechTutorialsX Guide](https://techtutorialsx.com/2017/11/18/esp32-arduino-https-get-request/) to obtain the root certificate. Save it as `myurl.pem` and use its content in your code.

### 🗝️ Certificate Store (ESP8266)

1. **Generate Certificate Store**:
   - Locate the `certs-from-mozilla.py` script within the ESP8266 library folder. The path typically is:
     ```
     ~/.arduino15/packages/esp8266/hardware/esp8266/3.1.1/libraries/ESP8266WiFi/examples/BearSSL_CertStore
     ```
   - Copy `certs-from-mozilla.py` to your project directory.
   - Run the script using Python 3 to generate the necessary certificate store file (`certs.ar`):
     ```sh
     python3 certs-from-mozilla.py
     ```
   - The generated `certs.ar` file should be uploaded to the ESP8266.

2. **LittleFS vs. SPIFFS**:
   - **LittleFS** is the preferred filesystem for ESP8266. Avoid using SPIFFS as it is deprecated.
   - **LittleFS Tool Installation**: [Guide](https://microcontrollerslab.com/littlefs-introduction-install-esp8266-nodemcu-filesystem-uploader-arduino/)

### 📈 Flash Layout

For ESP8266, configure the flash memory as follows:
   - **Total Flash Size**: 4MB
   - **LittleFS Size**: 2MB
   - **OTA (Over-the-Air) Size**: ~1019KB

This layout ensures you have enough space for both the code and filesystem.

### ⏳ Certificate Lifespan

The root certificate is valid for around **20 years**, so you will not need to update the certificate frequently. This long validity period means that code recompilation will not be necessary until the certificate expires.

### 📜 Functionality with Piled-Up Messages

The ESP devices poll the server at regular intervals based on UNIX time. Messages are processed in the order they are received, and the latest message determines the final state. For instance, if multiple commands are received (e.g., `1` followed by `0`), the board processes these messages sequentially and applies the final command.

## Example Project: 🐱 Feline Watchdog

The [Feline Watchdog](https://github.com/k-antoniou/FelineWatchdog) project demonstrates a practical application of this code:

- **Overview**: Monitors the Wi-Fi connection of a collar-mounted ESP32 and sends notifications if the device goes out of range.

## 🤝 Contributing

Contributions are welcome! Please submit pull requests or open issues if you encounter any problems.

## 📜 License

This project is licensed under the [GPLv3 License](https://opensource.org/licenses/GPL-3.0) - see the [LICENSE](LICENSE) file for details.

**Thank you for exploring the NTFY on a Chip project!** ✨
