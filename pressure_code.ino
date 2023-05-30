#include <LiquidCrystal.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <Ethernet.h>

// Ethernet shield MAC address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Cloud server details
const char* server = "mongodb+srv://imenemouhoubi337:<Projetpfe23>@cluster0.em44do3.mongodb.net/";
const int port = 80;
const String endpoint = "https://us-east-1.aws.data.mongodb-api.com/app/application-0-szsse/endpoint";

// Pin assignments
const int floatSwitch1Pin = 2;
const int floatSwitch2Pin = 3;
const int pressureTransductor1Pin = A0;
const int pressureTransductor2Pin = A1;
const int airPressureSensorPin = A2;

// Variables
int floatSwitch1State = 0;
int floatSwitch2State = 0;
float pressureTransductor1Value = 0.0;
float pressureTransductor2Value = 0.0;
float airPressureSensorValue = 0.0;

// Initialize the Ethernet client
EthernetClient client;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Start the Ethernet connection
  Ethernet.begin(mac);

  // Wait for Ethernet to initialize
  delay(1000);

  // Configure pin modes
  pinMode(floatSwitch1Pin, INPUT);
  pinMode(floatSwitch2Pin, INPUT);
}

void loop() {
  // Read the state of float switches
  floatSwitch1State = digitalRead(floatSwitch1Pin);
  floatSwitch2State = digitalRead(floatSwitch2Pin);

  // Read the values from pressure transducers
  pressureTransductor1Value = analogRead(pressureTransductor1Pin);
  pressureTransductor2Value = analogRead(pressureTransductor2Pin);

  // Read the value from air barometric pressure sensor
  airPressureSensorValue = analogRead(airPressureSensorPin);

  // Data compression
  int curr = 0;
  float percent = 0.1;
  int threshold = 1024 * percent;

  if ((floatSwitch1State >= floatSwitch2State + threshold || floatSwitch1State <= floatSwitch2State - threshold)) {
    Serial.println("Float Switch 2 condition not met");
  }

  if ((pressureTransductor1Value >= pressureTransductor2Value + threshold || pressureTransductor1Value <= pressureTransductor2Value - threshold)) {
    Serial.println("Pressure Transductor 2 condition not met");
  }

  if ((curr >= airPressureSensorValue + threshold || curr <= airPressureSensorValue - threshold)) {
    Serial.println("Air Pressure Sensor condition not met");
  }

  // Print the sensor values
  Serial.print("Float Switch 1: ");
  Serial.println(floatSwitch1State);
  Serial.print("Float Switch 2: ");
  Serial.println(floatSwitch2State);
  Serial.print("Pressure Transductor 1: ");
  Serial.println(pressureTransductor1Value);
  Serial.print("Pressure Transductor 2: ");
  Serial.println(pressureTransductor2Value);
  Serial.print("Air Pressure Sensor: ");
  Serial.println(airPressureSensorValue);

  // Send data to the cloud server
  sendDataToCloud();

  delay(1000); // Delay for stability
}

void sendDataToCloud() {
  // Create an HTTP POST request body with the sensor data
  String data = "Float Switch 1: " + String(floatSwitch1State) +
                ", Float Switch 2: " + String(floatSwitch2State) +
                ", Pressure Transductor 1: " + String(pressureTransductor1Value) +
                ", Pressure Transductor 2: " + String(pressureTransductor2Value) +
                ", Air Pressure Sensor: " + String(airPressureSensorValue);

  // Create the HTTP POST request
  String request = "POST " + endpoint + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n" +
                   "Content-Length: " + String(data.length()) + "\r\n" +
                   "Connection: close\r\n\r\n" +
                   data;

  // Connect to the cloud server
  if (client.connect(server, port)) {
    Serial.println("Connected to server");

    // Send the HTTP POST request
    client.print(request);
    Serial.println("HTTP request sent");

    // Wait for the response from the server
    while (client.connected()) {
      if (client.available()) {
        String response = client.readStringUntil('\n');
        Serial.println(response);
      }
    }

    // Disconnect from the server
    client.stop();
    Serial.println("Disconnected from server");
  }
}
