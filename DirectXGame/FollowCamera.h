#pragma once
#include"ViewProjection.h"
#include"WorldTransform.h"
#include"MyMatrix.h"
#include"MyVector.h"
#include"Input.h"
class FollowCamera {
public:
private:
	ViewingFrustum viewingFrustum_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 追従対象
	const WorldTransform* target_ = nullptr;

	// キーボード入力
	Input* input_ = nullptr;

	// コントローラー入力
	XINPUT_STATE joyState;

	// 追従対象の残像座標
	Vector3 interTarget_ = {};

	float distance;
	const float offset_t = 0.1f;

	// 補完レート
	float t = 0.1f;

	// 目標角度
	float destinationAngleY_ = 0.0f;
	float destinationAngleX_ = 0.0f;
	//基準のオフセット
	Vector3 baseOffset;

	Vector3 shotOffset;

	Vector3 rootOffset;

	float maxRotate;

	float minRotate;
	//基となる回転速度
	float rotateSpeedBase_ = 0.05f;
	//回転速度
	float rotateSpeed_ = 0;
	//デッドゾーン
	float threshold_ = 0.7f;

	// 行列の作成
	std::unique_ptr<MyMatrix> matrix_;

	// Vectorの作成
	std::unique_ptr<MyVector> vector_;

public:
	void Initialize();

	void Update();

	void Reset();

	Vector3 offsetCalculation(const Vector3& offset) const;

	void SetTarget(const WorldTransform* target);

	void SetThreshold(float threshold) { threshold_ = threshold; }

	void SetRotateSpeed(float rotatespeed) { rotateSpeedBase_ = rotatespeed; }

	ViewProjection& GetViewProjection() { return viewProjection_; }

	ViewingFrustum& GetViewingFrustum() { return viewingFrustum_; }

	float GetThreshold() { return threshold_; }

	float GetRotateSpeed() { return rotateSpeedBase_; }
};
