#ifndef ASSET_SYSTEM_H

#include "../common_code.h"

#pragma pack(push, 1)
struct wav_header
{
	u32 chunkName;		
	u32 chunkSize;		
	u32 waveChunk;		

	u32 fmtChunkName;	
	u32 fmtChunkSize;	
	u16  format;		
	u16  channels;
	u32 samplesPerSecond;
	u32 avgBytesPerSec;
	u16  blockAlign;		
	u16  bitsPerSample;	

	u32 dataChunkName;	
	u32 dataChunkSize;	
};
#pragma pack(pop)

struct loaded_shader{
	u32 program;
};

#define ASSET_SYSTEM_H
#endif