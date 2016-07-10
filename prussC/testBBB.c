/* based on programs written by Derek Molloy
licensed with GNU GPLv3 license */

#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

const int offset = 256;
#define PRU_NUM 	0
static void *pru0DataMemory;
static unsigned int *pru0DataMemory_int;
int i;
#include <string.h>     /* strcat */

const char *byte_to_binary(int x)
{
    static char b[17];
    b[0] = '\0';

    int z;
    for (z = 32768; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

int main (void)
{
   int n, ret;
   if(getuid()!=0){
      printf("You must run this program as root. Exiting.\n");
      exit(EXIT_FAILURE);
   }
   /* Initialize structure used by prussdrv_pruintc_intc   */
   /* PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h */
   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

   /* Allocate and initialize memory */
   prussdrv_init ();
   ret = prussdrv_open (PRU_EVTOUT_0);
   if(ret){
      printf("Failed to open the PRU-ICSS, have you loaded the overlay?");
      exit(EXIT_FAILURE);
   }

//pack the byte values into int data structure for transfer
   unsigned int numbersamples = 1024;  //number of samples
   int waveform[numbersamples/2];
	int j = 0;
   for (i=0; i<numbersamples/2; i++){
      waveform[i] = (char)j;
	j++;
      waveform[i] |=  (char)(j) << 8;
	j++;
//      printf("%s\n", byte_to_binary(waveform[i]));
   }

   // place the samples in memory
   prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0DataMemory);
   pru0DataMemory_int = (unsigned int *) pru0DataMemory;
   *(pru0DataMemory_int) = numbersamples/2;
   // copy the waveform data into PRU memory
   for (i=0; i<numbersamples/2; i++){
      *(pru0DataMemory_int+offset+i) = waveform[i];
   }

   /* Map PRU's INTC */
   prussdrv_pruintc_init(&pruss_intc_initdata);

   /* Load the memory data file -- what is this? */
  // prussdrv_load_datafile(PRU_NUM, "./data.bin");

   /* Load and execute binary on PRU */
   prussdrv_exec_program (PRU_NUM, "./text.bin");

   /* Wait for event completion from PRU */
   n = prussdrv_pru_wait_event (PRU_EVTOUT_0);  // This assumes the PRU generates an interrupt
                                                    // connected to event out 0 immediately before halting
   printf("PRU program completed, event number %d.\n", n);

   /* Disable PRU and close memory mappings */
   prussdrv_pru_disable(PRU_NUM);
   prussdrv_exit ();
   return(EXIT_SUCCESS);
}
