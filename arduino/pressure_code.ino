#include <LiquidCrystal.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <Ethernet.h>

// Ethernet shield MAC address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Cloud server details
const char* server = "mongodb://atlas-sql-6419945192b46315861eb975-7t8cd.a.query.mongodb.net/sample_airbnb?ssl=true&authSource=admin";
const int port = 27017;
const String endpoint = "mongodb+srv://imouhoubi:<Imene233>@cluster0.lijzt0k.mongodb.net/";

// Pin assignments
const int floatSwitch1Pin = 2;
const int floatSwitch2Pin = 3;
const int flowMeterPin = 4; // Connect flow meter to digital pin 4
const int airPressureSensorPin = A2;

// Variables
int floatSwitch1State = 0;
int floatSwitch2State = 0;
float flowMeterValue = 0.0; // Flow meter reading
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
  pinMode(flowMeterPin, INPUT_PULLUP); // Use internal pull-up resistor
}

void loop() {
  // Read the state of float switches
  floatSwitch1State = digitalRead(floatSwitch1Pin);
  floatSwitch2State = digitalRead(floatSwitch2Pin);

  // Read the value from flow meter
  flowMeterValue = pulseIn(flowMeterPin, HIGH); // Measure pulse duration

  // Read the value from air barometric pressure sensor
  airPressureSensorValue = analogRead(airPressureSensorPin);

  // Data compression
  int curr = 0;
  float percent = 0.1;
  int threshold = 1024 * percent;

  if ((floatSwitch1State >= floatSwitch2State + threshold || floatSwitch1State <= floatSwitch2State - threshold)) {
    Serial.println("Float Switch 2 condition not met");
  }

  // Compare flow meter values
  if (flowMeterValue < 100) {
    Serial.println("Flow Meter condition not met");
  }

  if ((curr >= airPressureSensorValue + threshold || curr <= airPressureSensorValue - threshold)) {
    Serial.println("Air Pressure Sensor condition not met");
  }

  // Print the sensor values
  Serial.print("Float Switch 1: ");
  Serial.println(floatSwitch1State);
  Serial.print("Float Switch 2: ");
  Serial.println(floatSwitch2State);
  Serial.print("Flow Meter: ");
  Serial.println(flowMeterValue);
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
                ", Flow Meter: " + String(flowMeterValue) +
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
        
        // Check for the existence or create the database
        if (response.indexOf("Database exists") != -1) {
          Serial.println("Database exists");
        } else if (response.indexOf("Database created") != -1) {
          Serial.println("Database created");
        }
      }
    }

    // Disconnect from the server
    client.stop();
    Serial.println("Disconnected from server");
  }
}
