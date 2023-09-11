#pragma once

#include "Input.h"
#include "MyMatrix.h"
#include "MyVector.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
class GameCamera {
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
	float t = 0.05f;

	// 目標角度
	float destinationAngleY_ = 0.0f;
	float destinationAngleX_ = 0.0f;
	// 基準のオフセット
	Vector3 baseOffset;

	Vector3 shotOffset;

	Vector3 rootOffset;

	float maxRotate;

	float minRotate;

	float rotateSpeed;

	float nowRotate;

	bool isSetRotate = true;

	// 行列の作成
	std::unique_ptr<MyMatrix> matrix_;

	// Vectorの作成
	std::unique_ptr<MyVector> vector_;

public:
	void Initialize();

	void Update();

	void Reset();

	void DrawImgui();

	Vector3 offsetCalculation(const Vector3& offset) const;

	void LostTarget() { target_ = nullptr; };

	void SetTarget(const WorldTransform* target);

	void SetViewProjection(const ViewProjection target) { viewProjection_ = target; };

	float GetNowRotate() { return nowRotate; }

	ViewProjection& GetViewProjection() { return viewProjection_; }

	ViewingFrustum& GetViewingFrustum() { return viewingFrustum_; }
};