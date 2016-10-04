#ifndef VECTOR_MY_H

#include <math.h>

struct vector2{
	union{
		struct {
			float x, y;
		};
		float data[2];
	};
};

inline vector2 Vector2(){
	vector2 res;
	res.x = 0.0f;
	res.y = 0.0f;
	return res;
}

inline vector2 Vector2(float x, float y){
	vector2 res;
	res.x = 0.0f;
	res.y = 0.0f;
	return res;
}

struct vector3{
	union{
		struct {
			float x, y, z;
		};
		float data[3];
	};
};

inline vector3 Vector3(){
	vector3 res;
	res.x = 0.0f;
	res.y = 0.0f;
	res.z = 0.0f;
	return res;
}

inline vector3 Vector3(float value){
	vector3 res;
	res.x = value;
	res.y = value;
	res.z = value;
	return res;
}

inline vector3 Vector3(float x, float y, float z){
	vector3 res;
	res.x = x;
	res.y = y;
	res.z = z;
	return res;
}

struct vector4{
	union{
		struct{
			float x, y, z, w;
		};
		float data[4];
	};

	vector4(){
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 0.0f;
	}
	vector4(float x, float y, float z, float w){
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

inline vector4 Vector4(){
	vector4 res;
	res.x = 0.0f;
	res.y = 0.0f;
	res.z = 0.0f;
	res.w = 0.0f;
	return res;
}

inline vector4 Vector4(float x, float y, float z, float w){
	vector4 res;
	res.x = x;
	res.y = y;
	res.z = z;
	res.w = w;
	return res;
}

inline vector4 Vector4(vector3 v, float w){
	vector4 res;
	res.x = v.x;
	res.y = v.y;
	res.z = v.z;
	res.w = w;
}

inline float Length(vector2 vec){
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline float Length(vector3 vec){
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}


inline vector2 Normalize(vector2 vec){
	float len = Length(vec);
	vector2 res;
	res.x = vec.x / len;
	res.y = vec.y / len;
	return res;
}

inline vector3 Normalize(vector3 vec){
	float len = Length(vec);
	vector3 res;
	res.x = vec.x / len;
	res.y = vec.y / len;
	res.z = vec.z / len;
	return res;
}

inline float DotProduct(vector4 v1, vector4 v2){
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
}

inline float DotProduct(vector3 v1, vector3 v2){
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

inline float DotProduct(vector2 v1, vector2 v2){
	return (v1.x * v2.x + v1.y * v2.y);
}

inline vector3 CrossProduct(vector3 v1, vector3 v2){
	vector3 res;

	res.x = v1.y * v2.z - v1.z * v2.y;
	res.y = v1.x * v2.z - v1.z * v2.y;
	res.z = v1.x * v2.y - v1.z * v2.y;

	return res;
}

inline vector2 operator+(vector2 v1, vector2 v2){
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

inline vector2 operator-(vector2 v1, vector2 v2){
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}

inline vector2 operator*(vector2 v, float f){
	return Vector2(v.x * f, v.y * f);
}

inline vector2 operator/(vector2 v, float f){
	return Vector2(v.x / f, v.y / f);
}

inline vector3 operator+(vector3 v1, vector3 v2){
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline vector3 operator-(vector3 v1, vector3 v2){
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline vector3 operator*(vector3 v, float f){
	return Vector3(v.x * f, v.y * f, v.z * f);
}

inline vector3 operator/(vector3 v, float f){
	return Vector3(v.x / f, v.y / f, v.z / f);
}

#define VECTOR_MY_H
#endif