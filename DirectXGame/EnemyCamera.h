#pragma once
#include"ViewProjection.h"
#include"WorldTransform.h"
#include"MyMatrix.h"
#include"MyVector.h"
class EnemyCamera {
public:
	

private:
	ViewingFrustum viewingFrustum_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 追従対象
	const WorldTransform* target_ = nullptr;

	// 追従対象の残像座標
	Vector3 interTarget_ = {};

	float distance;

	// 補完レート
	const float t = 0.1f;

	// 目標角度
	float destinationAngleY_ = 0.0f;
	float destinationAngleX_ = 0.0f;
	// 行列の作成
	std::unique_ptr<MyMatrix> matrix_;

	// Vectorの作成
	std::unique_ptr<MyVector> vector_;

public:
	void Initialize();

	void Update();

	void Reset();

	Vector3 offsetCalculation() const;

	void SetTarget(const WorldTransform* target);

	ViewProjection& GetViewProjection() { return viewProjection_; }

	ViewingFrustum& GetViewingFrustum() { return viewingFrustum_; }
};
