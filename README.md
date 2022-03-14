#### 前言

> 注意：flv是不支持h.265封装的。


### 1、demo使用

```bash
$ make clean && make DEBUG=1
$
$ ./flv_mux_h264_aac
Usage:
   ./flv_mux_h264_aac avfile/test1_856x480_24fps.h264 24 avfile/test1_44100_stereo.aac out1.flv
   ./flv_mux_h264_aac avfile/test2_960x544_25fps.h264 25 avfile/test2_44100_mono.aac   out2.flv
```


### 2、参考链接

【参考文章】

 - [FLV格式详解\_JT同学的博客-CSDN博客\_flv格式**（推荐！！！）**](https://blog.csdn.net/weixin_42462202/article/details/88661883)

 - [FLV封装格式介绍及解析 - 简书](https://www.jianshu.com/p/419f99be3989)

 - [音视频封装：FLV格式详解和打包H264、AAC方案（上） - 云+社区 - 腾讯云](https://cloud.tencent.com/developer/article/1747043)

 - [音视频封装：FLV格式详解和打包H264、AAC方案（下） - 云+社区 - 腾讯云](https://cloud.tencent.com/developer/article/1746991)

【参考源码】

 - [https://github.com/ImSjt/H.264toFLV.git](https://github.com/ImSjt/H.264toFLV.git)

 - [https://github.com/rainfly123/flvmuxer](https://github.com/rainfly123/flvmuxer)

【工具下载】

 - SpecialAAAC.exe：[https://sourceforge.net/projects/aacstreamanalysis/](https://sourceforge.net/projects/aacstreamanalysis/)

 - H264BSAnalyzer.exe：[https://github.com/latelee/H264BSAnalyzer/tree/master/release](https://github.com/latelee/H264BSAnalyzer/tree/master/release)

 - FlvParse.exe：[https://github.com/ty6815/AvStackDocs/tree/master/media%20format/flv](https://github.com/ty6815/AvStackDocs/tree/master/media%20format/flv)


### 附录（demo目录架构）

```bash
$ tree
.
├── aac_adts.c
├── aac_adts.h
├── avfile
│   ├── out1.flv
│   ├── out2.flv
│   ├── test1_44100_stereo.aac
│   ├── test1_856x480_24fps.h264
│   ├── test2_44100_mono.aac
│   └── test2_960x544_25fps.h264
├── docs
│   ├── FLV封装格式介绍及解析 - 简书.mhtml
│   ├── FLV格式详解_JT同学的博客-CSDN博客_flv格式.mhtml
│   ├── 音视频封装：FLV格式详解和打包H264、AAC方案（上） - 云+社区 - 腾讯云.mhtml
│   └── 音视频封装：FLV格式详解和打包H264、AAC方案（下） - 云+社区 - 腾讯云.mhtml
├── flv.c
├── flv_format.h
├── flv.h
├── h264_nalu.c
├── h264_nalu.h
├── main.c
├── Makefile
├── README.md
├── reference_code
│   ├── flvmuxer-master.zip
│   └── H.264toFLV-master.zip
└── tools
    ├── FlvParse.exe
    ├── H264BSAnalyzer.exe
    └── SpecialAAAC.exe
```
