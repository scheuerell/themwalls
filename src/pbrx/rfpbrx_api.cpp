/*
  detect rf pushbutton with c wrapper functions
*/

#include "RCSwitch.h"
#include "rfpbrx_api.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>

#ifdef __cplusplus
extern "C" {
#endif

static RCSwitch *mySwitch = NULL;
 
void lazyAAA() {
    if (mySwitch == NULL) {
        mySwitch = new RCSwitch();
    }
}

int rfpb_init () { 
	lazyAAA();
     if(wiringPiSetup() == -1) {
       printf("wiringPiSetup failed, exiting...");
       return 0;
     }
     //mySwitch = RCSwitch();
     mySwitch->enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2
     mySwitch->setProtocol(PROTOCOL, PULSE_WIDTH); 
     return 1;  
} 
    
int rfpb_test () {
	int status = 0;
	if (mySwitch->available()) 
	{
		if ( mySwitch->getReceivedValue() == VALUE ) 
		{
			status = 1;
		}
	}
	mySwitch->resetAvailable();
	return status;
}

#ifdef __cplusplus
}
#endif
