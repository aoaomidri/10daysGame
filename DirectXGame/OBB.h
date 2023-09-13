#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <stdint.h>
struct OBB
{
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix);
Matrix4x4 GetRotate(const OBB& obb);
