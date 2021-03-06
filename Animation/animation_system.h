#ifndef ANIMATION_SYSTEM_H

#include "../common_code.h"
#include "../Animation/skeleton_system.h"

struct NodeTransform{
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
};

struct AnimationSample{
	NodeTransform* jointTransforms;
};

struct AnimationClip{
	Skeleton* skeleton;
	float framesPerSecond;
	u32 frameCount;
	AnimationSample* samples;
	bool isLooping;
};

#define ANIMATION_SYSTEM_H
#endif