#  ![ntfy](https://img.shields.io/badge/ntfy-%2358bd8b?logo=ntfy&style=flat) on a Chip: Easy **ntfy** Integration for ESP Devices

ESP8266 and ESP32 client code to communicate with the [NTFY.sh](https://ntfy.sh) server.

## 🎯 Purpose

This code serves as a foundational example or starting point for integrating **ntfy** with ESP devices. While the current example focuses on turning an on-board LED on and off, it can be extended to control other hardware through relays or other mechanisms. It includes essential features for connecting ESP devices to the internet, enabling remote control without the need for private servers, MQTT brokers, or port forwarding. Ideal for makers and hobbyists, it supports both public and private **ntfy** channels.

### ❓ What is this code for?

This code allows a user to safely connect an ESP device to the internet and control it from anywhere. It avoids the complexity of private servers, MQTT brokers, port forwarding, and other advanced networking setups. This makes it accessible even for users behind NAT (shared public IP) networks.

**ntfy.sh** is a free and open-source service, making it an excellent choice for makers to get started quickly.

### 🔐 Private vs Public

For private instances or sponsor accounts on **ntfy.sh**, you can set username and password protection on your channel. For public channels, ensure that you choose a complex topic name, which essentially acts as your password.

## 🚀 Features

- **HTTPS Support**: Secure communication using a Root certificate.
- **Authentication**: Username/password login for private servers and supporters with **ntfy.sh** accounts.
- **Command Handling**: Listen for incoming commands on a topic and execute actions (e.g., turn LED on/off).
- **Response**: Send a reply message back to the server.

## 🛠️ Getting Started

### 🗝️ Obtaining the Root Certificate (ESP32)

1. **Using Firefox**:
   1. Open the **ntfy.sh** website or your private **ntfy** server URL.
   2. Click the lock icon in the URL bar, then select "Connection secure" > "More information."
   3. Go to the "Security" tab and click "View Certificate."
   4. Select the root CA certificate and download it in PEM format.

2. **Using Chrome**: [TechTutorialsX Guide](https://techtutorialsx.com/2017/11/18/esp32-arduino-https-get-request/)

   - Save the root certificate as `myurl.pem` and use its content in your code.

### 📜 Certificate Store (ESP8266)

1. **Using ESP8266 Cert Store**:
   - Find the `certs-from-mozilla.py` script in the ESP8266 library folder.
   - Run the script with Python to generate the necessary certificate store (`certs.ar` file).

2. **LittleFS vs. SPIFFS**:
   - Use LittleFS for filesystem storage.
   - Avoid using SPIFFS as it is no longer supported.

## ⚙️ Code Functionality

The ESP devices poll the server at regular intervals (based on UNIX time). Messages are processed in order, and actions are applied accordingly. For example, if multiple messages are received, they will be processed and applied in the sequence they were received.

## Example Project: 🐱 Feline Watchdog

The `Feline Watchdog` project showcases a practical application of this code:

- **Project Overview**: Feline Watchdog is an ESP32-based system designed to monitor the Wi-Fi connection of your cat's collar-mounted ESP32 device. If the connection is lost, the system sends a notification to your mobile device using **ntfy**.

- **Features**:
  - **ESP32 Configuration**: Connects to a Wi-Fi network or a Raspberry Pi hotspot.
  - **Notification System**: Sends alerts when the ESP32 device goes out of range.
  - **Power Supply**: Primarily powered by a Li-ion battery for the collar-mounted ESP32. USB power is used only for reprogramming.

- **Repository**: [Feline Watchdog Repository](https://github.com/k-antoniou/FelineWatchdog)

The `Feline Watchdog` project demonstrates how to utilize the `ntfy_on_a_chip` example code to build a real-world application. It integrates with the **ntfy** service to provide practical alerts for pet owners.

## 🤝 Contributing

Contributions are welcome! Feel free to submit pull requests or open issues if you encounter any problems.

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

**Thank you for exploring the NTFY on a Chip project!** ✨
