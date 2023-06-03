# Water Monitoring Netwrok Using Arduino Uno and Arduino Ethernet Shield 
## Introduction

This project utilizes an Arduino Uno and an Arduino Ethernet Shield to collect data from four sensors: two float switches, a water flow meter, and an air pressure sensor. The collected data is displayed in the serial terminal and transmitted to the MongoDB cloud for storage and visualization. The code also implements a data compression 
technique by comparing the values from the flow switches and setting the current value to 0 if the difference exceeds 10%.

### Hardware Requirements

- Arduino Uno
- Arduino Ethernet Shield
- Two float switches
- Water flow meter
- Air pressure sensor
- Resistors (1000 ohms)
- Jumper wires
- Power source (battery or power supply)

### Software Requirements

- Arduino IDE
- Libraries:
  - LiquidCrystal
  - Adafruit_BME280
  - WiFiNINA

### Circuit Diagram

Refer to the circuit diagram image provided to connect the components to the Arduino Uno and Ethernet Shield. Adjust the pin connections according to the provided information.

### Usage

1. Set up the circuit according to the circuit diagram and make sure all connections are secure.
2. Open the Arduino IDE and upload the provided code to the Arduino Uno.
3. Modify the code to include your Wi-Fi credentials and cloud server details.
4. Connect the Arduino Uno to your computer using a USB cable.
5. Open the Serial Monitor in the Arduino IDE to view the sensor readings and data transmission status.
6. The data will be transmitted to the MongoDB cloud and stored for visualization and analysis.

### Additional Notes

- Adjust the compression threshold percentage in the code to fit your requirements.
- Ensure that your Wi-Fi network is stable and accessible by the Arduino.
- Make sure to create the necessary databases and collections in the MongoDB cloud for storing the sensor data.
