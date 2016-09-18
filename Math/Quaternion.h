#ifndef QUATERNION_MY_H

#include <math.h>

struct quaternion{
	float x, y, z, w;

	quaternion(){
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 1.0f;
	}
	quaternion(float x, float y, float z, float w){
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	float Length() const{
		return sqrtf(x * x + y * y + z * z + w * w);
	}
};

inline quaternion IdentityQuaternion(){
	return quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}


inline quaternion NormalizeQuaternion(const quaternion& quat){
	float length = quat.Length();

	float x = quat.x / length;
	float y = quat.x / length;
	float z = quat.x / length;
	float w = quat.x / length;
}

inline quaternion ConjugateQuaternion(const quaternion& quat){
	return quaternion(-quat.x, -quat.y, -quat.z, quat.w);
}

inline quaternion MultiplyQuaternionByQuaternion(const quaternion& l, const quaternion& r){
	float w = l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z;
	float x = l.x * r.w + l.w * r.x + l.y * r.z - l.z * r.y;
	float y = l.y * r.w + l.w * r.y + l.z * r.x - l.x * r.z;
	float z = l.z * r.w + l.w * r.z + l.x * r.y - l.y * r.x;

	return quaternion(x, y, z, w);
}

inline quaternion operator * (const quaternion& q1, const quaternion& q2){
	return MultiplyQuaternionByQuaternion(q1, q2);
}

inline quaternion MultiplyQuaternionByVector(const quaternion& q, const vector3& v){
	float w = -q.x * v.x - q.y * v.y - q.z * v.z;
	float x = q.w * v.x + q.y * v.z - q.z * v.y;
	float y = q.w * v.y + q.z * v.x - q.x * v.z;
	float z = q.w * v.z + q.x * v.y - q.y * v.x;

	return quaternion(x, y, z, w);
}




#define QUATERNION_MY_H
#endif