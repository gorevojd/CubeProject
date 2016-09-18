#ifndef OBJECT_TRANSFORMATION

#include "../common_code.h"

struct object_transformation{
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
};

inline void TranslateObjectTransformation(object_transformation& transform, glm::vec3 offset){
	transform.translation = transform.translation + offset;
}

inline void RotateObjectTransformation(object_transformation& transform, glm::quat rotation){
	transform.rotation *= rotation;
}

inline void ScaleObjectTransformation(object_transformation& transform, glm::vec3 scale){
	
}

inline glm::mat4 GetTransformationMatrix(const object_transformation& tran){
	glm::mat4 transl = glm::translate(glm::mat4(1.0), tran.translation);
	glm::mat4 rot = glm::mat4_cast(tran.rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), tran.scale);
	return transl * rot * scale;
}

#define OBJECT_TRANSFORMATION
#endif