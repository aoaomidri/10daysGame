#pragma once
#include"Vector4.h"
#include "Vector3.h"
#include"Vector2.h"
#include"Matrix4x4.h"
#include<iostream>
#define _USE_MATH_DEFINES
#include<math.h>
class MyVector {
public:
	MyVector();

private:
	float x, y, z;

public:

	// ベクトル変換
	Vector3 Transform(const Vector3& v, const Matrix4x4& m);
	// ベクトルの掛け算(返り値はVector3)wで割らない
	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
	//球面線形補間
	//Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
	//線形ほぁん
	//Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
	// 正規化
	Vector3 Normalize(const Vector3& v);
	// 正規化して速度を整える
	Vector3 NormalizePlus(const Vector3& v, float speed);
	// 長さだけ
	float Length(const Vector3& v);

	Vector3 Cross(const Vector3& vA, const Vector3& vB);

	Vector3 Multiply(float scalar, const Vector3& Vec);

	float LerpShortAngle(float a, float b, float t);

	float Dot(const Vector3& v1, const Vector3& v2);
	
	Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
};
