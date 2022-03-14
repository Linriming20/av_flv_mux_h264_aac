#include <stdio.h>

#include "flv.h"



int main(int argc, char *argv[])
{
	if(argc == 1)
	{
		printf("Usage: \n"
			   "   %s avfile/test1_856x480_24fps.h264 24 avfile/test1_44100_stereo.aac out1.flv\n"
			   "   %s avfile/test2_960x544_25fps.h264 25 avfile/test2_44100_mono.aac   out2.flv\n",
			   argv[0], argv[0]);
		return -1;
	}

	flv_mux_h264_aac(argv[1], atoi(argv[2]), argv[3], argv[4]);

	printf("\e[32mSuccess!\n\e[0m");

	return 0;
}
