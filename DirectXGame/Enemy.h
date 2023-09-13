#pragma once
#include"BaseCharacter.h"
#include"EnemyEffect.h"
#include"EnemyBullet.h"
#include<optional>
#include"Adjustment_Item.h"
#include"Input.h"

class Enemy : public BaseCharacter{
private:
	//エネミーのパーツの座標
	WorldTransform worldTransformL_parts_;
	WorldTransform worldTransformHitBox_;
	//WorldTransform worldTransformR_parts_;

	WorldTransform worldTransformAir;

	Vector3 AirOffset = {0};

	Vector3 shot_offset = {0};

	float EnemyLife = 10;

	const float kMaxEnemyLife = 250.0f;
	//体力の割合
	float enemyLifePer;
	
	// 行列の作成
	MyMatrix matrix;
	// ベクトルの計算
	MyVector vector_;
	// モデルの大きさ
	float size = 5.0f;

	Vector3 scale = {size, size, size};
	Vector3 scaleChild = {1.0f, 1.0f, 1.0f};

	//移動制限
	const float MoveMax = 237.00000f;

	//ひれ
	Vector3 Fin_offset_Base = {0.0f, 0.0f, -8.72f};
	float finRotate = 0.0f;
	float theta = 0.0f;
	void FinAnimationUpdate();

	// キャラクターの移動速さ
	float kCharacterSpeed = 0.5f;

	Matrix4x4 minusMatrix{0};

	Vector3 minusVelocity{0, 0, 0};

	Vector3 velocityZ{0, 0, 0};

	OBB obb = {
	    .center{0.0f, 0.0f,0.0f},
	    .orientations = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	    .size{15.0f,20.0f,50.0f},
	};

	std::list<EnemyEffect*> effect_;

	// 弾
	std::list<EnemyBullet*> bullets_;

	// 発射の時間
	int fireTimer = 0;

	// 自機の弾の発射間隔
	int kFireInterval = 60;

	int Life = 0;

	// 発射タイマー
	int32_t ChargeTimer = 0;

	int chackCollision;

	// キーボード入力
	Input* input_ = nullptr;
	/*敵の行動に関する変数*/

	bool isRotate = false;

	// 射撃対象
	const WorldTransform* target_ = nullptr;

	Vector3 move;

	int enemyMoveCount = 0;

	int enemyMoveInterval = 180;

	int enemyMoveCountMax = 480;

	int fireCount;

	Vector3 movePos[5]{};
	
	int moveCount = 0;

	int EnemyActionsCount = 0;

	//タックルに関連する関数
	float rotate;

	bool isTackle = false;

	int tackleTimer;

	int tackleTimerMax = 90;

	int tackleMoveCount;

	int tackleMoveTime;

	int tackleMoveInterval = 120;

	int tackleMoveCountMax = 3;

	int shotInterval = 75;

	float tackleSpeedBase = 3.0f;

	//死んだ後の演出に関する変数
	int deathTimer = 0;

	WorldTransform worldTransformRoll_;

	void Fire(float bulletSpeed);

	void TripleFire(float bulletSpeed);

	void randFire(float bulletSpeed);

	void TackleInitialize();

	void Tackle(float tackleSpeed);

	void ExTackleInitialize();

	void ExTackle(float tackleSpeed);	

	void ExTackle2(float tackleSpeed);	

	void NormalAttack();

	void SecondAttack();

	void FlyAttack(float bulletSpeed);

	// 振る舞い
	enum class Behavior {
		kFirst,   // 第一形態
		kSecond, // 第二形態
		kThird,   // 第三形態
		kLast,   // 最終形態
		kDead,   // 死亡アニメーション
	};

	enum class Attack {
		Normal,
		Tackle

	};

	Attack attack_ = Attack::Tackle;

	Behavior behavior_ = Behavior::kFirst;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// 第一形態行動初期化
	void BehaviorFirstInitialize();
	// 第二形態行動初期化
	void BehaviorSecondInitialize();
	// 第三形態行動初期化
	void BehaviorThirdInitialize();
	// 最終形態行動初期化
	void BehaviorLastInitialize();
	// 死亡アニメーション初期化
	void BehaviorDeadInitialize();
	// 第一形態行動更新
	void BehaviorFirstUpdate();
	// 第二形態行動更新
	void BehaviorSecondUpdate();
	// 第三形態行動更新
	void BehaviorThirdUpdate();
	// 最終形態行動更新
	void BehaviorLastUpdate();
	//  死亡アニメーション更新
	void BehaviorDeadUpdate();

public:
	~Enemy();
	// 初期化
	void Initialize(const std::vector<Model*>& models) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(const ViewProjection& viewProjection) override;

	void ApplyGlobalVariables();

	///< summary>
	/// 射撃攻撃
	///</summary>
	 
	
	// 弾のエフェクト
	void HitEffect();

	OBB& GetOBB() { return obb; }

	float GetEnemyLife() { return EnemyLife; }

	float GetEnemyLifePer() { return enemyLifePer; }

	bool GetEnemyDead() { return isDead; }

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() { return bullets_; }

	void SetchackCollision() { chackCollision = 0; }

	void SetTarget(const WorldTransform* target) { target_ = target; }

	float radius = size * 2.5f;

	bool isDead = false;

	Vector3 GetMyWorldPosition() {
		Vector3 worldPos(0, 0, 0);

		worldPos.x = worldTransform_.matWorld_.m[3][0];
		worldPos.y = worldTransform_.matWorld_.m[3][1] + 9.0f;
		worldPos.z = worldTransform_.matWorld_.m[3][2];

		return worldPos;
	}

	int GetDamage() { return damage_; };

	// effectモデル
	Model* model_ = nullptr;

	//与えるダメージ
	int damage_=10;

	//口を開いているか
	bool isOpen = false;

	void OnCollision();
};
