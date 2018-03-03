/*
  detect rf pushbutton with c wrapper functions
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define PIN			2
#define PROTOCOL	1    
#define PULSE_WIDTH 213
#define BITS		24
#define VALUE		10732712

#ifndef AAA_C_CONNECTOR_H 
#define AAA_C_CONNECTOR_H 

#ifdef __cplusplus
extern "C" {
#endif

int rfpb_init ();
int rfpb_test ();

#ifdef __cplusplus
}
#endif


#endif
