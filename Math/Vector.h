#ifndef VECTOR_MY_H

#include <math.h>

struct vector2{
	union{
		struct {
			float x, y;
		};
		float data[2];
	};

	vector2(){
		this->x = 0.0f;
		this->y = 0.0f;
	}
	vector2(float x, float y){
		this->x = x;
		this->y = y;
	}
};

struct vector3{
	union{
		struct {
			float x, y, z;
		};
		float data[3];
	};

	vector3(){
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}
	vector3(float value){
		this->x = value;
		this->y = value;
		this->z = value;
	}
	vector3(float x, float y, float z){
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

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

inline float Length(const vector2& vec){
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline float Length(const vector3& vec){
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}


inline vector2 Normalize(const vector2& vec){
	float len = Length(vec);
	vector2 res;
	res.x = vec.x / len;
	res.y = vec.y / len;
	return res;
}

inline vector3 Normalize(const vector3& vec){
	float len = Length(vec);
	vector3 res;
	res.x = vec.x / len;
	res.y = vec.y / len;
	res.z = vec.z / len;
	return res;
}

inline float DotProduct(const vector4& v1, const vector4& v2){
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
}

inline float DotProduct(const vector3& v1, const vector3& v2){
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

inline float DotProduct(const vector2& v1, const vector2& v2){
	return (v1.x * v2.x + v1.y * v2.y);
}

inline vector3 CrossProduct(const vector3& v1, const vector3& v2){
	vector3 res;

	res.x = v1.y * v2.z - v1.z * v2.y;
	res.y = v1.x * v2.z - v1.z * v2.y;
	res.z = v1.x * v2.y - v1.z * v2.y;

	return res;
}

inline vector2 operator+(const vector2& v1, const vector2& v2){
	return vector2(v1.x + v2.x, v1.y + v2.y);
}

inline vector2 operator-(const vector2& v1, const vector2& v2){
	return vector2(v1.x - v2.x, v1.y - v2.y);
}

inline vector2 operator*(const vector2& v, float f){
	return vector2(v.x * f, v.y * f);
}

inline vector2 operator/(const vector2& v, float f){
	return vector2(v.x / f, v.y / f);
}

inline vector3 operator+(const vector3& v1, const vector3& v2){
	return vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline vector3 operator-(const vector3& v1, const vector3& v2){
	return vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline vector3 operator*(const vector3& v, float f){
	return vector3(v.x * f, v.y * f, v.z * f);
}

inline vector3 operator/(const vector3& v, float f){
	return vector3(v.x / f, v.y / f, v.z / f);
}

#define VECTOR_MY_H
#endif