#ifndef COMMON_CODE_H

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <stdint.h>

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Math/Vector.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"

#define Assert(condition) if(!condition){*(int*)0 = 0;}

#define MATH_PI 3.1415926535
#define INFLUENCE_BONE_COUNT 4

//#define DEFWNDWIDTH 1366
//#define DEFWNDHEIGHT 768

typedef glm::vec2 v2;
typedef glm::vec3 v3;
typedef glm::vec4 v4;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;
typedef int32_t bool32;

typedef float r32;
typedef float f32;
typedef double r64;


#define COMMON_CODE_H
#endif