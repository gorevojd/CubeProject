#ifndef QUATERNION_MY_H

#include <math.h>

struct quaternion{
	float x, y, z, w;	
};

inline quaternion Quaternion(){
	quaternion res;
	res.x = 0.0f;
	res.y = 0.0f;
	res.z = 0.0f;
	res.w = 1.0f;
	return res;
}

inline quaternion Quaternion(float x, float y, float z, float w){
	quaternion res;
	res.x = x;
	res.y = y;
	res.z = z;
	res.w = w;
	return res;
}

inline float Length(quaternion q) {
	return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

inline quaternion IdentityQuaternion(){
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

inline quaternion NormalizeQuaternion(const quaternion& quat){
	float length = Length(quat);

	float x = quat.x / length;
	float y = quat.x / length;
	float z = quat.x / length;
	float w = quat.x / length;
}

inline quaternion ConjugateQuaternion(const quaternion& quat){
	return Quaternion(-quat.x, -quat.y, -quat.z, quat.w);
}

quaternion MultiplyQuaternionByQuaternion(const quaternion& l, const quaternion& r){
#if 0 
	float w = l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z;
	float x = l.x * r.w + l.w * r.x + l.y * r.z - l.z * r.y;
	float y = l.y * r.w + l.w * r.y + l.z * r.x - l.x * r.z;
	float z = l.z * r.w + l.w * r.z + l.x * r.y - l.y * r.x;

#else
	float w = l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z;
	float x = l.w*r.x + l.x*r.w + l.y*r.z - l.z*r.y;
	float y = l.w*r.y - l.x*r.z + l.y*r.w + l.z*r.x;
	float z	= l.w*r.z + l.x*r.y - l.y*r.x + l.z*r.w;
#endif
	return Quaternion(x, y, z, w);
}

inline quaternion operator * (const quaternion& q1, const quaternion& q2){
	return MultiplyQuaternionByQuaternion(q1, q2);
}

inline quaternion MultiplyQuaternionByVector(const quaternion& q, const vector3& v){
	float w = -q.x * v.x - q.y * v.y - q.z * v.z;
	float x = q.w * v.x + q.y * v.z - q.z * v.y;
	float y = q.w * v.y + q.z * v.x - q.x * v.z;
	float z = q.w * v.z + q.x * v.y - q.y * v.x;

	return Quaternion(x, y, z, w);
}

inline quaternion InverseQuaternion(quaternion quat){
	
}

#define QUATERNION_MY_H
#endif