#include "xtmrctr.h"
#include "xparameters.h"
#include "xbasic_types.h"
#include <memory_parameters.h>
#include "xutil.h"
#include "mb_interface.h"

#define TMRCTR_DEVICE_ID  XPAR_XPS_TIMER_0_DEVICE_ID  //XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_COUNTER_0     0
#define chksum

#define N 8						// fixed
#define M N*N
#define num_fdcts 7500 // NTSC: 720x480 => 90 x 60 blocks of 8x8; if 1 then 8x8 DCT
// 1024 * 768 => 128 x 90 blocks of 8x8 therefore num_fdcts = 128 * 90


u32* dct_o = (u32 *) OUT;
int k,start2,end2;

int main(void) {	
	print("-------FDCT using two MB with FSL---------------\r\n "); 


	
	//void fdct_8x8(short dct_data, unsigned num_fdcts)  
	
	/* -------------------------------------------------------- /    
	/  Set up the cosine coefficients c0..c7.                  /    
	/ -------------------------------------------------------- */    
	const unsigned short c1 = 0x2C62, c3 = 0x25A0;                    
	const unsigned short c5 = 0x1924, c7 = 0x08D4;                    
	const unsigned short c0 = 0xB505, c2 = 0x29CF;                    
	const unsigned short c6 = 0x1151;                                 
	
	/* -------------------------------------------------------- /    
	/  Intermediate calculations.                              /    
	/ -------------------------------------------------------- */    
	int f0, f1, f2, f3,f4, f5, f6, f7;       // Spatial domain samples.       
	int   g0, g1, h0, h1,p0, p1;               // Even-half intermediate.     
	short r0, r1;               // Even-half intermediate.     
	int   P0, P1, R0, R1;       // Even-half intermediate.         
	short g2, g3, h2, h3;       // Odd-half intermediate.           
	short q0a,s0a,q0, q1,s0, s1;               // Odd-half intermediate.          
	short Q0, Q1, S0, S1;       // Odd-half intermediate.       
	int   F0, F1, F2, F3, F4, F5, F6, F7;       // Freq. domain results. 
	int   F0r,F1r,F2r,F3r,F4r,F5r,F6r,F7r;      // Rounded, truncated results.   

	/* -------------------------------------------------------- /    
	/  Input and output pointers, loop control.                /    
	/ -------------------------------------------------------- */    
	unsigned i, j, i_1;   


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


	
	/* -------------------------------------------------------- /    
	/  Perform Horizontal 1-D FDCT on each 8x8 block.          /    
	/ -------------------------------------------------------- */    
	//dct_io_ptr = dct_data;    
	i_1 = 0;
	for (i = 0; i < N*num_fdcts; i++) {  //N*num_fdcts                                                                                     
		// ---------------------------------------------------- /    
		//  Load the spatial-domain samples.                    /    
		// ---------------------------------------------------- /  
		
		putfsl(0+i_1, 0);	
		getfsl(f0, 1);

		putfsl(1+i_1, 0);	
		getfsl(f1, 1);	
		
		putfsl(2+i_1, 0);	
		getfsl(f2, 1);	

		putfsl(3+i_1, 0);	
		getfsl(f3, 1);
		
		putfsl(4+i_1,0);
		getfsl(f4, 1);
		
		putfsl(5+i_1,0);
		getfsl(f5, 1);
		
		putfsl(6+i_1,0);
		getfsl(f6, 1);
		
		putfsl(7+i_1,0);	
		getfsl(f7, 1);
	
		
		// ---------------------------------------------------- /    
		//  Stage 1:  Separate into even and odd halves.        /    
		// ---------------------------------------------------- /    
		g0 = f0 + f7;               h2 = f0 - f7;                     
		g1 = f1 + f6;               h3 = f1 - f6;                     
		h1 = f2 + f5;               g3 = f2 - f5;                     
		h0 = f3 + f4;               g2 = f3 - f4;                     
		
		// ---------------------------------------------------- /    
		//  Stage 2                                             /    
		// ---------------------------------------------------- /    
		p0 = g0 + h0;               r0 = g0 - h0;                     
		p1 = g1 + h1;               r1 = g1 - h1;                     
		q1 = g2;                    s1 = h2;                          
		
		s0a= h3 + g3;               q0a= h3 - g3;                     
		q0 = (q0a * c0 + 0x7FFF) >> 16;                               
		s0 = (s0a * c0 + 0x7FFF) >> 16;                               
		
		// ---------------------------------------------------- /    
		//  Stage 3                                             /    
		// ---------------------------------------------------- /    
		P0 = p0 + p1;               P1 = p0 - p1;                     
		R1 = c6 * r1 + c2 * r0;     R0 = c6 * r0 - c2 * r1;           
		
		Q1 = q1 + q0;               Q0 = q1 - q0;                     
		S1 = s1 + s0;               S0 = s1 - s0;                     
		
		// ---------------------------------------------------- /    
		//  Stage 4                                             /    
		// ---------------------------------------------------- /    
		F0 = P0;                    F4 = P1;                          
		F2 = R1;                    F6 = R0;                          
		
		F1 = c7 * Q1 + c1 * S1;     F7 = c7 * S1 - c1 * Q1;           
		F5 = c3 * Q0 + c5 * S0;     F3 = c3 * S0 - c5 * Q0;           
		
		// ---------------------------------------------------- /    
		//  Round and truncate values.                          /    
		//                                                      /    
		//  Note: F0 and F4 have different rounding since no    /    
		//  MPYs have been applied to either term.  Also, F0's  /    
		//  rounding is slightly different to offset the        /    
		//  truncation effects from the horizontal pass (which  /    
		//  does not round).                                    /    
		// ---------------------------------------------------- /    
	 F0r = (F0 + 0x0006) >>  3;                                    
    F1r = (F1 + 0x7FFF) >> 16;                                    
    F2r = (F2 + 0x7FFF) >> 16;                                    
    F3r = (F3 + 0x7FFF) >> 16;                                    
    F4r = (F4 + 0x0004) >>  3;                                    
    F5r = (F5 + 0x7FFF) >> 16;                                    
    F6r = (F6 + 0x7FFF) >> 16;                                    
    F7r = (F7 + 0x7FFF) >> 16;                                    
    
    // ---------------------------------------------------- /    
    //  Store the results                                   /    
    // ---------------------------------------------------- /    
    dct_o[0+i_1] = F0r;                                          
    dct_o[1+i_1] = F1r;                                          
    dct_o[2+i_1] = F2r;                                          
    dct_o[3+i_1] = F3r;                                          
    dct_o[4+i_1] = F4r;                                          
    dct_o[5+i_1] = F5r;                                          
    dct_o[6+i_1] = F6r;                                          
    dct_o[7+i_1] = F7r;              
		
		// ---------------------------------------------------- /    
		//  Update pointer to next FDCT row.                    /    
		// ---------------------------------------------------- /    
		//dct_io_ptr += 8;
		i_1 += 8;
	}                                                               
	putfsl(888888888, 0);                      // Finish signal to the ISB
}

  	
//------- STOP TIMER -------  
	XTmrCtr_Stop(&timer, TIMER_COUNTER_0);
   end2 = XTmrCtr_GetValue(&timer, TIMER_COUNTER_0);
	xil_printf("Latency = %x\n\r", end2-start2);
	xil_printf("value = %d\n\r", f0);
	xil_printf("value = %d\n\r", f1);
	xil_printf("value = %d\n\r", f2);
	xil_printf("value = %d\n\r", f3);
	xil_printf("value = %d\n\r", f4);
	xil_printf("value = %d\n\r", f5);
   xil_printf("value = %d\n\r", f6);
	xil_printf("value = %d\n\r", f7);
	xil_printf("END TASK ...\r\n");
	
	
	#ifdef chksum
				
		u32 diff = 0; u32 t1,t2;
		u32 sum=0;
		unsigned int checksum=0;
		u32 correct_sum=268682;
		u32 diff2=308078;
		u32 chksum2=1159240840;
		
	
		for(i=0; i<N*num_fdcts;i++) {
		checksum+=dct_o[i]*(i+1);
		}

	
		for (i = 0; i < N*num_fdcts; i+=2) {
			t1 = dct_o[i];
			sum += t1;
			t2 = dct_o[i+1];
			sum += t2;
			diff += t1-t2;
			}
		if (correct_sum != sum || diff != diff2 || checksum!=chksum2)
			xil_printf("ERROR sum: %d \n\r",sum);
		else
			xil_printf("CHECKSUM IS OK !\n\r"); 
		
	#endif

	
	return 0;
}

