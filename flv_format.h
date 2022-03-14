/***************************************************************
 * describe: Flv file format description(Mainly for H.264 & AAC)
 * author: linriming
 * e-mail: linriming20@163.com
 ***************************************************************/

#ifndef __FLV_FORMAT_H__
#define __FLV_FORMAT_H__


#include <stdint.h>


#define AUDIO_SUPPORT(x)    (x << 2)    /* bit[2] in flvheader's type_flag */
#define VIDEO_SUPPORT(x)    (x << 0)    /* bit[0] in flvheader's type_flag */


#define SIZE_FLV_HEADER         sizeof(struct flvHeader)    /* 9 Bytes */
#define SIZE_FLV_TAG_HEADER     sizeof(struct flvTagHeader) /* 11 Bytes */
#define SIZE_PREVIOUS_TAG_SIZE  sizeof(uint32_t)            /* 4 Bytes */


/* FLV tag type */
typedef enum{
	FLVTAG_TYPE_AUDIO  = 0x08,
	FLVTAG_TYPE_VIDEO  = 0x09,
	FLVTAG_TYPE_SCRIPT = 0x12,
}flvTagType;


/* AMF data type in <Script Tag> */
typedef enum{
    AMF_DATA_TYPE_NUMBER      = 0x00,
    AMF_DATA_TYPE_BOOL        = 0x01,
    AMF_DATA_TYPE_STRING      = 0x02,
    AMF_DATA_TYPE_OBJECT      = 0x03,
    AMF_DATA_TYPE_NULL        = 0x05,
    AMF_DATA_TYPE_UNDEFINED   = 0x06,
    AMF_DATA_TYPE_REFERENCE   = 0x07,
    AMF_DATA_TYPE_MIXEDARRAY  = 0x08,
    AMF_DATA_TYPE_OBJECT_END  = 0x09,
    AMF_DATA_TYPE_ARRAY       = 0x0a,
    AMF_DATA_TYPE_DATE        = 0x0b,
    AMF_DATA_TYPE_LONG_STRING = 0x0c,
    AMF_DATA_TYPE_UNSUPPORTED = 0x0d,
} amfDataType;


/* audio tag */
typedef enum{
	SFI_LINEAR_PCM_PLATFORM_ENDIAN = 0,
	SFI_ADPCM                      = 1,
	SFI_MP3                        = 2,
	SFI_LINEAR_PCM_LITTLE_ENDIAN   = 3,
	SFI_NELLYMOSER_16KHZ_MONO      = 4,
	SFI_NELLYMOSER_8KHZ_MONO       = 5,
	SFI_NELLYMOSER                 = 6,
	SFI_G711A                      = 7,
	SFI_G711MU                     = 8,
	SFI_RESERVED                   = 9,
	SFI_AAC                        = 10,
	SFI_SPEEX                      = 11,
	SFI_MP3_8KHZ                   = 14,
	SFI_DEVIVE_SPECIFIC_SOUND      = 15,
}soundFormatIndex;


typedef enum{
	SRI_5_5KHZ = 0,
	SRI_11KHZ  = 1,
	SRI_22KHZ  = 2,
	SRI_44KHZ  = 3,
}soundSamplerateIndex;


typedef enum{
	SSI_8BIT  = 0,
	SSI_16BIT = 1,
}soundSizeIndex;


typedef enum{
	STI_MONO   = 0,
	STI_STEREO = 1,
}soundTypeIndex;



#define  AAC_PACKET_TYPE_SEQUENCE_HEADER 	(0)
#define  AAC_PACKET_TYPE_RAW 				(1)

typedef enum{
	AAC_MAIN = 1,
    AAC_LC   = 2,
    AAC_SSR  = 3,
}aacProfileIndex;

