 
//the READY tasks state definition conflicts with registers definition (in sam.h)
#undef READY
#include "sam.h"
#include <pinAccess.h>
#include <../../../board/iutNantes/include/oled.h>
#include <../../../board/iutNantes/include/timer.h>
#include <eic.h>
#include <curl/curl.h>
#include <time.h>
#include "sensor_id_PH.h" // define the sensor's ID
#include <EEPROM.h>
#define PH_PIN A1

#include <stdio.h> //printf

//distance en cm.
volatile unsigned int res1l= 10;
volatile unsigned int res2l= 0;

int main(void)
{
  
  EICInitClock(F1MHZ);
  TCinitClock(F1MHZ,3);

  //1 pulse/us
  OLEDInit(0);	
  for (int i=0;i<8;i++)
    {
      pinMode(PORTB,i,OUTPUT);
      digitalWrite(PORTB,i, 1);      
    }
  
  pinMode(PORTA,13,OUTPUT);       // Push button ld 
  pinMode(PORTA, 28, INPUT);     // BT1 push button 
  pinMode(PORTA, 16, OUTPUT);    // IN Moteur 
  pinMode(PORTB, 14, OUTPUT);    // Enable  Moteur 
  pinMode(PORTB,9,OUTPUT);	//TRIGGER pin
  pinMode(PORTB,15,INPUT);	//ECHO pin
  pinMode(PORTB,12,PH_PIN A1);	//A1 pin for ph sensor (input)
  
  digitalWrite1(PORTA,13, 1);   
  digitalWrite2(PORTA,12, 2); 

  /* Sensor event (36 bytes) */
/** struct sensor_event_s is used to provide a single sensor event in a common format. */
typedef struct // for ultrasonic sensor
{
    int32_t version;
    int32_t sensor01;///sensor_id
    int32_t Ultrasonic; /// type
    int32_t reserved0;
    int32_t timestamp;
    union
    {
        float           data[4];
        sensors_vec_t   acceleration;
        sensors_vec_t   orientation;
        sensors_vec_t   gyro;
        float           distance; // functionality of sensor
    };
} sensors_event_t;
typedef struct // for ultrasonic sensor
{
    int32_t V2; // sensor_version
    int32_t sensor02;///sensor_id
    int32_t PH_Sensor; /// type
    int32_t reserved1;
    int32_t timestamp1;
    union
    {
        float           data[4];
        sensors_vec_t   acceleration;
        sensors_vec_t   orientation;
        sensors_vec_t   gyro;
        float           PH_Level; // functionality of sensor
    };
} sensors_event_t;
  OLEDInit(0);
  StartOS(OSDEFAULTAPPMODE);
  return 0;
}

TASK(displayLevel)
{
  OLEDSetLine(0);
  OLEDPrintString("RES1: ");
  OLEDPrintInt(res1l,3);
  OLEDPrintString("/10");
  OLEDSetLine(1);
  OLEDPrintString("RES2: ");
  OLEDPrintInt(res2l,2);
  OLEDPrintString("/50");
  OLEDPrintString("distance");
  OLEDPrintInt(range,5000);
  OLEDPrintString("cm");
  TerminateTask();
}


TASK(dec)
{
  res1l=res1l-1;
  if (res1l == 0)
    {
      CancelAlarm(alarmDecerement);
      SetRelAlarm(activateAlarm, 500,500);
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
    char url[] = "http://localhost:8000/distance_data"; 

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
        sprintf(post_data, "{\"distance\": %.2f, \"timestamp\": %ld}", distance, timestamp);
        // #####################
        /*
        In case of wanting to send the ph data use this line instead
        sprintf(post_data, "{\"ph\": %.2f, \"timestamp\": %ld}", phValue, timestamp);
        */

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
  
} 
TASK(DataCompression){
  uint32_t lz77_compress (uint8_t *uncompressed_text, uint32_t uncompressed_size, uint8_t *compressed_text)
{
    uint8_t pointer_length = 0, temp_pointer_length;
    uint16_t pointer_pos = 0, temp_pointer_pos, output_pointer;
    uint32_t compressed_pointer = 0, output_size = 0, coding_pos = 0, output_lookahead_ref = 0, look_behind = 0, look_ahead = 0;
    
    *((uint32_t *) compressed_text) = uncompressed_size;
    compressed_pointer = output_size = 4;
    
    for(coding_pos = 0; coding_pos < uncompressed_size; ++coding_pos)
    {
        temp_pointer_pos = 1;
		while(temp_pointer_pos <= 4096 && temp_pointer_pos <= coding_pos){
            look_behind = coding_pos - temp_pointer_pos;
            look_ahead = coding_pos;
			temp_pointer_length = 0;
			while(uncompressed_text[look_ahead++] == uncompressed_text[look_behind++]){
				temp_pointer_length++;
				if(temp_pointer_length == 15)
                    break;
			}
			if(temp_pointer_length > pointer_length)
            {
                pointer_pos = temp_pointer_pos;
                pointer_length = temp_pointer_length;
            }
            temp_pointer_pos++;
		}
        coding_pos += pointer_length;
        if(pointer_length && (coding_pos == uncompressed_size))
        {
            output_pointer = (pointer_pos << 4) | (pointer_length - 1);
            output_lookahead_ref = coding_pos - 1;
        }
        else
        {
            output_pointer = (pointer_pos << 4) | pointer_length;
            output_lookahead_ref = coding_pos;
        }
        *((uint32_t *) (compressed_text + compressed_pointer)) = output_pointer;
        compressed_pointer += 2;
        *(compressed_text + compressed_pointer++) = *(uncompressed_text + output_lookahead_ref);
        output_size += 3;
    }
    
    return output_size;
}

uint32_t lz77_decompress (uint8_t *compressed_text, uint8_t *uncompressed_text)
{
    uint8_t pointer_length = 0;
    uint16_t input_pointer = 0, pointer_pos = 0;
    uint32_t compressed_pointer = 0, coding_pos = 0, pointer_offset = 0, uncompressed_size = 0;
    
    uncompressed_size = *((uint32_t *) compressed_text);
    compressed_pointer = 4;
    
    for(coding_pos = 0; coding_pos < uncompressed_size; ++coding_pos)
    {
        input_pointer = *((uint32_t *) (compressed_text + compressed_pointer));
        compressed_pointer += 2;
        pointer_pos = input_pointer >> 4;
        pointer_length = input_pointer & 15;
        if(pointer_pos)
            for(pointer_offset = coding_pos - pointer_pos; pointer_length > 0; --pointer_length)
                uncompressed_text[coding_pos++] = uncompressed_text[pointer_offset++];
        *(uncompressed_text + coding_pos) = *(compressed_text + compressed_pointer++);
    }
    
    return coding_pos;
  
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
    char url[] = "http://localhost:8000/distance_data"; 

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
