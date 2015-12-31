/* Generate a header file for a particular 
   single or double frequency */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define CLIP 32635
#define BIAS 0x84

/* Dial frequency tables */
typedef struct
{
char    chr;    /* character representation */
float   f1;     /* first freq */
float   f2;     /* second freq */
} ZAP_DIAL;
 
ZAP_DIAL dtmf_dial[] = {
{ '0',941.0,1336.0 },
{ '1',697.0,1209.0 },
{ '2',697.0,1336.0 },
{ '3',697.0,1477.0 },
{ '4',770.0,1209.0 },
{ '5',770.0,1336.0 },
{ '6',770.0,1477.0 },
{ '7',852.0,1209.0 },
{ '8',852.0,1336.0 },
{ '9',852.0,1477.0 },
{ '*',941.0,1209.0 },
{ '#',941.0,1477.0 },
{ 'A',697.0,1633.0 },
{ 'B',770.0,1633.0 },
{ 'C',852.0,1633.0 },
{ 'D',941.0,1633.0 },
{ 0,0,0 }
} ;
 
ZAP_DIAL mf_dial[] = {
{ '0',1300.0,1500.0 },
{ '1',700.0,900.0 },
{ '2',700.0,1100.0 },
{ '3',900.0,1100.0 },
{ '4',700.0,1300.0 },
{ '5',900.0,1300.0 },
{ '6',1100.0,1300.0 },
{ '7',700.0,1500.0 },
{ '8',900.0,1500.0 },
{ '9',1100.0,1500.0 },
{ '*',1100.0,1700.0 }, /* KP */
{ '#',1500.0,1700.0 }, /* ST */
{ 'A',900.0,1700.0 }, /* ST' */
{ 'B',1300.0,1700.0}, /* ST'' */
{ 'C',700.0,1700.0}, /* ST''' */
{ 0,0,0 }
} ;                                                                             

unsigned char
linear2ulaw(sample)
short sample; {
  static int exp_lut[256] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
                             4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
                             7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};
  int sign, exponent, mantissa;
  unsigned char ulawbyte;
 
  /* Get the sample into sign-magnitude. */
  sign = (sample >> 8) & 0x80;          /* set aside the sign */
  if (sign != 0) sample = -sample;              /* get magnitude */
  if (sample > CLIP) sample = CLIP;             /* clip the magnitude */
 
  /* Convert from 16 bit linear to ulaw. */
  sample = sample + BIAS;
  exponent = exp_lut[(sample >> 7) & 0xFF];
  mantissa = (sample >> (exponent + 3)) & 0x0F;
  ulawbyte = ~(sign | (exponent << 4) | mantissa);
#ifdef ZEROTRAP
  if (ulawbyte == 0) ulawbyte = 0x02;   /* optional CCITT trap */
#endif
 
  return(ulawbyte);
}                                                                                            

int process(char *label, ZAP_DIAL z[], float low_tone_level, float high_tone_level)
{
	char c;
	float gain;
	int fac1, init_v2_1, init_v3_1,
	    fac2, init_v2_2, init_v3_2;

	while(z->chr) {
		c = z->chr;
		if (c == '*')
			c = 's';
		if (c == '#')
			c ='p';
		/* Bring it down 6 dbm */
		gain = pow(10.0, (low_tone_level - 3.14) / 20.0) * 65536.0 / 2.0;
		fac1 = 2.0 * cos(2.0 * M_PI * (z->f1 / 8000.0)) * 32768.0;
		init_v2_1 = sin(-4.0 * M_PI * (z->f1 / 8000.0)) * gain;
		init_v3_1 = sin(-2.0 * M_PI * (z->f1 / 8000.0)) * gain;
		
		gain = pow(10.0, (high_tone_level - 3.14) / 20.0) * 65536.0 / 2.0;
		fac2 = 2.0 * cos(2.0 * M_PI * (z->f2 / 8000.0)) * 32768.0;
		init_v2_2 = sin(-4.0 * M_PI * (z->f2 / 8000.0)) * gain;
		init_v3_2 = sin(-2.0 * M_PI * (z->f2 / 8000.0)) * gain;

		printf("\t /* %s_%c */ { %d, %d, %d, %d, %d, %d, DEFAULT_DTMF_LENGTH, &%s_silence }, \n", label, c,
			fac1, init_v2_1, init_v3_1, 
			fac2, init_v2_2, init_v3_2,
			label);
		
		z++;
	}
	return 0;
}

/* The following are the levels for the low tones (LEVEL1) and the high tones
   (LEVEL2) of DTMF digits, in dBm0. If you need to adjust these levels, this
   is the place to do it */
#define LEVEL_DTMF_LOW      -10
#define LEVEL_DTMF_HIGH     -10

/* The following is the level for the tones in MF digits, in dBm0. If you
   need to adjust this level, this is the place to do it */
#define LEVEL_MF            -10

int main(int argc, char *argv[])
{
	printf("/* DTMF and MF tones used by the Tormenta Driver, in static tables.\n"
				   "   Generated automatically from gendigits.  Do not edit by hand.  */\n"); 
	printf("static struct zt_tone dtmf_tones[16] = {\n");
	process("dtmf", dtmf_dial, LEVEL_DTMF_LOW, LEVEL_DTMF_HIGH);
	printf("};\n\n");
	printf("static struct zt_tone mfv1_tones[15] = {\n");
	process("mfv1", mf_dial, LEVEL_MF, LEVEL_MF);
	printf("};\n\n");
	printf("/* END tones.h */\n");

	return 0;
}   
