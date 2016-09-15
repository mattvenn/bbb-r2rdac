/* based on programs written by Derek Molloy
licensed with GNU GPLv3 license */

#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <string.h>     /* strcat */

#define offset 256 // have to avoid the first X bytes of the PRU memory or weird things happen - don't know why.
#define end_limit 80 // can't seem to use the whole 8kb, avoid this many samples at the end to ensure waveform is complete
#define maxsamples 8000 - offset - 80 // max number of samples to loop

#define PRU_NUM		0
static void *pru0DataMemory;
static unsigned int *pru0DataMemory_int;
int i;
unsigned int numbersamples = 0;  //number of samples
int samples[maxsamples];
int waveform[maxsamples/2];

// for debugging
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

// read the datafile and pack 2 bytes into an int
void read_data (const char* file_name)
{
  FILE* file = fopen (file_name, "r");
  int i = 0;
  int j = 0;
  while (!feof (file))
  {  
    fscanf (file, "%d\n", &i);
    if(i > 255)
    {
      printf("max value for data is 255\n");
      exit(1);
    }
    if(numbersamples > maxsamples)
    {
      printf("too many samples in data file, max is %d\n", maxsamples);
      exit(1);
    }
    samples[j++] = i;
    numbersamples ++;
  }
  fclose (file);        
  if(numbersamples % 2 != 0)
  {
    printf("must be an even number of samples\n");
    exit(1);
  }
  printf("loaded %d samples\n", numbersamples);
	
  j = 0;
  for (i=0; i<numbersamples/2; i++)
  {
    waveform[i] = (char)samples[j];
    j++;
    waveform[i] |=  (char)samples[j] << 8;
    j++;
    //uncomment this to see the 8 bit data packed into 16 bits.
    //printf("%s\n", byte_to_binary(waveform[i]));
  }
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
  if(ret)
  {
     printf("Failed to open the PRU-ICSS, have you loaded the overlay?");
     exit(EXIT_FAILURE);
  }

  //read the datafile into the waveform array
  read_data("data.txt");

  // place the samples in memory
  prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0DataMemory);
  pru0DataMemory_int = (unsigned int *) pru0DataMemory;
  *(pru0DataMemory_int) = numbersamples/2;

  // copy the waveform data into PRU memory
  for (i=0; i<numbersamples/2; i++)
  {
    *(pru0DataMemory_int+offset+i) = waveform[i];
  }

  /* Map PRU's INTC */
  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* Load and execute binary on PRU */
  prussdrv_exec_program (PRU_NUM, "./dac.bin");

  /* Wait for event completion from PRU */
  n = prussdrv_pru_wait_event (PRU_EVTOUT_0);  // This assumes the PRU generates an interrupt
                                                   // connected to event out 0 immediately before halting
  printf("PRU program completed, event number %d.\n", n);

  /* Disable PRU and close memory mappings */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit ();
  return(EXIT_SUCCESS);
}
