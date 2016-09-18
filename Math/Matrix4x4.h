#ifndef MATRIX4X4_MY_H

#include "Vector.h"
#include "Quaternion.h"

struct matrix4x4{
	union{
		float data[4][4];
		struct{
			float a1, a2, a3, a4,
					b1, b2, b3, b4,
					c1, c2, c3, c4,
					d1, d2, d3, d4;
		};
		struct {
			vector4 row1;
			vector4 row2;
			vector4 row3;
			vector4 row4;
		};
	};
	matrix4x4(){
		this->row1 = vector4(0.0f, 0.0f, 0.0f, 0.0f);
		this->row2 = vector4(0.0f, 0.0f, 0.0f, 0.0f);
		this->row3 = vector4(0.0f, 0.0f, 0.0f, 0.0f);
		this->row4 = vector4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	matrix4x4(float value){
		this->row1 = vector4(value, 0.0f, 0.0f, 0.0f);
		this->row2 = vector4(0.0f, value, 0.0f, 0.0f);
		this->row3 = vector4(0.0f, 0.0f, value, 0.0f);
		this->row4 = vector4(0.0f, 0.0f, 0.0f, value);
	}
	matrix4x4(vector4 vec1, vector4 vec2, vector4 vec3, vector4 vec4){
		this->row1 = vec1;
		this->row2 = vec2;
		this->row3 = vec3;
		this->row4 = vec4;
	}
};

inline matrix4x4 IdentityMatrix(){
	matrix4x4 result;

	result.row1 = vector4(1.0f, 0.0f, 0.0f, 0.0f);
	result.row2 = vector4(0.0f, 1.0f, 0.0f, 0.0f);
	result.row3 = vector4(0.0f, 0.0f, 1.0f, 0.0f);
	result.row4 = vector4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

inline matrix4x4 TransposeMatrix(const matrix4x4& mat){
	matrix4x4 res;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			res.data[i][j] = mat.data[j][i];
		}
	}
}

inline matrix4x4 MultiplyMatrices(const matrix4x4& m1, const matrix4x4& m2){
	matrix4x4 res;
	for (int j = 0; j < 4; j++){
		for (int i = 0; i < 4; i++){
#if 0
			vector4 SourceV(
				m1.data[j][0],
				m1.data[j][1],
				m1.data[j][2],
				m1.data[j][3]);
			vector4 DestV(
				m2.data[0][i],
				m2.data[1][i],
				m2.data[2][i],
				m2.data[3][i]);
#else
			vector4 SourceV(
				m1.data[0][j],
				m1.data[1][j],
				m1.data[2][j],
				m1.data[3][j]);
			vector4 DestV(
				m2.data[i][0],
				m2.data[i][1],
				m2.data[i][2],
				m2.data[i][3]);
#endif
			res.data[i][j] = DotProduct(SourceV, DestV);
		}
	}
	return res;
}

inline matrix4x4 operator *(const matrix4x4& m1, const matrix4x4& m2){
	return MultiplyMatrices(m1, m2);
}

inline vector4 MultiplyMatrixByVector4(const matrix4x4& mat, const vector4& vec){
	vector4 res;
	for (int j = 0; j < 4; j++){
		vector4 SourceV(
			mat.data[j][0],
			mat.data[j][1],
			mat.data[j][2],
			mat.data[j][3]);

		res.data[j] = DotProduct(SourceV, vec);
	}
	return res;
}

inline vector4 operator *(const matrix4x4& mat, const vector4& vec){
	return MultiplyMatrixByVector4(mat, vec);
}

inline matrix4x4 TranslationMatrix(const vector3& vec){
	matrix4x4 iden = IdentityMatrix();
	iden.a4 = vec.x;
	iden.b4 = vec.y;
	iden.c4 = vec.z;
	return iden;
}

inline matrix4x4 ScalingMatrix(const vector3& scale){
	matrix4x4 iden = IdentityMatrix();
	iden.a1 = scale.x;
	iden.b2 = scale.y;
	iden.c3 = scale.z;
	return iden;
}

inline matrix4x4 RotationMatrix(const quaternion& qu){
	matrix4x4 result;

	result.data[0][0] = 1.0 - 2 * qu.y * qu.y - 2 * qu.z * qu.z;
	result.data[0][1] = 2 * qu.x * qu.y + 2 * qu.z * qu.w;
	result.data[0][2] = 2 * qu.x * qu.z - 2 * qu.y * qu.w;
	result.data[0][3] = 0.0f;

	result.data[1][0] = 2 * qu.x * qu.y - 2 * qu.z * qu.w;
	result.data[1][1] = 1.0 - 2 * qu.x * qu.x - 2 * qu.z * qu.z;
	result.data[1][2] = 2 * qu.y * qu.z + 2 * qu.x * qu.w;
	result.data[1][0] = 0.0f;

	result.data[2][0] = 2 * qu.x * qu.z + 2 * qu.y * qu.w;
	result.data[2][1] = 2 * qu.y * qu.z - 2 * qu.x * qu.w;
	result.data[2][2] = 1.0 - 2 * qu.x * qu.x - 2 * qu.y * qu.y;
	result.data[2][3] = 0.0f;

	result.data[3][0] = 0.0f;
	result.data[3][1] = 0.0f;
	result.data[3][2] = 0.0f;
	result.data[3][3] = 1.0f;

	return result;
}

#define MATRIX4X4_MY_H
#endif