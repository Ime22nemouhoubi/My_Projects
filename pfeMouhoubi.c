#undef READY
#include "sam.h"
#include <pinAccess.h>
#include <../../../board/iutNantes/include/oled.h>
#include <../../../board/iutNantes/include/timer.h>
#include <eic.h>
#include "sensor_id_PH.h" // ph lib
#include <EEPROM.h> // ph lib
#include <curl/curl.h>
#include <time.h>
#define PH_PIN A1 // ph

#include <stdio.h>

volatile unsigned int res1l = 10;
volatile unsigned int res2l = 0;
volatile unsigned int distance_data = 0;

typedef struct {
  int32_t version;
  int32_t sensor_id;
  int32_t sensor_type;
  int32_t reserved0;
  int32_t timestamp;
  union {
    float data[4];
    sensors_vec_t acceleration;
    sensors_vec_t orientation;
    sensors_vec_t gyro;
    float distance;
  };
} ultrasonic_event_t;

typedef struct {
  int32_t version;
  int32_t sensor_id;
  int32_t sensor_type;
  int32_t reserved0;
  int32_t timestamp;
  union {
    float data[4];
    sensors_vec_t acceleration;
    sensors_vec_t orientation;
    sensors_vec_t gyro;
    float ph_data;
  };
} ph_event_t;

int main(void) {
  EICInitClock(F1MHZ);
  TCinitClock(F1MHZ, 3);

  OLEDInit(0);

  for (int i = 0; i < 8; i++) {
    pinMode(PORTB, i, OUTPUT);
    digitalWrite(PORTB, i, 1);
  }

  pinMode(PORTA, 13, OUTPUT); // Push button ld 
  pinMode(PORTA, 28, INPUT); //BT1 push button
  pinMode(PORTA, 16, OUTPUT); // IN Moteur
  pinMode(PORTB, 14, OUTPUT); // Enable  Moteur
  pinMode(PORTB, 9, OUTPUT); //TRIGGER pin
  pinMode(PORTB, 15, INPUT); //ECHO pin
  pinMode(PORTB, 12, INPUT); //A1 pin for ph sensor (input)

// display the distance_data on lcd screen
#define LCD_RS (PORTA, 10)
#define LCD_E (PORTA, 11)
#define LCD_D4 (PORTA,12)
#define LCD_D5 (PORTA, 13)
#define LCD_D6 (PORTA, 16)
#define LCD_D7 (PORTA, 17)

void setup() {
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_E, OUTPUT);
  pinMode(LCD_D4, OUTPUT);
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D7, OUTPUT);

  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, LOW);
  digitalWrite(LCD_D4, LOW);
  digitalWrite(LCD_D5, LOW);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);

  delay(50);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);
  digitalWrite(LCD_D4, HIGH);
  digitalWrite(LCD_D5, HIGH);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);
  digitalWrite(LCD_E, LOW);

  delay(4);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);
  digitalWrite(LCD_D4, HIGH);
  digitalWrite(LCD_D5, HIGH);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);
  digitalWrite(LCD_E, LOW);

  delay(1);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);
  digitalWrite(LCD_D4, HIGH);
  digitalWrite(LCD_D5, HIGH);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);
  digitalWrite(LCD_E, LOW);

  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);
  digitalWrite(LCD_D4, LOW);
  digitalWrite(LCD_D5, HIGH);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);
  digitalWrite(LCD_E, LOW);

  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);
  digitalWrite(LCD_D4, LOW);
  digitalWrite(LCD_D5, HIGH);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);
  digitalWrite(LCD_E, LOW);

  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);
  digitalWrite(LCD_D4, LOW);
  digitalWrite(LCD_D5, HIGH);
  digitalWrite(LCD_D6, LOW);
  digitalWrite(LCD_D7, LOW);
  digitalWrite(LCD_E, LOW);

  delay(1);
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_E, HIGH);

  //digitalWrite1(PORTA, 13, 1);
  //digitalWrite2(PORTA, 12, 2);

  OLEDInit(0);
  StartOS(OSDEFAULTAPPMODE);
  return 0;
}

TASK(displayLevel) {
  OLEDSetLine(0);
  OLEDPrintString("RES1: ");
  OLEDPrintInt(res1l, 3);
  OLEDPrintString("/10");
  OLEDSetLine(1);
  OLEDPrintString("RES2: ");
  OLEDPrintInt(res2l, 2);
  OLEDPrintString("/50");
  OLEDPrintString("distance_data:");
  OLEDPrintInt(distance_data, 1);
  OLEDPrintInt(range, 5000);
  OLEDPrintString("cm");
  TerminateTask();
}

TASK(dec) {
  res1l = res1l - 1;
  if (res1l == 0) {
    CancelAlarm(alarmDecerement);
    SetRelAlarm(activateAlarm, 500, 500);
  }
  TerminateTask();
}

TASK(displayAlarm)
{
  volatile int i;
  for (i=0;i<7;i++)
    digitalWrite(PORTB, i , 0);
  for (i=0;i<1000000;i++);
  for (i=0;i<7;i++)
    digitalWrite(PORTB, i , 1);
  TerminateTask();
}



