#ifndef __FLV_H__
#define __FLV_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "flv_format.h"


/* for debug */
#define RED 		"\e[31m"
#define GREEN		"\e[32m"
#define COLOR_END	"\e[0m"

#ifdef ENABLE_DEBUG
	#define DEBUG(fmt, args...) 	printf(fmt, ##args)
#else
	#define DEBUG(fmt, args...)
#endif


#define MAX_FLV_BUF_SIZE 	1*1024*1024 	/* 1 MBytes */



/************************************************************************
 * function describe: Mux h264 and aac to FLV file.
 * params:
 *   [h264FileName]: h264 file.(in)
 *   [vFps]: video fps.(in)
 *   [aacFileName]: aac file file.(in)
 *   [flvFileName]: flv file.(in)
 * return: 0-success  other-error
 ************************************************************************/
int flv_mux_h264_aac(char *h264FileName, uint32_t vFps, char *aacFileName, char *flvFileName);



#endif /* __FLV_H__ */

