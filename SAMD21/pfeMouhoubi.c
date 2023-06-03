#include "tpl_os.h"
//the READY tasks state definition conflicts with registers definition (in sam.h)
#undef READY
#include "sam.h"
#include <pinAccess.h>
#include <../../../board/iutNantes/include/oled.h>
#include <../../../board/iutNantes/include/timer.h>
#include <eic.h>
#include <R.h> 
#include <Rinternals.h>
#include <stdio.h> //printf
//#include "sensor_id_PH.h" // ph lib
#include <eeprom.h> // ph lib
#include <curl/curl.h>
#include <sys/socket.h>
//distance en cm.
volatile unsigned int range; 
 volatile unsigned int PH;

int main(void)
{
	pinMode(PORTB,9,OUTPUT);	//TRIGGER pin
	pinMode(PORTB,15,INPUT);	//ECHO pin
	pinMode(PORTB, 12, INPUT); //A1 pin for ph sensor (input)
	EICInitClock(F1MHZ);
	TCinitClock(F1MHZ,3); //1 pulse/us
	OLEDInit(0);
	range = 0;
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

TASK(displayTask)
{
	OLEDSetLine(0);
	OLEDPrintString("distance_data ");
	OLEDPrintInt(range,3);
	OLEDPrintString("cm.");
    OLEDPrintString("ph_data ");
	OLEDPrintInt(PH,3);
	OLEDPrintString("ph.");
	TerminateTask();
}
TASK(timeoutTask)
{
	OLEDSetLine(0);
	OLEDPrintInt(range,3);
	OLEDPrintString("  No Data!");
    OLEDPrintInt(PH,3);
	OLEDPrintString("  No Data!");
	TerminateTask();
}

TASK(measureTask)
{
	//trigger
	Sensor_ID_ultrasonic ul;
	digitalWrite(PORTB,9,1);
	for(volatile int i=0;i<1000;i++);
	digitalWrite(PORTB,9,0);
	
	//lancement alarme du timeout
	SetRelAlarm(activateTimeout,35,0);
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
          Serial.print("^C  PH:");
          Serial.println(phValue,PORTA,12, 2);
    	}
	 ph.calibration(voltage,temperature);     
	}     
}
TASK (TransmitTask) {

    SEXP create_dataframe() { /// define http dataframe
    SEXP Length_ = PROTECT(allocVector(INTSXP , 16));
    SEXP Type_   = PROTECT(allocVector(REALSXP, 8));
    SEXP Flag_   = PROTECT(allocVector(REALSXP, 8));
    SEXP StramID_   = PROTECT(allocVector(REALSXP, 32));
    SEXP FramePayload_   = PROTECT(allocVector(REALSXP, 32));
    PROTECT(5);
	}
  // data transmission 
  void loop(){
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char distance_url[] = "http://localhost:8080/distance_data";
    char ph_url[] = "http://localhost:8080/ph_data";

    // Distance Data
    double distance = 10.5;
    char distance_post_data[100];
    sprintf(distance_post_data, "{\"distance_data\": %.2f}", distance);

    // pH Data
    double ph = 7.2;  // Replace with the appropriate pH value
    char ph_post_data[100];
    sprintf(ph_post_data, "{\"ph_data\": %.2f}", ph);

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the POST headers
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Send Distance Data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, distance_post_data);
        curl_easy_setopt(curl, CURLOPT_URL, distance_url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed for distance_data: %s\n", curl_easy_strerror(res));
        }

        // Send pH Data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ph_post_data);
        curl_easy_setopt(curl, CURLOPT_URL, ph_url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed for ph_data: %s\n", curl_easy_strerror(res));
        }

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    

    return 0;
	}
  }


  TerminateTask();
} 

TASK (DataCompression) {
  int curr1=0; 
  float percent = 0.05;
  int threshold = 1024*percent; 
  if((curr1 >=distance_data+threshold || curr1 <=distance_data-threshold)){
  printf(", distance_data: ");
  }
  if((curr1 >=ph_data+threshold || curr1 <=ph_data-threshold)){
  printf(", ph_data: ");
  }
  TerminateTask();
  return 0;
  }

ISR(triggerISR)
{
	if(digitalRead(PORTB,15) == 1)
	{
		//front montant. Lancement timer TC3.
		TC3->COUNT16.COUNT.reg = 0; //init val
		TC3->COUNT16.CTRLA.reg = 2; //run, sans prescaler.
	} else {
		unsigned int mes = TC3->COUNT16.COUNT.reg; //1 tick/us
		range = mes/58; //conversion en cm.
		TC3->COUNT16.CTRLA.reg = 0; //arret (non necessaire)
	}
	CancelAlarm(activateTimeout);
}


