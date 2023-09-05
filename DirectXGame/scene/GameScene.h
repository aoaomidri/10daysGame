#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include"Enemy.h"
#include"SkyDome.h"
#include"Ground.h"
#include"Wall.h"
#include "DebugCamera.h"
#include"FollowCamera.h"
#include"EnemyCamera.h"
#include<filesystem>
#include<sstream>
#include<memory>
#include<imgui.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

    bool IsCollisionViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum);

	bool IsCollisionOBB(const OBB& obb, const ViewingFrustum& viewingFrustum);

	bool IsCollisionOBBViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum);

	bool isCollisionOBBSphere(const OBB& obb, const Sphere& sphere);

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	Sprite* sprite_[14] = {0};

	Sprite* conSprite_[13] = {0};

	Sprite* titleSprite_ = nullptr;

	Sprite* PLSprite_ = nullptr;
	Sprite* ENSprite_ = nullptr;


	Vector4 colorChangeEN = {0};
	Vector4 colorChangePL = {0};

	// テクスチャハンドル
	uint32_t textureHandle = 0;
	uint32_t textureHandleSkydome = 0;
	uint32_t textureHandleGround = 0;
	uint32_t textureHandleWall = 0;
	uint32_t textureHandlePlayer = 0;
	uint32_t textureHandleEnemy = 0;
	uint32_t textureHamdleEnemyparts = 0;
	uint32_t textureHandleWeapon = 0;
	uint32_t textureHandleTitle = 0;

	uint32_t textureHandlePL = 0;
	uint32_t textureHandleEN = 0;

	uint32_t textureHandleBackGround = 0;
	uint32_t textureHandleBackGround2 = 0;

	uint32_t textureHandleText[10] = {0};

	uint32_t textureHandleCon[10] = {0};

	//音楽データハンドル
	uint32_t TitleBGMDataHandle_ = 0;
	uint32_t MainBGMDataHandle_ = 0;
	uint32_t EndBGMDataHandle_ = 0;

	uint32_t SEDataHandle_ = 0;

	//音楽再生用ハンドル
	uint32_t TitleBGMHandle_ = 0;
	uint32_t MainBGMHandle_ = 0;
	uint32_t EndBGMHandle_ = 0;

	uint32_t ENSEHandle_ = 0;

	// 3Dモデル
	std::unique_ptr<Model> modelSkyDome_;

	std::unique_ptr<Model> modelGround_;

	std::unique_ptr<Model> modelWall_;
	/*プレイヤーのモデル*/
	std::unique_ptr<Model> modelPlayerBody_;
	std::unique_ptr<Model> modelPlayerHead_;
	std::unique_ptr<Model> modelPlayerL_arm_;
	std::unique_ptr<Model> modelPlayerR_arm_;
	std::unique_ptr<Model> modelPlayerBullet_;
	/*エネミーのモデル*/
	std::unique_ptr<Model> modelEnemy_;
	std::unique_ptr<Model> modelEnemyL_parts_;
	std::unique_ptr<Model> modelEnemyR_parts_;
	/*武器のモデル*/
	std::unique_ptr<Model> modelPlayerWeapon_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	WorldTransform worldTransform_;

	// 自キャラ
	std::unique_ptr<Player> player_;

	//敵キャラ
	std::unique_ptr<Enemy> enemy_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_;

	//プレイヤーを追うカメラ
	std::unique_ptr<FollowCamera> followCamera_;

	//エネミーのカメラ
	std::unique_ptr<EnemyCamera> enemyCamera_;

	//天球
	std::unique_ptr<SkyDome> skyDome_;

	//地面
	std::unique_ptr<Ground> ground_;

	//壁
	std::unique_ptr<Wall> wall_;

	// コントローラーの入力
	XINPUT_STATE joyState;

	XINPUT_STATE preJoyState;

	enum class Scene {
		Title,		//タイトル
		Control,	//操作説明
		Main,		//ゲーム
		Pose,		//ポーズ画面
		End,		//リザルト
		GameOver	//ゲームオーバー
	};

	Scene scene_ = Scene::Title;

	Scene BeforeScene_;

	std::optional<Scene> sceneRequest_ = Scene::Title;

	int selectMode = 0;

	void TitleInitialize();

	void TitleUpdate();

	void ControlInitialize();

	void ControlUpdate();

	void MainInitialize();

	void MainUpdate();

	void PoseInitialize();

	void PoseUpdate();

	void EndInitialize();

	void EndUpdate();

	void GameOverInitialize();

	void GameOverUpdate();

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
