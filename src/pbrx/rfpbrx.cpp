/*
  RFSniffer

  Usage: ./rfpbrx [<pulseLength>]
  [] = optional

  Hacked from http://code.google.com/p/rc-switch/
  by @justy to provide a handy RF code sniffer
*/

#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
 
#define PROTOCOL	1    
#define PULSE_WIDTH 213
#define BITS		24
#define VALUE		10732712
RCSwitch mySwitch;
 


int main(int argc, char *argv[]) {
  
     // This pin is not the first pin on the RPi GPIO header!
     // Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     // for more information.
     int PIN = 2;
     
     if(wiringPiSetup() == -1) {
       printf("wiringPiSetup failed, exiting...");
       return 0;
     }

     //int pulseLength = 0;
     //if (argv[1] != NULL) pulseLength = atoi(argv[1]);

     mySwitch = RCSwitch();
     //if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
     mySwitch.enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2
     mySwitch.setProtocol(PROTOCOL, PULSE_WIDTH);
     
    
     while(1) {
  
      usleep(100000);
      if (mySwitch.available()) {
		//if ( mySwitch.getReceivedValue() == VALUE ) {
			
        int value = mySwitch.getReceivedValue();
    
        if (value == 0) {
          printf("Unknown encoding\n");
        } else {    
   
          printf("Value: %i\n", mySwitch.getReceivedValue() );
          printf("Bits: %i\n", mySwitch.getReceivedBitlength());
          printf("Pulse Width: %i\n", mySwitch.getReceivedDelay());
          printf("Protocol: %i\n", mySwitch.getReceivedProtocol());
          printf("Raw: %i\n", mySwitch.getReceivedRawdata());
        }
    
        mySwitch.resetAvailable();
    
      }
      
  
  }

  exit(0);


}

