#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>

class Player;

class PlayerBullet {
public:
	enum class PlayerBulletState {
		Idle,
		Move,
		Return
	};
	~PlayerBullet();

	void Initialize(
	    Model* model, const Vector3& position, const Vector3& rotate, const Vector3& velocity);

	void Update();

	void ReturnPlayer();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	Vector3 GetWorldPosition();
	inline PlayerBulletState GetState() { return state_; };
	inline void SetState(PlayerBulletState state) { state_ = state; };
	inline void SetPlayer(Player* player) { player_ = player; };
	inline void SetShot(const Vector3& position, const Vector3& rotate, const Vector3& velocity){
		worldTransform_.translation_ = position;

		worldTransform_.rotation_ = rotate;

		velocity_ = velocity;
	};
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

	float size = 0.8f;

	Vector3 scale = {size, size, size};

	Vector3 velocity_;
	// 寿命
	static const int32_t kLifeTime = 120;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	PlayerBulletState state_ = PlayerBulletState::Idle;

	Player* player_ = nullptr;

	float t = 0.0f;
	//戻ってくるときの速度
	const float kReturnSpeed = 1.2f;
};
