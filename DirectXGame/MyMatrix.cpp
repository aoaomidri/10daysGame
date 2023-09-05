#include "MyMatrix.h"
#include <assert.h>
MyMatrix::MyMatrix() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ScaleMatrix.m[i][j] = 0.0f;
			TranslateMatrix.m[i][j] = 0.0f;
			RotateMatrix.m[i][j] = 0.0f;
			RotateMatrixX.m[i][j] = 0.0f;
			RotateMatrixY.m[i][j] = 0.0f;
			RotateMatrixZ.m[i][j] = 0.0f;
		}
	}
};

////積
Matrix4x4 MyMatrix::Multiply(const Matrix4x4& mat1, const Matrix4x4& mat2) {
	Matrix4x4 result = {0.0f};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = mat1.m[i][0] * mat2.m[0][j] + mat1.m[i][1] * mat2.m[1][j] +
			                 mat1.m[i][2] * mat2.m[2][j] + mat1.m[i][3] * mat2.m[3][j];
		}
	}
	return result;
}

Matrix4x4 MyMatrix::MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result{};
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MyMatrix::MakeRotateMatrixX(const Vector3& rot) {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = cosf(rot.x);
	result.m[1][2] = sinf(rot.x);
	result.m[2][1] = -sinf(rot.x);
	result.m[2][2] = cosf(rot.x);
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 MyMatrix::MakeRotateMatrixY(const Vector3& rot) {
	Matrix4x4 result{};
	result.m[1][1] = 1.0f;
	result.m[0][0] = cosf(rot.y);
	result.m[0][2] = -sinf(rot.y);
	result.m[2][0] = sinf(rot.y);
	result.m[2][2] = cosf(rot.y);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MyMatrix::MakeRotateMatrixZ(const Vector3& rot) {
	Matrix4x4 result{};
	result.m[2][2] = 1.0f;
	result.m[0][0] = cosf(rot.z);
	result.m[0][1] = sinf(rot.z);
	result.m[1][0] = -sinf(rot.z);
	result.m[1][1] = cosf(rot.z);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MyMatrix::MakeRotateMatrix(const Vector3& rot) {
	Matrix4x4 result{};
	// X,Y,Z軸の回転行列の作成
	RotateMatrixX = MakeRotateMatrixX(rot);
	RotateMatrixY = MakeRotateMatrixY(rot);
	RotateMatrixZ = MakeRotateMatrixZ(rot);

	result = Multiply(RotateMatrixX, Multiply(RotateMatrixY, RotateMatrixZ));

	return result;
}

Matrix4x4 MyMatrix::MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

Matrix4x4
    MyMatrix::MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {
	Matrix4x4 result{};

	// スケーリング行列の作成
	ScaleMatrix = MakeScaleMatrix(scale);
	// X,Y,Z軸の回転行列の作成
	RotateMatrixX = MakeRotateMatrixX(rot);
	RotateMatrixY = MakeRotateMatrixY(rot);
	RotateMatrixZ = MakeRotateMatrixZ(rot);
	// 回転行列の結合
	RotateMatrix = Multiply(RotateMatrixX, Multiply(RotateMatrixY, RotateMatrixZ));
	// 平行移動行列の作成
	TranslateMatrix = MakeTranslateMatrix(translate);

	result = Multiply(ScaleMatrix, Multiply(RotateMatrix, TranslateMatrix));

	return result;
}

Matrix4x4
    MyMatrix::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{0};

	result.m[0][0] = (1.0f / aspectRatio) * (1.0f / tanf(fovY / 2.0f));
	result.m[1][1] = (1.0f / tanf(fovY / 2.0f));
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}

Matrix4x4 MyMatrix::MakeOrthographicMatrix(
    float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{0};

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 MyMatrix::MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result{0};

	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -(height / 2.0f);
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + (width / 2.0f);
	result.m[3][1] = top + (height / 2.0f);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 MyMatrix::Inverce(const Matrix4x4& mat) {
	float A = 0.0f;
	Matrix4x4 result = {0.0f};
	// 行列式の計算
	A = mat.m[0][0] * mat.m[1][1] * mat.m[2][2] * mat.m[3][3] +
	    mat.m[0][0] * mat.m[1][2] * mat.m[2][3] * mat.m[3][1] +
	    mat.m[0][0] * mat.m[1][3] * mat.m[2][1] * mat.m[3][2] -
	    mat.m[0][0] * mat.m[1][3] * mat.m[2][2] * mat.m[3][1] -
	    mat.m[0][0] * mat.m[1][2] * mat.m[2][1] * mat.m[3][3] -
	    mat.m[0][0] * mat.m[1][1] * mat.m[2][3] * mat.m[3][2] -
	    mat.m[0][1] * mat.m[1][0] * mat.m[2][2] * mat.m[3][3] -
	    mat.m[0][2] * mat.m[1][0] * mat.m[2][3] * mat.m[3][1] -
	    mat.m[0][3] * mat.m[1][0] * mat.m[2][1] * mat.m[3][2] +
	    mat.m[0][3] * mat.m[1][0] * mat.m[2][2] * mat.m[3][1] +
	    mat.m[0][2] * mat.m[1][0] * mat.m[2][1] * mat.m[3][3] +
	    mat.m[0][1] * mat.m[1][0] * mat.m[2][3] * mat.m[3][2] +
	    mat.m[0][1] * mat.m[1][2] * mat.m[2][0] * mat.m[3][3] +
	    mat.m[0][2] * mat.m[1][3] * mat.m[2][0] * mat.m[3][1] +
	    mat.m[0][3] * mat.m[1][1] * mat.m[2][0] * mat.m[3][2] -
	    mat.m[0][3] * mat.m[1][2] * mat.m[2][0] * mat.m[3][1] -
	    mat.m[0][2] * mat.m[1][1] * mat.m[2][0] * mat.m[3][3] -
	    mat.m[0][1] * mat.m[1][3] * mat.m[2][0] * mat.m[3][2] -
	    mat.m[0][1] * mat.m[1][2] * mat.m[2][3] * mat.m[3][0] -
	    mat.m[0][2] * mat.m[1][3] * mat.m[2][1] * mat.m[3][0] -
	    mat.m[0][3] * mat.m[1][1] * mat.m[2][2] * mat.m[3][0] +
	    mat.m[0][3] * mat.m[1][2] * mat.m[2][1] * mat.m[3][0] +
	    mat.m[0][2] * mat.m[1][1] * mat.m[2][3] * mat.m[3][0] +
	    mat.m[0][1] * mat.m[1][3] * mat.m[2][2] * mat.m[3][0];

	// 逆行列の計算
	result.m[0][0] =
	    (mat.m[1][1] * mat.m[2][2] * mat.m[3][3] + mat.m[1][2] * mat.m[2][3] * mat.m[3][1] +
	     mat.m[1][3] * mat.m[2][1] * mat.m[3][2] - mat.m[1][3] * mat.m[2][2] * mat.m[3][1] -
	     mat.m[1][2] * mat.m[2][1] * mat.m[3][3] - mat.m[1][1] * mat.m[2][3] * mat.m[3][2]) /
	    A;

	result.m[0][1] =
	    (-mat.m[0][1] * mat.m[2][2] * mat.m[3][3] - mat.m[0][2] * mat.m[2][3] * mat.m[3][1] -
	     mat.m[0][3] * mat.m[2][1] * mat.m[3][2] + mat.m[0][3] * mat.m[2][2] * mat.m[3][1] +
	     mat.m[0][2] * mat.m[2][1] * mat.m[3][3] + mat.m[0][1] * mat.m[2][3] * mat.m[3][2]) /
	    A;

	result.m[0][2] =
	    (mat.m[0][1] * mat.m[1][2] * mat.m[3][3] + mat.m[0][2] * mat.m[1][3] * mat.m[3][1] +
	     mat.m[0][3] * mat.m[1][1] * mat.m[3][2] - mat.m[0][3] * mat.m[1][2] * mat.m[3][1] -
	     mat.m[0][2] * mat.m[1][1] * mat.m[3][3] - mat.m[0][1] * mat.m[1][3] * mat.m[3][2]) /
	    A;

	result.m[0][3] =
	    (-mat.m[0][1] * mat.m[1][2] * mat.m[2][3] - mat.m[0][2] * mat.m[1][3] * mat.m[2][1] -
	     mat.m[0][3] * mat.m[1][1] * mat.m[2][2] + mat.m[0][3] * mat.m[1][2] * mat.m[2][1] +
	     mat.m[0][2] * mat.m[1][1] * mat.m[2][3] + mat.m[0][1] * mat.m[1][3] * mat.m[2][2]) /
	    A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[1][0] =
	    (-mat.m[1][0] * mat.m[2][2] * mat.m[3][3] - mat.m[1][2] * mat.m[2][3] * mat.m[3][0] -
	     mat.m[1][3] * mat.m[2][0] * mat.m[3][2] + mat.m[1][3] * mat.m[2][2] * mat.m[3][0] +
	     mat.m[1][2] * mat.m[2][0] * mat.m[3][3] + mat.m[1][0] * mat.m[2][3] * mat.m[3][2]) /
	    A;

	result.m[1][1] =
	    (mat.m[0][0] * mat.m[2][2] * mat.m[3][3] + mat.m[0][2] * mat.m[2][3] * mat.m[3][0] +
	     mat.m[0][3] * mat.m[2][0] * mat.m[3][2] - mat.m[0][3] * mat.m[2][2] * mat.m[3][0] -
	     mat.m[0][2] * mat.m[2][0] * mat.m[3][3] - mat.m[0][0] * mat.m[2][3] * mat.m[3][2]) /
	    A;

	result.m[1][2] =
	    (-mat.m[0][0] * mat.m[1][2] * mat.m[3][3] - mat.m[0][2] * mat.m[1][3] * mat.m[3][0] -
	     mat.m[0][3] * mat.m[1][0] * mat.m[3][2] + mat.m[0][3] * mat.m[1][2] * mat.m[3][0] +
	     mat.m[0][2] * mat.m[1][0] * mat.m[3][3] + mat.m[0][0] * mat.m[1][3] * mat.m[3][2]) /
	    A;

	result.m[1][3] =
	    (mat.m[0][0] * mat.m[1][2] * mat.m[2][3] + mat.m[0][2] * mat.m[1][3] * mat.m[2][0] +
	     mat.m[0][3] * mat.m[1][0] * mat.m[2][2] - mat.m[0][3] * mat.m[1][2] * mat.m[2][0] -
	     mat.m[0][2] * mat.m[1][0] * mat.m[2][3] - mat.m[0][0] * mat.m[1][3] * mat.m[2][2]) /
	    A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[2][0] =
	    (mat.m[1][0] * mat.m[2][1] * mat.m[3][3] + mat.m[1][1] * mat.m[2][3] * mat.m[3][0] +
	     mat.m[1][3] * mat.m[2][0] * mat.m[3][1] - mat.m[1][3] * mat.m[2][1] * mat.m[3][0] -
	     mat.m[1][1] * mat.m[2][0] * mat.m[3][3] - mat.m[1][0] * mat.m[2][3] * mat.m[3][1]) /
	    A;

	result.m[2][1] =
	    (-mat.m[0][0] * mat.m[2][1] * mat.m[3][3] - mat.m[0][1] * mat.m[2][3] * mat.m[3][0] -
	     mat.m[0][3] * mat.m[2][0] * mat.m[3][1] + mat.m[0][3] * mat.m[2][1] * mat.m[3][0] +
	     mat.m[0][1] * mat.m[2][0] * mat.m[3][3] + mat.m[0][0] * mat.m[2][3] * mat.m[3][1]) /
	    A;

	result.m[2][2] =
	    (mat.m[0][0] * mat.m[1][1] * mat.m[3][3] + mat.m[0][1] * mat.m[1][3] * mat.m[3][0] +
	     mat.m[0][3] * mat.m[1][0] * mat.m[3][1] - mat.m[0][3] * mat.m[1][1] * mat.m[3][0] -
	     mat.m[0][1] * mat.m[1][0] * mat.m[3][3] - mat.m[0][0] * mat.m[1][3] * mat.m[3][1]) /
	    A;

	result.m[2][3] =
	    (-mat.m[0][0] * mat.m[1][1] * mat.m[2][3] - mat.m[0][1] * mat.m[1][3] * mat.m[2][0] -
	     mat.m[0][3] * mat.m[1][0] * mat.m[2][1] + mat.m[0][3] * mat.m[1][1] * mat.m[2][0] +
	     mat.m[0][1] * mat.m[1][0] * mat.m[2][3] + mat.m[0][0] * mat.m[1][3] * mat.m[2][1]) /
	    A;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.m[3][0] =
	    (-mat.m[1][0] * mat.m[2][1] * mat.m[3][2] - mat.m[1][1] * mat.m[2][2] * mat.m[3][0] -
	     mat.m[1][2] * mat.m[2][0] * mat.m[3][1] + mat.m[1][2] * mat.m[2][1] * mat.m[3][0] +
	     mat.m[1][1] * mat.m[2][0] * mat.m[3][2] + mat.m[1][0] * mat.m[2][2] * mat.m[3][1]) /
	    A;

	result.m[3][1] =
	    (mat.m[0][0] * mat.m[2][1] * mat.m[3][2] + mat.m[0][1] * mat.m[2][2] * mat.m[3][0] +
	     mat.m[0][2] * mat.m[2][0] * mat.m[3][1] - mat.m[0][2] * mat.m[2][1] * mat.m[3][0] -
	     mat.m[0][1] * mat.m[2][0] * mat.m[3][2] - mat.m[0][0] * mat.m[2][2] * mat.m[3][1]) /
	    A;

	result.m[3][2] =
	    (-mat.m[0][0] * mat.m[1][1] * mat.m[3][2] - mat.m[0][1] * mat.m[1][2] * mat.m[3][0] -
	     mat.m[0][2] * mat.m[1][0] * mat.m[3][1] + mat.m[0][2] * mat.m[1][1] * mat.m[3][0] +
	     mat.m[0][1] * mat.m[1][0] * mat.m[3][2] + mat.m[0][0] * mat.m[1][2] * mat.m[3][1]) /
	    A;

	result.m[3][3] =
	    (mat.m[0][0] * mat.m[1][1] * mat.m[2][2] + mat.m[0][1] * mat.m[1][2] * mat.m[2][0] +
	     mat.m[0][2] * mat.m[1][0] * mat.m[2][1] - mat.m[0][2] * mat.m[1][1] * mat.m[2][0] -
	     mat.m[0][1] * mat.m[1][0] * mat.m[2][2] - mat.m[0][0] * mat.m[1][2] * mat.m[2][1]) /
	    A;

	return result;
}