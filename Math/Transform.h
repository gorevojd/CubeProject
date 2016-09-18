#ifndef TRANSFORM_MY_H

#include "Matrix4x4.h"
#include "Vector.h"
#include "Quaternion.h"

struct transform{
	vector3 Offset;
	quaternion Rotation;
	vector3 Scaling;
};

inline transform DefaultTransform(){
	transform trn;
	trn.Offset = vector3(0.0f);
	trn.Rotation = IdentityQuaternion();
	trn.Scaling = vector3(1.0f);
	return trn;
}

inline void TransformTranslate(transform& trn, vector3 offset){
	trn.Offset = trn.Offset + offset;
}

inline void TransformScale(transform& trn, vector3 scale){
	trn.Scaling.x = trn.Scaling.x * scale.x;
	trn.Scaling.y = trn.Scaling.y * scale.y;
	trn.Scaling.z = trn.Scaling.z * scale.z;
}

inline void TransformRotate(transform& trn, quaternion rotation){
	trn.Rotation = trn.Rotation * rotation;
}

inline matrix4x4 GetTransformationMatrix(const transform& trn){
	matrix4x4 translation = TranslationMatrix(trn.Offset);
	matrix4x4 rotation = RotationMatrix(trn.Rotation);
	matrix4x4 scaling = ScalingMatrix(trn.Scaling);
	return translation * rotation * scaling;
}

#define TRANSFORM_MY_H
#endif