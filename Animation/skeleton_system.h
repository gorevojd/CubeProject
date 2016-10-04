#ifndef SKELETON_CODE_H

#include "../common_code.h"

struct Joint{
	glm::mat4x3 invBindPose;
	std::string name;
	u32 parentIndex;
};

struct Skeleton{
	std::vector<Joint> joints;
	std::map<std::string, s32> boneMapping;
};

#define SKELETON_CODE_H
#endif