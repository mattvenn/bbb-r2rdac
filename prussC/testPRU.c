/** based on programs written by Derek Molloy
licensed with GNU GPLv3 license */

volatile register unsigned int __R31, __R30;

unsigned int i;                 //sample counter
unsigned int j;
unsigned int samples = 0;	//number of samples
unsigned d = 0;
unsigned int delay = 1;
#define OFFSET 256
int main()
{
   //pointer to start of memory location
   unsigned int *p = 0x00000000;
   //read number of samples
   samples = p[0];

	while(1)
	{
		
		//for each sample
		//for( i=256; i<samples+256; i++)
		for( i=OFFSET; i<samples+OFFSET; i++)
		{
		      //unpack the byte values and write on the register
		      __R30 = (0xFF00 & p[i]) >> 8;
			d++;
			d++;

		      __R30 = (0x00FF & p[i]);
		      //delay
			d++;
			d++;
		}
		      __R30 = 0;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
			d++;
	}

   // Exiting the application - send the interrupt
   __R31 = 35;                      // PRUEVENT_0 on PRU0_R31_VEC_VALID
   __halt();                        // halt the PRU
}
