#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include"MyMatrix.h"
#include"MyVector.h"
#include <list>
class EnemyBullet {
public:
	~EnemyBullet();

	void Initialize(
	    Model* model, const Vector3& position, const Vector3& rotate, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	Vector3 GetWorldPosition();

	// 衝突したら呼び出す関数
	void OnCollision();

	float radius = 1.0f;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	MyMatrix matrix_;

	MyVector vector_;

	float size = 1.5f;

	Vector3 scale = {size, size, size};

	Vector3 velocity_;
	// 寿命
	static const int32_t kLifeTime = 180;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	Matrix4x4 minusMatrix{0};

	Vector3 minusVelocity{0, 0, 0};

	Vector3 velocityZ{0, 0, 0};

};
