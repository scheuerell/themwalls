/*
  RFSniffer

  Usage: ./rfpbrx [<pulseLength>]
  [] = optional

  Hacked from http://code.google.com/p/rc-switch/
  by @justy to provide a handy RF code sniffer
*/

#include "rfpbrx_api.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) 
{
	
	if ( rfpb_init () == 0) 
	{
		printf("init failed\n");
		exit(0);
	}
    
	while(1) 
	{
		usleep(100000);
		if ( rfpb_test() == 1 ) 
		{
			printf("active\n");
		}
	}
  exit(0);
}

