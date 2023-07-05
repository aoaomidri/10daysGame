﻿#include "MyVector.h"
#include<assert.h>
MyVector::MyVector() { 
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Vector3 MyVector::Transform(const Vector3& v, const Matrix4x4& m) { 
	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2]};
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

Vector3 MyVector::TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};

	return result;
}

float MyVector::Length(const Vector3& v) {
	float result{0.0f};
	float bulletNorm = static_cast<float>(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
	result = bulletNorm;
	return result;
}

Vector3 MyVector::Slerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 result{0, 0, 0};

	float dot = ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
	float denominatorA = sqrtf((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z));
	float denominatorB = sqrtf((v2.x * v2.x) + (v2.y * v2.y) + (v2.z * v2.z));
	float cosAngle = dot / (denominatorA * denominatorB);

	if (cosAngle > 1.0f) {
		cosAngle = 1.0f;
	} else if (cosAngle < -1.0f) {
		cosAngle = -1.0f;
	}

	// 2ベクトル間の角度
	float angle = std::acosf(cosAngle) * (180.0f / static_cast<float>(M_PI));

	float sinTh = std::sin(angle);
	if (sinTh == 0.0f) {
		return v1;
	}

	float Ps = std::sin(angle * (1 - t));
	float Pe = std::sin(angle * t);

	result.x = ((Ps * v1.x) + (Pe * v2.x) / sinTh);
	result.y = ((Ps * v1.y) + (Pe * v2.y) / sinTh);
	result.z = ((Ps * v1.z) + (Pe * v2.z) / sinTh);

	result = Normalize(result);

	return result;
}

Vector3 MyVector::Normalize(const Vector3& v) {
	Vector3 result{0, 0, 0};
	float bulletNorm = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

	if (bulletNorm != 0.0f) {

		result = {(v.x / bulletNorm), (v.y / bulletNorm), (v.z / bulletNorm)};
	}

	return result;
}

Vector3 MyVector::NormalizePlus(const Vector3& v, float speed) {
	Vector3 result{0, 0, 0};
	float bulletNorm = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

	if (bulletNorm != 0.0f) {

		result = {
		    (v.x / bulletNorm) * speed, (v.y / bulletNorm) * speed, (v.z / bulletNorm) * speed};
	}

	return result;
}

Vector3 MyVector::Cross(const Vector3& vA,const Vector3& vB) {
	Vector3 result{};

	result = {
	    (vA.y * vB.z) - (vA.z * vB.y), (vA.z * vB.x) - (vA.x * vB.z),
	    (vA.x * vB.y) - (vA.y * vB.x)};

	return result;
}

Vector3 MyVector::Multiply(float scalar, const Vector3& Vec) {
	Vector3 result{};
	result.x = Vec.x * scalar;
	result.y = Vec.y * scalar;
	result.z = Vec.z * scalar;
	return result;
}