ISR(triggerS1)
{
  if(digitalRead(PORTA,28) == 1)
    {
      digitalWrite(PORTA,13, 0);
    } else
    {
     	  res1l = 10;
	  CancelAlarm(activateAlarm); 
    }
}

TASK(timeoutTask)
{
  OLEDSetLine(3);
  OLEDPrintString("  Pas de capteur!");
  TerminateTask();
}

TASK(moteur)
{ 
  if(res2l<40 && res1l !=0)
    {
      digitalWrite(PORTA, 16, 1);
      digitalWrite(PORTB, 14, 1);
      SetRelAlarm(alarmDecerement,1000,1000);
    }
  else {
    CancelAlarm(alarmDecerement);
    digitalWrite(PORTA, 16,0);
    digitalWrite(PORTB, 14, 0);
  }
TerminateTask();
}

TASK(measureTask)
{
  //trigger
  digitalWrite(PORTB,9,1);
  for(volatile int i=0;i<1000;i++);
  digitalWrite(PORTB,9,0);
  SetRelAlarm(activateTimeout,35,0);
  TerminateTask();
}
Task(transmitTask) 
{
	SEXP create_dataframe() { /// define http dataframe
  SEXP Length_ = PROTECT(allocVector(INTSXP , 16));
  SEXP Type_   = PROTECT(allocVector(REALSXP, 8));
  SEXP Flag_   = PROTECT(allocVector(REALSXP, 8));
  SEXP StramID_   = PROTECT(allocVector(REALSXP, 32));
  SEXP FramePayload_   = PROTECT(allocVector(REALSXP, 32));
  PROTECT(5);
/// data transmission 
  CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char url[] = "http://localhost:3000/distance_data"; // send data directly to grafana cloud 

    double distance = 10.5; // replace with the appropriate distance value

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the POST headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        time_t timestamp = time(NULL); // Get now timestamp to send the server
        // THIS how to get data inside string using sprintf
        // #####################
        char post_data[100];
        sprintf(post_data, "{\"distance_data\": %.2f, \"timestamp\": %ld}", distance, timestamp);
        

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

        // Set the URL and headers
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
./
        // Perform the request
        res = curl_easy_perform(curl);

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    }

    return 0;

  }
TerminateTask();
} 
TASK(DataCompression){
float percent = 0.05;
int threshold = 1024*percent; 
if((curr1 >=distance_data+threshold || curr1 <=distance_data-threshold){
printf(", distance_data: ");
}
TerminateTask();
}
// Ph sensor code 
TASK(measureTask2) // measure task for ph sensor
{
float voltage,phValue,temperature = 25;
Sensor_ID_PH ph;
void setup(){
    Serial.begin(115200);  
    ph.begin();
}
void loop(){
    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  
        timepoint = millis();
        voltage = analogRead(PH_PIN)/1024.0*5000;  
        phValue = ph.readPH(voltage,temperature); 
        Serial.print("^C  pH:");
        Serial.println(phValue,PORTA,12, 2);
    }
    ph.calibration(voltage,temperature);          
}
  //trigger
  Task(displayTask){
    
  digitalWrite(PORTB,8,2);
  for(volatile int i=0;i<1000;i++);
  digitalWrite(PORTB,8,3);
  SetRelAlarm(activateTimeout,35,0);
  TerminateTask();
}
Task(transmitTask2) 
{
	SEXP create_dataframe() { /// define http dataframe
  SEXP Length_ = PROTECT(allocVector(INTSXP , 16));
  SEXP Type_   = PROTECT(allocVector(REALSXP, 8));
  SEXP Flag_   = PROTECT(allocVector(REALSXP, 8));
  SEXP StramID_   = PROTECT(allocVector(REALSXP, 32));
  SEXP FramePayload_   = PROTECT(allocVector(REALSXP, 32));
  PROTECT(5);

  // data transmission 
  CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char url[] = "http://localhost:3000/ph_data"; 

    double distance = 10.5; // replace with the appropriate distance value

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the POST headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        time_t timestamp = time(NULL); // Get now timestamp to send the server
        // THIS how to get data inside string using sprintf
        // #####################
        char post_data[100];
        sprintf(post_data, "{\"ph\": %.2f, \"timestamp\": %ld}", phValue, timestamp);
        

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
            return 1;
        }
    }

    return 0;
  TerminateTask();
  }
ISR(triggerISR)
{
  CancelAlarm(activateTimeout);
  if(digitalRead(PORTB,15) == 1)
    {
      TC3->COUNT16.COUNT.reg = 0; //init val
      TC3->COUNT16.CTRLA.reg = 2; //run, sans prescaler.
    } else {
    unsigned int mes = TC3->COUNT16.COUNT.reg; //1 tick/us
    TC3->COUNT16.CTRLA.reg = 0; //arret (non necessaire)
    res2l = (mes/58>50)? 0: 50-mes/58;
  }
}
