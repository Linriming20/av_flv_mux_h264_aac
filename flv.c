
#include "h264_nalu.h"
#include "aac_adts.h"
#include "flv.h"


static int generateFlvHeader(int hasVideo, int hasAudio, uint8_t *pOutData, uint32_t *pOutDataLen)
{
	T_FlvHeader flvheader = {0};

	if(!pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	flvheader.signature[0] = 'F';
	flvheader.signature[1] = 'L';
	flvheader.signature[2] = 'V';
	flvheader.version = 0x01;
	flvheader.type_flag = AUDIO_SUPPORT(hasAudio) | VIDEO_SUPPORT(hasVideo);
	flvheader.data_offset = 0x09000000; /* 9 Bytes, size of flv header. big-endian. */

	memcpy(pOutData, (uint8_t *)&flvheader, sizeof(T_FlvHeader));
	*pOutDataLen = SIZE_FLV_HEADER;

	return 0;
}


static int generatePreviousTagSize(uint32_t size, uint8_t *pOutData, uint32_t *pOutDataLen)
{
	if(!pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* storge in file with big-endian */
	pOutData[0] = (uint8_t)((size >> 24) & 0xFF);
	pOutData[1] = (uint8_t)((size >> 16) & 0xFF);
	pOutData[2] = (uint8_t)((size >> 8) & 0xFF);
	pOutData[3] = (uint8_t)(size & 0xFF);

	*pOutDataLen = SIZE_PREVIOUS_TAG_SIZE;

	return 0;
}


static int generateScriptTag(uint8_t *pOutData, uint32_t *pOutDataLen)
{
	PT_FlvTag ptScriptTag = NULL;
	char *pString = NULL;
	uint32_t stringLen = -1;
	uint32_t dataSize = -1;
	uint32_t amf2ArrayCnt = 0; /* have no member in this demo! */
	uint32_t curPos = 0;
#if 0
	/* have no member in this demo, so not need to convert! */
	union{
		double d;
		uint8_t c[8];
	} un;
#endif

	if(!pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ptScriptTag = (PT_FlvTag)pOutData;
	//dataSize = -1; // calcurate bihind.
	//*pOutDataLen = -1; // calcurate bihind.

	ptScriptTag->flvheader.TagType = FLVTAG_TYPE_SCRIPT;
	//ptScriptTag->flvheader.DataSize[0] = -1; // calcurate bihind.
	//ptScriptTag->flvheader.DataSize[1] = -1;
	//ptScriptTag->flvheader.DataSize[2] = -1;
	ptScriptTag->flvheader.Timestamp[0] = 0;
	ptScriptTag->flvheader.Timestamp[1] = 0;
	ptScriptTag->flvheader.Timestamp[2] = 0;
	ptScriptTag->flvheader.TimestampExtended = 0;
	ptScriptTag->flvheader.StreamID[0] = 0;
	ptScriptTag->flvheader.StreamID[1] = 0;
	ptScriptTag->flvheader.StreamID[2] = 0;

	/* AMF1 */
	pString = "onMetaData";
	stringLen = strlen(pString);
	ptScriptTag->flvdata[curPos + 0] = AMF_DATA_TYPE_STRING;
	ptScriptTag->flvdata[curPos + 1] = (uint8_t)((stringLen >> 8) & 0xFF);
	ptScriptTag->flvdata[curPos + 2] = (uint8_t)(stringLen & 0xFF);
	memcpy(&ptScriptTag->flvdata[curPos + 3], pString, stringLen);
	curPos += (3 + stringLen);

	/* AMF2 */
	ptScriptTag->flvdata[curPos + 0] = AMF_DATA_TYPE_MIXEDARRAY;
	ptScriptTag->flvdata[curPos + 1] = (uint8_t)((amf2ArrayCnt >> 24) & 0xFF);
	ptScriptTag->flvdata[curPos + 2] = (uint8_t)((amf2ArrayCnt >> 16) & 0xFF);
	ptScriptTag->flvdata[curPos + 3] = (uint8_t)((amf2ArrayCnt >>  8) & 0xFF);
	ptScriptTag->flvdata[curPos + 4] = (uint8_t)((amf2ArrayCnt >>  0) & 0xFF);
	curPos += 5;

	#if 0 /* reference to `uint32_t amf2ArrayCnt = ...` */
	pString = "duration";
	stringLen = strlen(pString);
	ptScriptTag->flvdata[curPos + 0] = (uint8_t)((stringLen >> 8) & 0xFF);
	ptScriptTag->flvdata[curPos + 1] = (uint8_t)(stringLen & 0xFF);
	memcpy(&ptScriptTag->flvdata[curPos + 2], pString, stringLen);
	curPos += (2 + stringLen);

	un.d = 30; /* Second */
	ptScriptTag->flvdata[curPos + 0] = AMF_DATA_TYPE_NUMBER;
	ptScriptTag->flvdata[curPos + 1] = un.c[7];
	ptScriptTag->flvdata[curPos + 2] = un.c[6];
	ptScriptTag->flvdata[curPos + 3] = un.c[5];
	ptScriptTag->flvdata[curPos + 4] = un.c[4];
	ptScriptTag->flvdata[curPos + 5] = un.c[3];
	ptScriptTag->flvdata[curPos + 6] = un.c[2];
	ptScriptTag->flvdata[curPos + 7] = un.c[1];
	ptScriptTag->flvdata[curPos + 8] = un.c[0];
	curPos += (1+8);
	#endif

	/* end */
	ptScriptTag->flvdata[curPos + 0] = 0x00;
	ptScriptTag->flvdata[curPos + 1] = 0x00;
	ptScriptTag->flvdata[curPos + 2] = AMF_DATA_TYPE_OBJECT_END;
	curPos += 3;

	// now we can calculate it.
	dataSize = curPos;
	*pOutDataLen = SIZE_FLV_TAG_HEADER + dataSize;
	ptScriptTag->flvheader.DataSize[0] = (uint8_t)((dataSize >> 16 & 0xFF));
	ptScriptTag->flvheader.DataSize[1] = (uint8_t)((dataSize >> 8 & 0xFF));
	ptScriptTag->flvheader.DataSize[2] = (uint8_t)(dataSize & 0xFF);

	return 0;
}


static int generateAvcSequenceHeader(uint8_t *spsBuf, uint16_t spsLen, uint8_t *ppsBuf, uint16_t ppsLen,
									uint32_t timestamp_ms, uint8_t *pOutData, uint32_t *pOutDataLen)
{
	PT_FlvTag ptVideoTag = NULL;
	uint32_t dataSize = 0;

	if(!spsBuf || !spsLen || !ppsBuf || !ppsLen || !pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ptVideoTag = (PT_FlvTag)pOutData;
	dataSize = (13 + spsLen) + (3 + ppsLen);
	*pOutDataLen = SIZE_FLV_TAG_HEADER + dataSize;

	ptVideoTag->flvheader.TagType = FLVTAG_TYPE_VIDEO;
	ptVideoTag->flvheader.DataSize[0] = (uint8_t)((dataSize >> 16 & 0xFF));
	ptVideoTag->flvheader.DataSize[1] = (uint8_t)((dataSize >> 8 & 0xFF));
	ptVideoTag->flvheader.DataSize[2] = (uint8_t)(dataSize & 0xFF);
	ptVideoTag->flvheader.Timestamp[0] = (uint8_t)((timestamp_ms >> 16) & 0xFF);
	ptVideoTag->flvheader.Timestamp[1] = (uint8_t)((timestamp_ms >> 8) & 0xFF);
	ptVideoTag->flvheader.Timestamp[2] = (uint8_t)((timestamp_ms) & 0xFF);
	ptVideoTag->flvheader.TimestampExtended = (uint8_t)((timestamp_ms >> 24) & 0xFF);
	ptVideoTag->flvheader.StreamID[0] = 0;
	ptVideoTag->flvheader.StreamID[1] = 0;
	ptVideoTag->flvheader.StreamID[2] = 0;

	ptVideoTag->flvdata[0] = ((VIDEOTAG_FRAMETYPE_KEYFRAME << 4) |\
							  (VIDEOTAG_CODECID_AVC)); /* 0x17, keyframe, avc */
	/* next for AVCVIDEOPACKET */
	ptVideoTag->flvdata[1] = AVC_PACKET_TYPE_SEQUENCE_HEADER; /* AVCPacketType: 0, AVC sequence header */
	ptVideoTag->flvdata[2] = 0x00; /* CompositionTime: AVCPacketType != 1, so it is 0, otherwise data[2~4] is CTS */
	ptVideoTag->flvdata[3] = 0x00; /* CompositionTime: AVCPacketType != 1, so it is 0, otherwise data[2~4] is CTS */
	ptVideoTag->flvdata[4] = 0x00; /* CompositionTime: AVCPacketType != 1, so it is 0, otherwise data[2~4] is CTS */
								   /* next for AVCDecoderConfigurationRecord */
	ptVideoTag->flvdata[5] = 0x01; /* ConfigurationVersion: always 0x01*/
	ptVideoTag->flvdata[6] = spsBuf[1]; /* AVCProfileIndication: the first byte after the 'nalu type'(buf no include 'start code') */
	ptVideoTag->flvdata[7] = spsBuf[2]; /* profile_compatibility: the second byte after the 'nalu type'(buf no include 'start code') */
	ptVideoTag->flvdata[8] = spsBuf[3]; /* AVCLevelIndication: the third byte after the 'nalu type'(buf no include 'start code') */
	ptVideoTag->flvdata[9] = 0xFF; /* lengthSizeMinusOne: always 0xFF, bit[7:2]: '111111b'reversed */
	ptVideoTag->flvdata[10] = 0xE1; /* NumOfSequenceParmeterSets: always 0xE1, bit[7:5]: '111b'reversed  */
	ptVideoTag->flvdata[11] = (uint8_t)((spsLen >> 8) & 0xFF); /* SequenceParamterSetLength: big-endian, H  */
	ptVideoTag->flvdata[12] = (uint8_t)(spsLen & 0xFF); /* SequenceParamterSetLength: big-endian, L */
	memcpy(&ptVideoTag->flvdata[13], spsBuf, spsLen);

	ptVideoTag->flvdata[13+spsLen] = 0x01; /* NumOfPictureParmeterSets: always 0x01 */
	ptVideoTag->flvdata[13+spsLen+1] = (uint8_t)((ppsLen >> 8) & 0xFF); /* PictureParamterSetLength: big-endian, H */
	ptVideoTag->flvdata[13+spsLen+2] = (uint8_t)(ppsLen& 0xFF); /* PictureParamterSetLength: big-endian, L */
	memcpy(&ptVideoTag->flvdata[13+spsLen+3], ppsBuf, ppsLen);

	return 0;
}


static int generateAvcNALU(uint8_t *pNaluData, uint32_t naluDataLen, uint32_t isIDRNalu, uint32_t timestamp_ms, uint8_t *pOutData, uint32_t *pOutDataLen)
{
	PT_FlvTag ptVideoTag = NULL;
	uint32_t dataSize = 0;

	if(!pNaluData || !naluDataLen || !pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ptVideoTag = (PT_FlvTag)pOutData;
	dataSize = 9 + naluDataLen;
	*pOutDataLen = SIZE_FLV_TAG_HEADER + dataSize;

	ptVideoTag->flvheader.TagType = FLVTAG_TYPE_VIDEO;
	ptVideoTag->flvheader.DataSize[0] = (uint8_t)((dataSize >> 16 & 0xFF));
	ptVideoTag->flvheader.DataSize[1] = (uint8_t)((dataSize >> 8 & 0xFF));
	ptVideoTag->flvheader.DataSize[2] = (uint8_t)(dataSize & 0xFF);
	ptVideoTag->flvheader.Timestamp[0] = (uint8_t)((timestamp_ms >> 16) & 0xFF);
	ptVideoTag->flvheader.Timestamp[1] = (uint8_t)((timestamp_ms >> 8) & 0xFF);
	ptVideoTag->flvheader.Timestamp[2] = (uint8_t)((timestamp_ms) & 0xFF);
	ptVideoTag->flvheader.TimestampExtended = (uint8_t)((timestamp_ms >> 24) & 0xFF);
	ptVideoTag->flvheader.StreamID[0] = 0;
	ptVideoTag->flvheader.StreamID[1] = 0;
	ptVideoTag->flvheader.StreamID[2] = 0;

	if(isIDRNalu)
	{
		ptVideoTag->flvdata[0] = ((VIDEOTAG_FRAMETYPE_KEYFRAME << 4) |\
									(VIDEOTAG_CODECID_AVC)); /* 0x17, keyframe, avc */
	}
	else
	{
		ptVideoTag->flvdata[0] = ((VIDEOTAG_FRAMETYPE_INTER_FRAME << 4) |\
									(VIDEOTAG_CODECID_AVC)); /* 0x27, inter frame, avc */
	}
	/* next for AVCVIDEOPACKET */
	ptVideoTag->flvdata[1] = AVC_PACKET_TYPE_NALU; /* AVCPacketType: 1, NALU */
	ptVideoTag->flvdata[2] = 0x00; /* CompositionTime: CTS = 0, because no b'frame */
	ptVideoTag->flvdata[3] = 0x00; /* CompositionTime: CTS = 0, because no b'frame */
	ptVideoTag->flvdata[4] = 0x00; /* CompositionTime: CTS = 0, because no b'frame */

	ptVideoTag->flvdata[5] = (uint8_t)((naluDataLen >> 24 & 0xFF));
	ptVideoTag->flvdata[6] = (uint8_t)((naluDataLen >> 16 & 0xFF));
	ptVideoTag->flvdata[7] = (uint8_t)((naluDataLen >> 8 & 0xFF));
	ptVideoTag->flvdata[8] = (uint8_t)(naluDataLen & 0xFF);

	memcpy(&ptVideoTag->flvdata[9], pNaluData, naluDataLen);

	return 0;
}


static int generateAvcEndOfSequence(uint32_t timestamp_ms, uint8_t *pOutData, uint32_t *pOutDataLen)
{
	PT_FlvTag ptVideoTag = NULL;
	uint32_t dataSize = 0;

	if(!pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ptVideoTag = (PT_FlvTag)pOutData;
	dataSize = 5; /* fixed, flvdata[0~4] */
	*pOutDataLen = SIZE_FLV_TAG_HEADER + dataSize;

	ptVideoTag->flvheader.TagType = FLVTAG_TYPE_VIDEO;
	ptVideoTag->flvheader.DataSize[0] = (uint8_t)((dataSize >> 16 & 0xFF));
	ptVideoTag->flvheader.DataSize[1] = (uint8_t)((dataSize >> 8 & 0xFF));
	ptVideoTag->flvheader.DataSize[2] = (uint8_t)(dataSize & 0xFF);
	ptVideoTag->flvheader.Timestamp[0] = (uint8_t)((timestamp_ms >> 16) & 0xFF);
	ptVideoTag->flvheader.Timestamp[1] = (uint8_t)((timestamp_ms >> 8) & 0xFF);
	ptVideoTag->flvheader.Timestamp[2] = (uint8_t)((timestamp_ms) & 0xFF);
	ptVideoTag->flvheader.TimestampExtended = (uint8_t)((timestamp_ms >> 24) & 0xFF);
	ptVideoTag->flvheader.StreamID[0] = 0;
	ptVideoTag->flvheader.StreamID[1] = 0;
	ptVideoTag->flvheader.StreamID[2] = 0;

	ptVideoTag->flvdata[0] = ((VIDEOTAG_FRAMETYPE_KEYFRAME << 4) |\
							  (VIDEOTAG_CODECID_AVC)); /* 0x17, keyframe, avc */
	/* next for AVCVIDEOPACKET */
	ptVideoTag->flvdata[1] = AVC_PACKET_TYPE_END_OF_SEQUENCE; /* AVCPacketType: 2, AVC end of sequence header */
	ptVideoTag->flvdata[2] = 0x00; /* CompositionTime: AVCPacketType != 1, so it is 0, otherwise data[2~4] is CTS */
	ptVideoTag->flvdata[3] = 0x00; /* CompositionTime: AVCPacketType != 1, so it is 0, otherwise data[2~4] is CTS */
	ptVideoTag->flvdata[4] = 0x00; /* CompositionTime: AVCPacketType != 1, so it is 0, otherwise data[2~4] is CTS */

	return 0;
}


static int generateAacSequenceHeader(uint32_t timestamp_ms, PT_AdtsHeader adtsInfo, uint8_t *pOutData, uint32_t *pOutDataLen)
{
	PT_FlvTag ptAudioTag = NULL;
	uint32_t dataSize = 0;
	soundSamplerateIndex sri;
	soundTypeIndex sti;
	aacSamplerateIndex asi;
	aacChannelIndex aci;

	if(!pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	switch(adtsInfo->sampling_freq_index)
	{
		case SFI_44100: sri = SRI_44KHZ;asi = AAC_44_1KHz; 	break;
		case SFI_22050: sri = SRI_22KHZ;asi = AAC_22_05KHz; break;
		case SFI_11025: sri = SRI_11KHZ;asi = AAC_11_025KHz;break;
		default:
			printf("[%s:%d]: Params invaild!\n", __FUNCTION__, __LINE__);
			return -1;
	}
	switch(adtsInfo->channel_configuration)
	{
		case 1: sti = STI_MONO; aci = AAC_CHANNEL_MONO; break;
		case 2: sti = STI_STEREO; aci = AAC_CHANNEL_STEREO; break;
		default:
			printf("[%s:%d]: Params invaild!\n", __FUNCTION__, __LINE__);
			return -1;
	}

	ptAudioTag = (PT_FlvTag)pOutData;
	dataSize = 4; /* fixed, flvdata[0~3] */
	*pOutDataLen = SIZE_FLV_TAG_HEADER + dataSize;

	ptAudioTag->flvheader.TagType = FLVTAG_TYPE_AUDIO;
	ptAudioTag->flvheader.DataSize[0] = (uint8_t)((dataSize >> 16 & 0xFF));
	ptAudioTag->flvheader.DataSize[1] = (uint8_t)((dataSize >> 8 & 0xFF));
	ptAudioTag->flvheader.DataSize[2] = (uint8_t)(dataSize & 0xFF);
	ptAudioTag->flvheader.Timestamp[0] = (uint8_t)((timestamp_ms >> 16) & 0xFF);
	ptAudioTag->flvheader.Timestamp[1] = (uint8_t)((timestamp_ms >> 8) & 0xFF);
	ptAudioTag->flvheader.Timestamp[2] = (uint8_t)((timestamp_ms) & 0xFF);
	ptAudioTag->flvheader.TimestampExtended = (uint8_t)((timestamp_ms >> 24) & 0xFF);
	ptAudioTag->flvheader.StreamID[0] = 0;
	ptAudioTag->flvheader.StreamID[1] = 0;
	ptAudioTag->flvheader.StreamID[2] = 0;

	ptAudioTag->flvdata[0] = (SFI_AAC << 4) |\
							 (sri << 2) |\
							 (SSI_16BIT << 1) |\
							 (sti);
	ptAudioTag->flvdata[1] = AAC_PACKET_TYPE_SEQUENCE_HEADER;
	ptAudioTag->flvdata[2] = (AAC_LC << 3) |\
							 ((asi >> 1) & 0x7);
	ptAudioTag->flvdata[3] = ((asi & 0x1) << 7) |\
							 (aci << 3);

	return 0;
}


static int generateAacRaw(uint8_t *pAtdsRawData, PT_AdtsHeader pAdtsInfo, uint32_t timestamp_ms, uint8_t *pOutData, uint32_t *pOutDataLen)
{
	PT_FlvTag ptAudioTag = NULL;
	uint32_t dataSize = 0;
	soundSamplerateIndex sri;
	soundTypeIndex sti;

	if(!pAtdsRawData || !pAdtsInfo || !pOutData || !pOutDataLen)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	switch(pAdtsInfo->sampling_freq_index)
	{
		case SFI_44100: sri = SRI_44KHZ; break;
		case SFI_22050: sri = SRI_22KHZ; break;
		case SFI_11025: sri = SRI_11KHZ; break;
		default:
			printf("[%s:%d]: Params invaild!\n", __FUNCTION__, __LINE__);
			return -1;
	}
	switch(pAdtsInfo->channel_configuration)
	{
		case 1: sti = STI_MONO;   break;
		case 2: sti = STI_STEREO; break;
		default:
			printf("[%s:%d]: Params invaild!\n", __FUNCTION__, __LINE__);
			return -1;
	}

	ptAudioTag = (PT_FlvTag)pOutData;
	dataSize = 2 + (pAdtsInfo->aac_frame_length - AAC_ADTS_HEADER_SIZE);
	*pOutDataLen = SIZE_FLV_TAG_HEADER + dataSize;

	ptAudioTag->flvheader.TagType = FLVTAG_TYPE_AUDIO;
	ptAudioTag->flvheader.DataSize[0] = (uint8_t)((dataSize >> 16 & 0xFF));
	ptAudioTag->flvheader.DataSize[1] = (uint8_t)((dataSize >> 8 & 0xFF));
	ptAudioTag->flvheader.DataSize[2] = (uint8_t)(dataSize & 0xFF);
	ptAudioTag->flvheader.Timestamp[0] = (uint8_t)((timestamp_ms >> 16) & 0xFF);
	ptAudioTag->flvheader.Timestamp[1] = (uint8_t)((timestamp_ms >> 8) & 0xFF);
	ptAudioTag->flvheader.Timestamp[2] = (uint8_t)((timestamp_ms) & 0xFF);
	ptAudioTag->flvheader.TimestampExtended = (uint8_t)((timestamp_ms >> 24) & 0xFF);
	ptAudioTag->flvheader.StreamID[0] = 0;
	ptAudioTag->flvheader.StreamID[1] = 0;
	ptAudioTag->flvheader.StreamID[2] = 0;

	ptAudioTag->flvdata[0] = (SFI_AAC << 4) |\
							 (sri << 2) |\
							 (SSI_16BIT << 1) |\
							 (sti);
	ptAudioTag->flvdata[1] = AAC_PACKET_TYPE_RAW;
	memcpy(&ptAudioTag->flvdata[2], pAtdsRawData, pAdtsInfo->aac_frame_length - AAC_ADTS_HEADER_SIZE);

	return 0;
}


int flv_mux_h264_aac(char *h264FileName, uint32_t vFps, char *aacFileName, char *flvFileName)
{
	FILE *fpH264 = NULL;
	FILE *fpAAC  = NULL;
	FILE *fpFLV  = NULL;
	uint8_t *h264Buf = NULL;
	uint8_t *aacBuf = NULL;
	uint8_t *flvBuf = NULL;
	uint32_t flvBufLen = 0;
	uint64_t timeStamp_ms = 0; // the timestamp is start from 0 in flv file.
	uint8_t spsBuf[1024] = {0};  // note!!!: it maybe happen the 'Segmentation fault', as 1024 is too long for 'sps', but it maybe save the 'sei' or other unused data if first nalu isn't 'sps'.
	uint8_t ppsBuf[64] = {0};
	uint32_t previousTagSize = 0;
	uint32_t videoFps = vFps;
	uint32_t audioFps = -1; // calcurate bihind.
	int ret = -1;
	T_NaluInfo spsNaluInfo = {};
	T_NaluInfo ppsNaluInfo = {};
	T_NaluInfo naluInfo = {};
	T_AdtsHeader adtsHeader = {};

	if(!h264FileName || !vFps || !aacFileName || !flvFileName)
	{
		printf("[%s:%d] Params invalid!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* open file */
	fpH264 = fopen(h264FileName, "rb");
	if (!fpH264)
	{
		printf("open %s error!\n", h264FileName);
		goto exit;
	}
	fpAAC  = fopen(aacFileName,  "rb");
	if (!fpAAC)
	{
		printf("open %s error!\n", aacFileName);
		goto exit;
	}
	fpFLV  = fopen(flvFileName,  "wb");
	if (!fpFLV)
	{
		printf("open %s error!\n", flvFileName);
		goto exit;
	}


	/* alloc memory */
	h264Buf = (uint8_t *)malloc(MAX_NALU_SIZE);
	if (!h264Buf)
	{
		printf("malloc error!\n");
		goto exit;
	}
	aacBuf = (uint8_t *)malloc(MAX_ADTS_SIZE);
	if (!aacBuf)
	{
		printf("malloc error!\n");
		goto exit;
	}
	flvBuf = (uint8_t *)malloc(MAX_FLV_BUF_SIZE);
	if (!flvBuf)
	{
		printf("malloc error!\n");
		goto exit;
	}


	/* parse AAC-ADTS */
	ret = getAdtsFrame(fpAAC, aacBuf, &adtsHeader);
	if(!ret)
	{
		fseek(fpAAC, 0, SEEK_SET); // reset
		switch(adtsHeader.sampling_freq_index)
		{
			case SFI_96000: audioFps = 1000.0/(1024*1000/96000); break;
			case SFI_88200: audioFps = 1000.0/(1024*1000/88200); break;
			case SFI_64000: audioFps = 1000.0/(1024*1000/64000); break;
			case SFI_48000: audioFps = 1000.0/(1024*1000/48000); break;
			case SFI_44100: audioFps = 1000.0/(1024*1000/44100); break;
			case SFI_32000: audioFps = 1000.0/(1024*1000/32000); break;
			case SFI_24000: audioFps = 1000.0/(1024*1000/24000); break;
			case SFI_22050: audioFps = 1000.0/(1024*1000/22050); break;
			case SFI_16000: audioFps = 1000.0/(1024*1000/16000); break;
			case SFI_12000: audioFps = 1000.0/(1024*1000/12000); break;
			case SFI_11025: audioFps = 1000.0/(1024*1000/11025); break;
			case SFI_8000:  audioFps = 1000.0/(1024*1000/ 8000); break;
			case SFI_7350:  audioFps = 1000.0/(1024*1000/ 7350); break;
			default:        audioFps = -1; break;
		}
		DEBUG("AAC Info:\n"
				"\t id: %d\n"
				"\t profile: %d\n"
				"\t freq index: %d\n"
				"\t fps: %d\n"
				"\t channels: %d\n",
				adtsHeader.id, adtsHeader.profile,
				adtsHeader.sampling_freq_index, audioFps,
				adtsHeader.channel_configuration);
	}


	/* part 1/7: FLV Header */
	generateFlvHeader(1, 1, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

	previousTagSize = 0;  // 0, because there's no tag before
	generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);


#if 1 //todo
	/* part 2/7: Script Tag */
	generateScriptTag(flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

	previousTagSize = flvBufLen;
	generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);
#endif


	/* part 3/7: Video Tag (AVC sequence header) */
	while(1)
	{

		DEBUG("find sps now!\n");
		ret = getOneH264Nalu(fpH264, spsBuf, &spsNaluInfo);
		if (ret == -1)
		{
			printf("get h264 nalu failed!\n");
			goto exit;
		}

		DEBUG("type = 0x%02x\n", spsBuf[spsNaluInfo.startcode_len]);
		if(spsNaluInfo.nalu_type == NALU_TYPE_SPS)
		{
			DEBUG("had found sps, now find pps!\n");
			ret = getOneH264Nalu(fpH264, ppsBuf, &ppsNaluInfo);
			if (ret == -1)
			{
				printf("get h264 nalu failed!\n");
				goto exit;
			}

			DEBUG("ppsBuf[%d] = 0x%02x\n", ppsNaluInfo.startcode_len, ppsBuf[ppsNaluInfo.startcode_len]);
			if(ppsNaluInfo.nalu_type == NALU_TYPE_PPS)
			{
				DEBUG("had found pps\n");
				break;
			}
		}
		else
		{
			DEBUG("sps not found, continue!\n");
			/* find next NALU and judge */
			continue;
		}

	}
	generateAvcSequenceHeader(spsBuf + spsNaluInfo.startcode_len, spsNaluInfo.data_len - spsNaluInfo.startcode_len,
							  ppsBuf + ppsNaluInfo.startcode_len, ppsNaluInfo.data_len - ppsNaluInfo.startcode_len,
							  timeStamp_ms/* =0 */, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

	previousTagSize = flvBufLen;
	generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);


	/* part 4/7: Audio Tag (Audio sequence header) */
	generateAacSequenceHeader(timeStamp_ms/* =0 */, &adtsHeader, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

	previousTagSize = flvBufLen;
	generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);


#if 0
	/* just for debug !!! */
	ret = getOneH264Nalu(fpH264, h264Buf, &naluInfo);
	if (ret < 0)
	{
		printf("getOneH264Nalu error!\n");
	}
	generateAvcNALU(h264Buf + naluInfo.startcode_len, naluInfo.data_len - naluInfo.startcode_len, 1, timeStamp_ms, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

	previousTagSize = flvBufLen;
	generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);


	ret = getAdtsFrame(fpAAC, aacBuf, &adtsHeader);
	if (ret < 0)
	{
		printf("getAdtsFrame error!\n");
	}

	generateAacRaw(aacBuf+AAC_ADTS_HEADER_SIZE, &adtsHeader, timeStamp_ms, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

	previousTagSize = flvBufLen;
	generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);
#else
	while(1)
	{
		if (timeStamp_ms % (1000/videoFps) == 0)
		{
			/* part 5/7: Video Tag (AVC NALU) */
			do
			{
				ret = getOneH264Nalu(fpH264, h264Buf, &naluInfo);
				if (ret < 0)
				{
					if(ret == -2)
						DEBUG("h264 file end!\n");
					else
						printf(RED"getOneH264Nalu error!\n"COLOR_END);
					goto mux_end;
				}
				DEBUG(GREEN"[video] get one H.264 NALU(0x%02X) with length: %d\n"COLOR_END, h264Buf[naluInfo.startcode_len], naluInfo.data_len);

			}while((naluInfo.nalu_type != NALU_TYPE_IDR) &&\
				   (naluInfo.nalu_type != NALU_TYPE_SLICE));

			if (naluInfo.nalu_type == NALU_TYPE_IDR)
			{
				generateAvcNALU(h264Buf + naluInfo.startcode_len, naluInfo.data_len - naluInfo.startcode_len, 1, timeStamp_ms, flvBuf, &flvBufLen);
				fwrite(flvBuf, flvBufLen, 1, fpFLV);
			}
			else
			{
				generateAvcNALU(h264Buf + naluInfo.startcode_len, naluInfo.data_len - naluInfo.startcode_len, 0, timeStamp_ms, flvBuf, &flvBufLen);
				fwrite(flvBuf, flvBufLen, 1, fpFLV);
			}

			previousTagSize = flvBufLen;
			generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
			fwrite(flvBuf, flvBufLen, 1, fpFLV);
		}

		if (timeStamp_ms % (1000/audioFps) == 0)
		{
			/* part 6/7: Audio Tag (AAC raw) */
			ret = getAdtsFrame(fpAAC, aacBuf, &adtsHeader);
			if (ret < 0)
			{
				if(ret == -2)
					DEBUG("aac file end!\n");
				else
					printf(RED"getAdtsFrame error!\n"COLOR_END);
				goto mux_end;
			}
			DEBUG(RED"[audio] get one AAC-ADTS frame with length: %d\n"COLOR_END, adtsHeader.aac_frame_length);

			generateAacRaw(aacBuf+AAC_ADTS_HEADER_SIZE, &adtsHeader, timeStamp_ms, flvBuf, &flvBufLen);
			fwrite(flvBuf, flvBufLen, 1, fpFLV);

			previousTagSize = flvBufLen;
			generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
			fwrite(flvBuf, flvBufLen, 1, fpFLV);
		}
		timeStamp_ms++;
	}
#endif

mux_end:
	/* part 7/7: Video Tag (AVC end of sequence) */
	generateAvcEndOfSequence(timeStamp_ms, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

	previousTagSize = flvBufLen;
	generatePreviousTagSize(previousTagSize, flvBuf, &flvBufLen);
	fwrite(flvBuf, flvBufLen, 1, fpFLV);

exit:
	if(flvBuf) free(flvBuf);
	if(h264Buf) free(h264Buf);
	if(aacBuf) free(aacBuf);
	if(fpH264) fclose(fpH264);
	if(fpAAC)  fclose(fpAAC);
	if(fpFLV)  {fflush(fpFLV); fclose(fpFLV);}

	return 0;
}



