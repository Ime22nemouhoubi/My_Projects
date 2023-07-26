#include "tpl_os.h"
#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <curl/curl.h>
#include <stdio.h>
#include <time.h>

byte sensorInterrupt1 = 0;  // Interrupt 0 = digital pin 2
byte sensorPin1 = 2;

byte sensorInterrupt2 = 1;  // Interrupt 1 = digital pin 3
byte sensorPin2 = 3;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // MAC address of the Ethernet shield
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount1;
volatile byte pulseCount2;

float flowRate1;
float flowRate2;

unsigned int flowMilliLitres1;
unsigned int flowMilliLitres2;

unsigned long totalMilliLitres1;
unsigned long totalMilliLitres2;

unsigned long oldTime;

EthernetClient client; // Ethernet Connection 

void setup()
{
    Ethernet.begin(mac);
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  
  delay(1000);
  Serial.println("System initialised");

  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached

  pinMode(sensorPin1, INPUT);
  digitalWrite(sensorPin1, HIGH);

  pinMode(sensorPin2, INPUT);
  digitalWrite(sensorPin2, HIGH);

  pulseCount1 = 0;
  pulseCount2 = 0;
  flowRate1 = 0.0;
  flowRate2 = 0.0;
  flowMilliLitres1 = 0;
  flowMilliLitres2 = 0;
  totalMilliLitres1 = 0;
  totalMilliLitres2 = 0;
  oldTime = 0;
  attachInterrupt(sensorInterrupt1, pulseCounter1, FALLING);
  attachInterrupt(sensorInterrupt2, pulseCounter2, FALLING);
}
void loop()
{
    // initialise the curl function
    CURLcode res = CURLE_OK;
    struct curl_slist *headers = NULL;
    char url[] = "http://10.3.2.77:8001/endpoint";


  if ((millis() - oldTime) > 1000) // Only process counters once per second
  {
    // Disable the interrupts while calculating flow rate and sending the values to the host
    detachInterrupt(sensorInterrupt1);
    detachInterrupt(sensorInterrupt2);

    // Because this loop may not complete in exactly 1-second intervals, we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensors.
    flowRate1 = ((1000.0 / (millis() - oldTime)) * pulseCount1) / calibrationFactor;
    flowRate2 = ((1000.0 / (millis() - oldTime)) * pulseCount2) / calibrationFactor;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts, the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1-second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres1 = (flowRate1 / 60) * 1000;
    flowMilliLitres2 = (flowRate2 / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres1 += flowMilliLitres1;
    totalMilliLitres2 += flowMilliLitres2;

    // Print the flow rates for this second in litres/minute
    Serial.print("Flow rate sensor 1: ");
    Serial.print(int(flowRate1)); // Print the integer part of the variable
    Serial.print(" L/min");
    Serial.print("\t"); // Print tab space

    Serial.print("Flow rate sensor 2: ");
    Serial.print(int(flowRate2)); // Print the integer part of the variable
    Serial.print(" L/min");
    Serial.println();

    // Print the cumulative total of litres flowed since starting
    Serial.print("Total liquid quantity sensor 1: ");
    Serial.print(totalMilliLitres1);
    Serial.print(" mL\t");
    Serial.print(totalMilliLitres1 / 1000);
    Serial.print(" L");

    Serial.print("\t");

    Serial.print("Total liquid quantity sensor 2: ");
    Serial.print(totalMilliLitres2);
    Serial.print(" mL\t");
    Serial.print(totalMilliLitres2 / 1000);
    Serial.print(" L");
    Serial.println();

    // Reset the pulse counters so we can start incrementing again
    pulseCount1 = 0;
    pulseCount2 = 0;

    // Re-enable the interrupts
    attachInterrupt(sensorInterrupt1, pulseCounter1, FALLING);
    attachInterrupt(sensorInterrupt2, pulseCounter2, FALLING);
  }
}
void pulseCounter1()
{
  // Increment the pulse counter for sensor 1
  pulseCount1++;
}

void pulseCounter2()
{
  // Increment the pulse counter for sensor 2
  pulseCount2++;
}

// Data compression

  TASK (DataCompression) {
  int curr1=0; 
  int curr2=0; 
  float percent = 0.05; // define the tolerated diffrence at 5%
  int threshold = 1024*percent; 
  if((curr1 >=flowRate1+threshold || curr1 <=flowRate1-threshold)){
  SERIALPutString(", flowRate1: ");
  }
   if((curr2 >=flowRate2+threshold || curr2 <=flowRate2-threshold)){
  SERIALPutString(", flowRate1: ");
  }
  TerminateTask();
  return 0;
  }
TASK(Http_TransmissionTask){
	    curl = curl_easy_init();
    if (curl) {
        // Set the POST headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        time_t timestamp = time(NULL); // Get now timestamp to send the server
		
        char post_data[100];

        SERIALPutstrig(post_data, "{\"flowRate1\": %.2f, \"timestamp\": %ld}", flowRate1, timestamp);
        SERIALPutstring(post_data, "{\"flowRate2\": %.2f, \"timestamp\": %ld}", flowRate2, timestamp);
        

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

        // Set the URL and headers
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			TerminateTask();
        }
		
    }

   TerminateTask();
}

