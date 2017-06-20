#include <datatypes.h>
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */

#include <sw_timer.h>
#include<xbee_operations.h>


int main()
{
    sw_timer_init();
    
    XbeeOperationsInit();

    while(1)
    {
        //zwavemode_state_event_test();
    }
    
    sw_timer_term();
    return 0;
}


