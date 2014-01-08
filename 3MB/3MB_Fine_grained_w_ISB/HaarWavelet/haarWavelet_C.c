//------- HEADERS -------
#include "xparameters.h"
#include "xtmrctr.h"
#include "xutil.h"
#include "mb_interface.h"
#include <memory_parameters.h>

//------- DEFINES -------
#define TMRCTR_DEVICE_ID  XPAR_XPS_TIMER_0_DEVICE_ID
#define TIMER_COUNTER_0     0


int i,j;
u32* buf2 = (u32 *) TMP2;
u32* out = (u32 *) OUT;

int start2, end2,k; 
u32 buf2_1,buf2_2;

int main(void) {
	//------- TIMER INIT -------
	XTmrCtr timer;
	XTmrCtr_Initialize(&timer, XPAR_XPS_TIMER_0_DEVICE_ID);
	XTmrCtr_Reset(&timer, TIMER_COUNTER_0);
	start2 = XTmrCtr_GetValue(&timer, TIMER_COUNTER_0);
	xil_printf("Begin (CPU):%d\r\n", start2);


	//------- START TIMER -------  
	XTmrCtr_Start(&timer, TIMER_COUNTER_0);


	//------- LOOP FOR IMAGES -------
	for (k=0; k < Number_of_Images; k++)  // loop for Images 
	{
			// Apply 1D Haar vertically
     for (i=0; i<LENGTH; i++) {
      //Haar1D.fwt(buf2[i], buf2[i], size, level);
    	for (j=0; j<WIDTH; j+=2) {
		
						
			putfsl(WIDTH*i+j, 0);
			getfsl(buf2_1, 1); 
			
			putfsl(WIDTH*i+j+1,0);
			getfsl(buf2_2, 1);

				
      	buf2[WIDTH*i+(j>>1)] = (short) ((buf2_1+buf2_2)>>1); 
      	buf2[WIDTH*i+(WIDTH>>1)+(j>>1)] = (short) (buf2_1-buf2_2); 
    	}

    }

		// Transpose the array
		for (i=0; i<WIDTH; i++)
			for (j=0; j<LENGTH; j++)
			out[WIDTH*i+j] = buf2[LENGTH*j+i];

		putfsl(888888888, 0);                      // Finish signal to the ISB		
	}

	//------- STOP TIMER -------  
	XTmrCtr_Stop(&timer, TIMER_COUNTER_0);
	end2 = XTmrCtr_GetValue(&timer, TIMER_COUNTER_0);
	//xil_printf("Latency = %x\n\r", end2-start2);
	xil_printf("value1 = %d\n\r", buf2_1);
	xil_printf("value2 = %d\n\r", buf2_2);
	xil_printf("---------------------------\n\r");
	return 0;

}