typedef enum{
	AAC_96KHz = 0x0,
    AAC_88_2KHz = 0x1,
    AAC_64KHz = 0x2,
   	AAC_48KHz = 0x3,
    AAC_44_1KHz = 0x4,
    AAC_32KHz = 0x5,
   	AAC_24KHz = 0x6,
    AAC_22_05KHz = 0x7,
    AAC_16KHz  = 0x8,
    AAC_12KHz  = 0x9,
    AAC_11_025KHz  = 0xa,
    AAC_8KHz  = 0xb,
    AAC_RESERVED = 0xc,
}aacSamplerateIndex;

typedef enum{
	AAC_CHANNEL_SPECIAL = 0x0,
    AAC_CHANNEL_MONO = 0x1,
    AAC_CHANNEL_STEREO = 0x2,
   	AAC_CHANNEL_3 = 0x3,
	AAC_CHANNEL_4 = 0x4,
	AAC_CHANNEL_5 = 0x5,
	AAC_CHANNEL_5_1 = 0x6,
	AAC_CHANNEL_7_1 = 0x7,
    AAC_CHANNELRESERVED = 0x8,
}aacChannelIndex;



#define  AVC_PACKET_TYPE_SEQUENCE_HEADER 			 (0)
#define  AVC_PACKET_TYPE_NALU 						 (1)
#define  AVC_PACKET_TYPE_END_OF_SEQUENCE 			 (2)

/* next for video tag */
#define  VIDEOTAG_FRAMETYPE_KEYFRAME                 (1)
#define  VIDEOTAG_FRAMETYPE_INTER_FRAME              (2)
#define  VIDEOTAG_FRAMETYPE_DISPOSABLE_INTER_FRAME   (3)
#define  VIDEOTAG_FRAMETYPE_GENERATED_KEYFRAME       (4)
#define  VIDEOTAG_FRAMETYPE_VIDEO_INFO_FRAME         (5)

#define  VIDEOTAG_CODECID_JPEG                       (1)
#define  VIDEOTAG_CODECID_SORENSON_H263              (2)
#define  VIDEOTAG_CODECID_SCREEN_VIDEO               (3)
#define  VIDEOTAG_CODECID_ON2_VP6                    (4)
#define  VIDEOTAG_CODECID_ON2_VP6_WITH_ALPHA_CHANNEL (5)
#define  VIDEOTAG_CODECID_SCREEN_VIDEO_VERSION_2     (6)
#define  VIDEOTAG_CODECID_AVC                        (7)



#pragma pack(push)
#pragma pack(1)   /* 1 bytes align */

typedef struct flvHeader{
	uint8_t  signature[3]; 		/* signature bytes always 'F' 'L' 'V': 0x46 0x4C 0x56 */
	uint8_t  version; 			/* file version, always 0x01 */
	uint8_t  type_flag; 		/* bit[7:3] and bit[1] always 0, bit[2] for aduio, bit[0] for video */
	uint32_t data_offset; 		/* size of header, 00 00 00 09(big-endian) for version 1 */
}T_FlvHeader, *PT_FlvHeader;   	    /* 9 bytes totally */


typedef struct flvTagHeader{
	uint8_t TagType; 					/* Type of this tag. Value are 8(audio), 9(video), 18(script), other(reserved). */
	uint8_t DataSize[3]; 				/* Length of the data in the Data filed. */
	uint8_t Timestamp[3]; 				/* Time in milliseconds at which the data in this applies. 0 in first tag in the FLV file. */
	uint8_t TimestampExtended; 			/* Extension of Timestamp field to form a SI32 value, it is upper 8 bits. */
	uint8_t StreamID[3]; 				/* Always 0 */
}T_FlvTagHeader, *PT_FlvTagHeader; 	/* 11 bytes total */


typedef struct flvTag{
	T_FlvTagHeader flvheader; 	/* tag header */
	uint8_t flvdata[0]; 		/* tag data index */
}T_FlvTag, *PT_FlvTag;


typedef struct avcVideoPacket{
	T_FlvTagHeader flvheader;
	uint8_t flvdata[0]; 		/* flv tag data index */
}T_AvcVideoPacket, *PT_AvcVideoPacket;


#pragma pack(pop)


#endif /* __FLV_FORMAT_H__ */
