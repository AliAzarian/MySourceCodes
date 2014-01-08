//------- HEADERS -------
#include "xutil.h"
#include "xenv.h"
#include "mb_interface.h"
#include <memory_parameters.h>

//------- VARIABLES -------
u32* buf1 = (u32 *) TMP1;
u32* in = (u32 *) IN;
int i, j,m,id1;  

int main(void) {
	
	//------- LOOP FOR IMAGES -------
	for (m=0; m < Number_of_Images; m++)  // loop for Images 
	{
			getfsl(id1,0);	                       // Start Signal					
// Apply 1D Haar horizontally
    for ( i=0; i<WIDTH; i++) {
      //Haar1D.fwt(in[i], buf1[i], size, level);
    	for (j=0; j<LENGTH; j+=2) {
      		buf1[LENGTH*i+(j>>1)] = (short) ((in[LENGTH*i+j]+in[LENGTH*i+j+1])>>1);
      		buf1[LENGTH*i+(LENGTH>>1)+(j>>1)] = (short) (in[LENGTH*i+j]-in[LENGTH*i+j+1]);
    	}
    }

    // Transpose the array
    for (i=0; i<WIDTH; i++)
      for (j=0; j<LENGTH; j++) {        
			putfsl(WIDTH*j+i, 0);
			putfsl(buf1[LENGTH*j+i], 0);
			
		  }
		putfsl(999999999,0);					 // Stop Signal
	}
	return 0;
}


