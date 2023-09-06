#pragma once
#include"BaseCharacter.h"
#include"Input.h"
#include<optional>
#include"PlayerBullet.h"
#include"Adjustment_Item.h"
#include <Sprite.h>

class Player : public BaseCharacter {
public:
	~Player();

	// 初期化
	void Initialize(const std::vector<Model*>& models) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(const ViewProjection& viewProjection) override;

	// UI描画
	void DrawUI();

	//浮遊ギミック初期化
	void InitializeFloatingGimmick();

	//浮遊ギミックの初期化更新
	void UpdateFloatingGimmick();

	// 腕ぶらぶらギミックの初期化
	void initializeMoveArm();

	//腕ぶらぶらギミックの更新
	void UpdateMoveArm();

	//射撃の時のレティクル
	void ShotReticle(const Matrix4x4& matView, const Matrix4x4& matProjection);

	///< summary>
	/// 射撃攻撃
	///</summary>
	void Attack();
		
	//通常行動初期化
	void BehaviorRootInitialize();
	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//ダッシュ行動初期化
	void BehaviorDashInitialize();
	//射撃攻撃行動初期化
	void BehaviorShotInitialize();
	// 通常行動更新
	void BehaviorRootUpdate();
	//攻撃行動更新
	void BehaviorAttackUpdate();
	//ダッシュ行動更新
	void BehaviorDashUpdate();
	//射撃攻撃行動更新
	void BehaviorShotUpdate();

	void SetViewProjection(const ViewProjection* viewprojection) {
		viewProjection_ = viewprojection;
	}

	WorldTransform& GetBodyWorldTransform() { return worldTransformBody_; }
	//調整項目の適用
	void ApplyGlobalVariables();

	OBB& GetOBB() { return obb; }

	const WorldTransform& GetBodyWorldPosition() { return worldTransformBody_; }

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() { return bullets_; }

	Vector3 GetWorldPosition(Matrix4x4 mat);

	float GetPlayerLife() { return PlayerLife; }

	float GetPlayerLifePer() {
		return (PlayerLife / kPlayerLifeMax);
	}

	int GetBulletNum() { return BulletNum; }

	int GetBulletNumMax() { return kBulletNum; }

	void SetchackCollision() { chackCollision = 0; }

	void OnCollision();
	
	void DrawImgui();

private:

	const float MoveMax = 237.00000f;

	OBB obb = {
	    .center{0.0f,0.0f,0.0f},
	    .orientations = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	    .size{3.5f,5.0f,1.5f},
	};

	// ワールド変換データ
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformWeapon_;

	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;	

	// モデル
	Model* model_ = nullptr;

	// キーボード入力
	Input* input_ = nullptr;
	// キャラクターの移動ベクトル
	Vector3 move = {0.0f, 0.0f, 0.0f};
	//目標角度
	float target_angle = 0.0f;
	//ダッシュ時のスピード倍率
	const float kDashSpeed = 15.0f;	
	// 行列の作成
	MyMatrix matrix;
	// ベクトルの計算
	MyVector vector;
	//モデルの大きさ
	float size = 1.0f;

	const float kPlayerLifeMax = 50.0f; 

	float PlayerLife = 0.0f;

	const int kDashCoolTime = 20;

	int dashCoolTime;

	Vector3 scale = {size, size, size};
	
	Vector3 headScale = {0.9f, 0.9f, 0.9f};

	Vector3 bodyScale = {0.9f, 0.9f, 0.9f};

	Vector3 leftArmScale = {
	    0.7f,
	    1.0f,
	    0.7f,
	};
	//向きをそろえる
	Matrix4x4 minusMatrix{0};

	Vector3 minusVelocity{0, 0, 0};

	Vector3 velocityZ{0, 0, 0};
	//コントローラーの入力
	XINPUT_STATE joyState;

	//浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;
	// 浮遊移動のサイクル
	int floatingCycle_ = 0;
	// 浮遊の振幅(ｍ)
	float floatingAmplitude = 0;

	// 腕の媒介変数
	float armParameter_ = 0.0f;
	// 腕移動のサイクル
	int armPeriod = 0;
	//腕のふり幅
	float armAmplitude = 0;

	// 地面からの距離
	float disGround = 0;

	//キャラクターのスピード調整変数
	float kCharacterSpeedBase = 0.5f;
	// キャラクターの移動速さ
	float kCharacterSpeed = 0.0f;
	//パーツのあれこれ
	Vector3 Head_offset;
	Vector3 L_arm_offset;
	Vector3 R_arm_offset;
	Vector3 Weapon_offset;

	Vector3 Head_offset_Base = {0.0f, 4.5f, 0.0f};
	Vector3 L_arm_offset_Base = {1.4f, 3.5f, 0.0f};
	Vector3 R_arm_offset_Base = {-1.4f, 3.5f, 0.0f};
	Vector3 Weapon_offset_Base = {0.0f, 3.5f, 0.0f};


	//武器の回転
	float weapon_Rotate = 0.0f;
	float arm_Rotate = -3.15f;
	//武器開店に関連する変数
	const float moveWeapon = 0.1f;
	const float moveWeaponShakeDown = 0.2f;
	const float MaxRotate = 1.55f;
	const float MinRotate = -0.6f;

	int WaitTimeBase = 20;
	int WaitTime = 0;

	bool isShakeDown = false;

	//振る舞い
	enum class Behavior {
		kRoot,		//通常状態
		kAttack,	//攻撃中
		kDash,		//ダッシュ中
		kShot,		//射撃攻撃中
	};
	
	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
	};

	WorkDash workDash_;
	//当たってるか調べるためのやつ
	int chackCollision;

	/*弾関連変数群*/
	// 2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 弾
	std::list<PlayerBullet*> bullets_;

	Vector3 reticleScale_ = {size / 2.0f, size / 2.0f, size / 2.0f};

	// 発射の時間
	int bulletTime = 0;

	// 自機の弾の発射間隔
	int bulletInterval = 10;

	int Life = 0;

	// 発射タイマー
	int32_t ChargeTimer = 0;


	// 弾の数
	int BulletNum = 0;
	//弾の最大数
	const int kBulletNum = 50;
	//現在の弾を調べる関数
	int CheckBullet();

};
