#ifndef CLASS_FONTS_H

#include "common_code.h"

struct character_info{
	u32 TextureID;
	s32 Width;
	s32 Height;
	s32 BearingX;
	s32 BearingY;
	u32 Advance;
};

struct atlas_character_info{
	s32 Width;
	s32 Height;

	s32 BearingX;
	s32 BearingY;

	u32 AdvanceX;
	u32 AdvanceY;

	float tCoordsX1;
	float tCoordsX2;
	float tCoordsY1;
	float tCoordsY2;
};

struct character_atlas{
	u32 TextureID;
	s32 Width;
	s32 Height;
	atlas_character_info Characters[128];
};

#define CLASS_FONTS_H
#endif