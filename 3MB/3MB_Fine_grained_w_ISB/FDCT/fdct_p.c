//------- HEADERS -------
#include "xutil.h"
#include "xenv.h"
#include "mb_interface.h"
#include <memory_parameters.h>

//------- DEFINES -------
#define chksum 
#define SW
#define N 8 // fixed
#define M N*N
#define num_fdcts 7500 // NTSC: 720x480 => 90 x 60 blocks of 8x8; if 1 then 8x8 DCT
								// 1024 * 768 => 128 x 90 blocks of 8x8 therefore num_fdcts = 128 * 90

//------- VARIABLES -------
u32 * dct_io_ptr = (u32 *) IN;

int m,id1;
int main (void) 
{
   
//------- LOOP FOR IMAGES -------
	for (m=0; m < Number_of_Images; m++)  // loop for Images 
	{
	getfsl(id1,0);	                       // Start Signal			
	
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
  short f0, f1, f2, f3,f4, f5, f6, f7;       // Spatial domain samples.       
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
        
  
  #ifdef chksum
/* initilize the input image */
 for (i = 0; i < num_fdcts*M; i++) 
    dct_io_ptr[i] = i;   
#endif	

 
		 
  /* -------------------------------------------------------- /    
     /  Outer vertical loop -- Process each 8x8 block.          /    
     / -------------------------------------------------------- */    
  //dct_io_ptr = dct_data;    
  i_1 = 0;
  for (i = 0; i < num_fdcts; i++) {      
 
    /* ---------------------------------------------------- /    
       /  Perform Vert 1-D FDCT on columns within each block. /    
       / ---------------------------------------------------- */    
    for (j = 0; j < N; j++) {                                                             
      /* ------------------------------------------------ /    
	 /  Load the spatial-domain samples.                /    
	 / ------------------------------------------------ */    
      f0 = dct_io_ptr[ 0+i_1];                                      
      f1 = dct_io_ptr[ 8+i_1];                                      
      f2 = dct_io_ptr[16+i_1];                                      
      f3 = dct_io_ptr[24+i_1];                                      
      f4 = dct_io_ptr[32+i_1];                                      
      f5 = dct_io_ptr[40+i_1];                                      
      f6 = dct_io_ptr[48+i_1];                                      
      f7 = dct_io_ptr[56+i_1];                                      
                                                                           
      /* ------------------------------------------------ /    
	 /  Stage 1:  Separate into even and odd halves.    /    
	 / ------------------------------------------------ */    
      g0 = f0 + f7;               h2 = f0 - f7;                 
      g1 = f1 + f6;               h3 = f1 - f6;                 
      h1 = f2 + f5;               g3 = f2 - f5;                 
      h0 = f3 + f4;               g2 = f3 - f4;                 
      
      /* ------------------------------------------------ /    
	 /  Stage 2                                         /    
	 / ------------------------------------------------ */    
      p0 = g0 + h0;               r0 = g0 - h0;                 
      p1 = g1 + h1;               r1 = g1 - h1;                 
      q1 = g2;                    s1 = h2;                      
      
      s0a= h3 + g3;               q0a= h3 - g3;                   
      s0 = (s0a * c0 + 0x7FFF) >> 16;                           
      q0 = (q0a * c0 + 0x7FFF) >> 16;                           
                                                                           
      /* ------------------------------------------------ /    
	 /  Stage 3                                         /    
	 / ------------------------------------------------ */    
      P0 = p0 + p1;             P1 = p0 - p1;                 
      R1 = c6 * r1 + c2 * r0;     R0 = c6 * r0 - c2 * r1;       
      
      Q1 = q1 + q0;               Q0 = q1 - q0;                 
      S1 = s1 + s0;               S0 = s1 - s0;                 
      
      /* ------------------------------------------------ /    
	 /  Stage 4                                         /    
	 / ------------------------------------------------ */    
      F0 = P0;                    F4 = P1;                      
      F2 = R1;                    F6 = R0;                      
      
      F1 = c7 * Q1 + c1 * S1;     F7 = c7 * S1 - c1 * Q1;       
      F5 = c3 * Q0 + c5 * S0;     F3 = c3 * S0 - c5 * Q0;        
      
      /* ------------------------------------------------ /    
	 /  Store the frequency domain results.             /    
	 / ------------------------------------------------ */    
	
		
	 putfsl(0+i_1, 0);
	putfsl(F0, 0);
	
	putfsl(8+i_1, 0);
	putfsl(F1 >> 13, 0);
	
	putfsl(16+i_1, 0);
	putfsl(F2 >> 13, 0);
	
	putfsl(24+i_1, 0);
	putfsl(F3 >> 13, 0);
	
	putfsl(32+i_1, 0);
	putfsl(F4, 0);

	putfsl(40+i_1, 0);
	putfsl(F5 >> 13, 0);

	putfsl(48+i_1, 0);
	putfsl(F6 >> 13, 0);

	putfsl(56+i_1, 0);
	putfsl(F7 >> 13, 0);

	i_1++;                                            
    }                                                             
    /* ---------------------------------------------------- /    
    /  Update pointer to next 8x8 FDCT block.              /    
   / ---------------------------------------------------- */    
   i_1 += 56;                                    
  }
 putfsl(999999999,0);					 // Stop Signal
}  
 	
  
   return 0;
} 
