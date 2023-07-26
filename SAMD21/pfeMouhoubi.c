#include "tpl_os.h"
//the READY tasks state definition conflicts with registers definition (in sam.h)
#undef READY
#include "sam.h"
#include "board/iutNantes/include/timer.h"
#include "board/iutNantes/include/oled.h"
#include "timer.h"
#include "serial.h"
#include "../../../../../../machines/simple/armv6m/samd21/utils/pinMux.h"
#include "../../../../../../machines/simple/armv6m/samd21/utils/eic.h"
#include <errno.h>
#include "serial.h"
#include "pinAccess.h"
#include "adc.h"
#include "unistd.h"
#include <stdio.h> 
#include <time.h> 
#include <curl/curl.h>
void int_to_str(uint16_t range);
//distance en cm.
volatile unsigned int range=0;
volatile unsigned int res2l=0;
volatile unsigned int rsl=0;
volatile unsigned int i=0;
volatile float T;

volatile unsigned  int valeur=0;
void int_to_str(uint16_t range);



int main(void)
{  

	ADCInit();
	pinMode(PORTA,9,OUTPUT);	//TRIGGER pin
	pinMode(PORTB,15,INPUT);	//ECHO pin
	pinMode(PORTB,6,OUTPUT);	 // Temp pin Out
	EICInitClock(F1MHZ);         //
	TCinitClock(F1MHZ,3); //1 pulse/us
	OLEDInit(0);
    SERIALInit(115200);
 	range = 0;
	StartOS(OSDEFAULTAPPMODE);

	CURLcode res = CURLE_OK;
    struct curl_slist *headers = NULL;
    char url[] = "http://10.3.2.77:8002/sensor_data"; 
}

 TASK (Measure_Task){
		T = (ADCRead(3)*15)/1023; 
 }

TASK(displayTask)
{
	OLEDSetLine(0);
	OLEDPrintString("distance ");
	OLEDPrintInt(range,3);
	OLEDPrintString("cm.");
	TerminateTask();
}

TASK(timeoutTask)
{
	OLEDSetLine(0);
	OLEDPrintInt(range,3);
	OLEDPrintString("  NO DATA!");
	TerminateTask();
}




TASK(measureTask)
{	
	 //trigger
  digitalWrite(PORTA,9,1);
  for(volatile int i=0;i<1000;i++);
  digitalWrite(PORTA,9,0);
  SetRelAlarm(activateTimeout,35,0);
  TerminateTask();
}

ISR(triggerISR)
{  						     


	  if(digitalRead(PORTB,15) == 1)
    {       
      TC3->COUNT16.COUNT.reg = 0; //init val
      TC3->COUNT16.CTRLA.reg =2; //run, sans prescaler.


    } else {
    unsigned int mes = TC3->COUNT16.COUNT.reg; //1 tick/us 
    range = mes/58;

	    TC3->COUNT16.CTRLA.reg = 0; //arret (non necessaire)
	CancelAlarm(activateTimeout);
}
}
// Data compression

  TASK (DataCompression) {
  int curr1=0; 
  int curr2=0; 
  float percent = 0.05;
  int threshold = 1024*percent; 
  if((curr1 >=range+threshold || curr1 <=range-threshold)){
  SERIALPutString(", Distance: ");
  }
   if((curr2 >=T+threshold || curr2 <=T-threshold)){
  SERIALPutString(", Temperature: ");
  }
  TerminateTask();
  return 0;
  }

TASK(Serial_Print){

   SERIALPutString("Temp_Data");

    int_to_str(T);
	    SERIALPutString("\n");


    for (volatile int  i=0;i<5000;i++){}; 

				SERIALPutString("Distance_Data"); 
				int_to_str(range);
				               SERIALPutString("\n");
                         for (volatile int  i=0;i<1000;i++){}; 
	 TerminateTask();
}

void int_to_str(uint16_t range)
{
    if (range == 0)
    {
        SERIALPutString("0");
        return;
    }

    char y[10];
    int n = 0;

    while (range != 0)
    {
        int digit = range % 10;
        range /= 10;
        y[n] = '0' + digit;
        n++;
    }

    for (int i = n - 1; i >= 0; i--)
    {
        SERIALPutchar(y[i]);
    }
}

        
TASK(Http_TransmissionTask){
	    curl = curl_easy_init();
    if (curl) {
        // Set the POST headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        time_t timestamp = time(NULL); // Get now timestamp to send the server
		
        char post_data[100];
        SERIALPutstrig(post_data, "{\"Distance_Data\": %.2f, \"timestamp\": %ld}", range, timestamp);
        SERIALPutstring(post_data, "{\"Temp_Data\": %.2f, \"timestamp\": %ld}", T, timestamp);
        

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


