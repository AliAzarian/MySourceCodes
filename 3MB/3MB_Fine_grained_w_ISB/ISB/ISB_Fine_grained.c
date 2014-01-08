/*
* Port configuration:
* --------------------------------------------------
* Producer --fsl0--> (ISB-PORT 0) --fsl1--> Consumer
* Producer <--fsl3-- (ISB-PORT 1) <--fsl1-- Consumer
* --------------------------------------------------
**/
#include "mb_interface.h"
#include "memory_parameters.h"
#include "xparameters.h"
#include <MEMORY_PARAMETERS.H>
#define FLUID 0
/* --- VARIABLES --- */
int data, data1,data2,data22, hash1, hash2;
int index1, index2;
int i = 0;
int j;
int put = 1;
int get = 1;
int index_local[BUFFER_SIZE];
int data_local[BUFFER_SIZE];
char flag_local[BUFFER_SIZE];

int *data_ext = (int *) TMP1;
int *flag_ext = (int *) TMP2;

int sl; // store in local memory
int ll; // load from local memory
int se; // store in external memory
int le; // load from external memory
int miss_p, miss_c;



int main()
{
sl=0;
ll=0;
se=0;
le=0;
miss_p=0;miss_c=0;
    /* Initialize the Local memory */	 
    for (i = 0; i < BUFFER_SIZE; i++) 
		flag_local[i] = 0;  

	 for (i = 0; i < SIZE; i++) 
		flag_ext[i] = 0; 
   
    
    //int MASK_LENGTH = (int) ceil((log(BUFFER_SIZE))/log(2));
    int MASK_LENGTH = 4;
    int MASK = ((1 << MASK_LENGTH) - 1);
	 
    /* INIT THE PRODUCER TO START: */
    	 putfsl(0, 1);               // start with image 0
    
	 while (1)
    {
        if (put == 1)
        {
            getfsl(index1, 0);               		// get index from the producer
            if (index1 != 999999999)               // Check if it's not the last index of the producer
            {
                hash1 = index1 & MASK;
                if (flag_local[hash1] != 1)         // Check if the Buffer is Empty
                {
						  getfsl(data, 0);                // get data from the producer
                    index_local[hash1] = index1;    // Write index into local table
                    data_local[hash1] = data;       // Write data into local table                    
                    flag_local[hash1] = 1;          // Set the flag into local table
                    put = 1;
						  sl++;  //store local : producer
                }
                else //if (flag_ext[index1]==0)
                {
                    getfsl(data, 0);                // get data from the producer
						  data_ext[index1]=data;
                    flag_ext[index1]=1;
                    se++;                 
                }
            
            }
            else
					put = 0;
        }
        if (get == 1)
        {
            getfsl(index2, 1);               // Get the index requested from the Consumer
            if (index2 == 888888888)
            {  				      // If it's a finish signal from the consumer send it to the producer
                putfsl(index2, 1);
                put = 1;
            }
            else
            hash2 = index2 & MASK;
        }
			
        if (flag_local[hash2] == 1 && index_local[hash2] == index2)               // Check if the data exist in local table
        {
            ll++;
				data2 = data_local[hash2];               // Read the data from local table            
            putfsl(sl, 0);               			// Send data to the consumer      
            flag_local[hash2] = 0;               		// set the flag
            get = 1;
        }
        else if (index2 != 888888888 && flag_ext[index2]==1)
         { // here it was get=0;
				le++;
            data22=data_ext[index2];
				putfsl(sl, 0);
				flag_ext[index2]=0;
				get = 1;            
        }
        else // added for counting the number of miss,load and store
        {
            miss_c++;
            get=0;
        }
    }
    return (0);
}
