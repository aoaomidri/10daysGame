#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "WaterFlowEffect.h"
#include <list>

class Player;

class PlayerBullet {
public:
	enum class PlayerBulletState {
		Idle,
		Stance,
		Move,
		Return,
		Death
	};
	~PlayerBullet();

	void Initialize(
	    Model* model, Model* model2, Model* model3, const Vector3& position, const Vector3& rotate,
	    const Vector3& velocity);

	void Update();

	void FinAnimationUpdate();

	void Move();

	void Stance();

	void ReturnPlayer();

	void Idle();

	void Death();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	void setTarget(const WorldTransform* target) { target_ = target; };
	void setEnemy(const WorldTransform* target) { enemy_ = target; };

	Vector3 GetWorldPosition();
	Vector3 GetTargetWorldPosition();
	inline PlayerBulletState GetState() { return state_; };
	inline void SetState(PlayerBulletState state) { state_ = state; };
	void SetPlayer(Player* player);
	void SetShot(const Vector3& position, const Vector3& rotate, const Vector3& velocity);
	void StanceCancel();
	//攻撃前にプレイヤーの手元に持ってくる
	void SetShotIdle(const Vector3& position);
	// 衝突したら呼び出す関数
	void OnCollision();

	bool IsInvincible() { return isInvincible_; };

	float radius = 1.0f;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	Model* modelFin_ = nullptr;
	Model* modelWaterFlow_ = nullptr;
	
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	float size = 1.8f;
	float finSize = 1.0f;

	Vector3 scale = {size, size, size};
	Vector3 finScale = {finSize, finSize, finSize};

	Vector3 velocity_;
	// 寿命
	static const int32_t kLifeTime = 60;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	PlayerBulletState state_ = PlayerBulletState::Idle;

	Player* player_ = nullptr;

	float t = 0.0f;

	//攻撃時の速度
	const float kAttackSpeed =2.0f;

	//戻ってくるときの速度
	const float kReturnSpeed = 1.2f;

	Vector3 Fin_offset_Base = {0.0f, 0.0f, -1.0f};
	WorldTransform worldTransformFin_;
	float finRotate = 0.0f;
	float theta=0.0f;

	WorldTransform worldTransformRoll_;

	const WorldTransform* target_ = nullptr;
	const WorldTransform* enemy_ = nullptr;
	//水流エフェクト
	WaterFlowEffect waterFlowEffect;

	//idle状態で目指す位置
	WorldTransform worldTransformHerd_;
	float idleSpeed = 0.050f;
	float idleFollow = 0.020f;

	//待機中に追従するか
	bool isMove_ = true;
	//追従を再開する範囲
	const float kFollowArea = 20.0f;
	//追従を切る範囲
	const float kFollowOutArea = 2.0f;

	//モデルの色(texture)
	uint32_t color_;
	uint32_t textureHandleWhite_;
	uint32_t textureHandleRed_;

	//無敵判定
	bool isInvincible_ = false;

	//攻撃終了時の無敵時間
	const int kAttackEndInvincible = 30;
	int invincibleTime_;

	Vector3 rotate_;
};
