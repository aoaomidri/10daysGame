#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"
#include<imgui.h>
#include<fstream>
#include<iostream>
#include <algorithm>
#include <cmath>
GameScene::GameScene() {}

GameScene::~GameScene() {
	for (int i = 0; i < 13; i++) {
		delete sprite_[i];
		delete conSprite_[i];
	}
	delete sprite_[13];
	for (int i = 0; i < 4; i++) {
		delete NumberSprite_[i];
	}
	delete slashSprite_;
	delete titleSprite_;
	delete PLSprite_;
	delete ENSprite_;
}

int GameScene::CheckTensPlaceNumber(int num) { return num / 10; }
int GameScene::CheckOensPlaceNumber(int num) { return (num % 10); }

void GameScene::TextureInitialize() {
	// 画像データ読み込み
	textureHandle = TextureManager::Load("white1x1.png");
	textureHandleSkydome = TextureManager::Load("skyDome/skyDome.jpg");
	textureHandleGround = TextureManager::Load("Ground/firld.png");
	textureHandleWall = TextureManager::Load("Wall/renga.png");
	textureHandlePlayer = TextureManager::Load("Player/PlayerTex.png");
	textureHandleEnemy = TextureManager::Load("Enemy/EnemyTex.png");
	textureHamdleEnemyparts = TextureManager::Load("EnemyParts/EnemyParts.png");
	textureHandleWeapon = TextureManager::Load("Weapon/Sword.png");
	TextureManager::Load("Magic.png");

	textureHandleBackGround = TextureManager::Load("backGround/BackGround.png");
	textureHandleBackGround2 = TextureManager::Load("backGround/Sky.png");

	textureHandleText[0] = TextureManager::Load("text/PRESS.png");
	textureHandleText[1] = TextureManager::Load("text/B.png");
	textureHandleText[2] = TextureManager::Load("text/continue.png");
	textureHandleText[3] = TextureManager::Load("text/end.png");
	textureHandleText[4] = TextureManager::Load("text/pose.png");
	textureHandleText[5] = TextureManager::Load("text/GAMECLEAR.png");
	textureHandleText[6] = TextureManager::Load("arrow.png");
	textureHandleText[7] = TextureManager::Load("text/GAMEOVER.png");
	textureHandleText[8] = TextureManager::Load("text/retry.png");
	textureHandleText[9] = TextureManager::Load("text/PRESS.png");

	textureHandleCon[0] = TextureManager::Load("Contro.png");
	textureHandleCon[1] = TextureManager::Load("text/attack.png");
	textureHandleCon[2] = TextureManager::Load("text/control.png");
	textureHandleCon[3] = TextureManager::Load("text/dash.png");
	textureHandleCon[4] = TextureManager::Load("text/set.png");
	textureHandleCon[5] = TextureManager::Load("text/move.png");
	textureHandleCon[6] = TextureManager::Load("text/camera.png");

	textureHandleTitle = TextureManager::Load("text/title.png");

	textureHandlePL = TextureManager::Load("text/PL.png");
	textureHandleEN = TextureManager::Load("text/EN.png");

	textureHandleNumber[0] = TextureManager::Load("Number/number0.png");
	textureHandleNumber[1] = TextureManager::Load("Number/number1.png");
	textureHandleNumber[2] = TextureManager::Load("Number/number2.png");
	textureHandleNumber[3] = TextureManager::Load("Number/number3.png");
	textureHandleNumber[4] = TextureManager::Load("Number/number4.png");
	textureHandleNumber[5] = TextureManager::Load("Number/number5.png");
	textureHandleNumber[6] = TextureManager::Load("Number/number6.png");
	textureHandleNumber[7] = TextureManager::Load("Number/number7.png");
	textureHandleNumber[8] = TextureManager::Load("Number/number8.png");
	textureHandleNumber[9] = TextureManager::Load("Number/number9.png");
	textureHandleNumber[10] = TextureManager::Load("Number/slash.png");

}

void GameScene::MakeTexture() {
	// テクスチャ生成
	titleSprite_ = Sprite::Create(textureHandleTitle, {640, 200}, {1, 1, 1, 1}, {0.5f, 0.5f});
	titleSprite_->SetSize({672, 130});

	sprite_[0] = Sprite::Create(textureHandleBackGround, {0, 0});
	sprite_[13] = Sprite::Create(textureHandleBackGround2, {0, 0});
	sprite_[13]->SetSize({1280, 720});
	sprite_[1] = Sprite::Create(textureHandle, {0, 0}, {0, 0, 0, 0.0f});
	sprite_[1]->SetSize({1280, 720});

	colorChangeEN = {0, 1, 0, 0.5f};

	sprite_[2] = Sprite::Create(textureHandle, {0, 0}, colorChangeEN);
	sprite_[12] = Sprite::Create(textureHandle, {560, 650}, {0, 0, 1, 0.5f});
	// テキスト系
	sprite_[3] = Sprite::Create(textureHandleText[0], {570, 500}, {1, 1, 1, 1}, {0.5f, 0.5f});
	sprite_[4] = Sprite::Create(textureHandleText[1], {750, 500}, {1, 1, 1, 1}, {0.5f, 0.5f});
	sprite_[5] =
	    Sprite::Create(textureHandleText[2], {640, 350}, {0.3f, 0.3f, 0.3f, 0.9f}, {0.5f, 0.5f});
	sprite_[5]->SetSize({284.0f, 134.0f});

	sprite_[6] =
	    Sprite::Create(textureHandleText[3], {640, 550}, {0.3f, 0.3f, 0.3f, 0.9f}, {0.5f, 0.5f});
	sprite_[6]->SetSize({284.0f, 134.0f});

	sprite_[7] =
	    Sprite::Create(textureHandleText[4], {640, 100}, {0.3f, 0.3f, 0.3f, 0.9f}, {0.5f, 0.5f});
	sprite_[7]->SetSize({362.0f, 138.0f});
	// 256//128
	sprite_[8] = Sprite::Create(textureHandleText[5], {640, 200}, {1, 1, 1, 1}, {0.5f, 0.5f});
	sprite_[8]->SetSize({384.0f, 192.0f});

	sprite_[9] = Sprite::Create(textureHandleText[6], {450, 350}, {1, 1, 1, 1}, {0.5f, 0.5f});

	sprite_[10] =
	    Sprite::Create(textureHandleText[7], {640, 100}, {0.3f, 0.3f, 0.3f, 0.9f}, {0.5f, 0.5f});
	sprite_[10]->SetSize({640.0f, 128.0f});

	sprite_[11] =
	    Sprite::Create(textureHandleText[8], {640, 350}, {0.3f, 0.3f, 0.3f, 0.9f}, {0.5f, 0.5f});
	sprite_[11]->SetSize({284.0f, 134.0f});

	conSprite_[0] = Sprite::Create(textureHandleCon[0], {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});
	conSprite_[1] = Sprite::Create(textureHandleCon[1], {780, 160}, {1, 1, 1, 1}, {0.5f, 0.5f});
	conSprite_[2] = Sprite::Create(textureHandleCon[2], {140, 60}, {1, 1, 1, 1}, {0.5f, 0.5f});
	conSprite_[3] = Sprite::Create(textureHandleCon[3], {780, 230}, {1, 1, 1, 1}, {0.5f, 0.5f});
	conSprite_[4] = Sprite::Create(textureHandleCon[4], {500, 160}, {1, 1, 1, 1}, {0.5f, 0.5f});
	conSprite_[5] = Sprite::Create(textureHandleCon[5], {480, 310}, {1, 1, 1, 1}, {0.5f, 0.5f});
	conSprite_[6] = Sprite::Create(textureHandleCon[6], {800, 380}, {1, 1, 1, 1}, {0.5f, 0.5f});

	PLSprite_ = Sprite::Create(textureHandlePL, {500, 670}, {1, 1, 1, 1}, {0.5f, 0.5f});
	ENSprite_ = Sprite::Create(textureHandleEN, {50, 70}, {1, 1, 1, 1}, {0.5f, 0.5f});

	NumberSprite_[0] =
	    Sprite::Create(textureHandleNumber[0], {1100, 600}, {1, 1, 1, 1}, {0.5f, 0.5f});
	NumberSprite_[1] =
	    Sprite::Create(textureHandleNumber[1], {1148, 600}, {1, 1, 1, 1}, {0.5f, 0.5f});
	NumberSprite_[2] =
	    Sprite::Create(textureHandleNumber[2], {1200, 680}, {1, 1, 1, 1}, {0.5f, 0.5f});
	NumberSprite_[3] =
	    Sprite::Create(textureHandleNumber[3], {1248, 680}, {1, 1, 1, 1}, {0.5f, 0.5f});
	
	for (int i = 0; i < 4; i++) {
		NumberSprite_[i]->SetSize({48.0f, 48.0f}); 
	}

	slashSprite_ = Sprite::Create(textureHandleNumber[10], {1170, 640}, {1, 1, 1, 1}, {0.5f, 0.5f});
	slashSprite_->SetSize({144.0f, 144.0f});

}

void GameScene::SoundInitialize() {
	// サウンドデータ読み込み
	TitleBGMDataHandle_ = audio_->LoadWave("audio/8bit13.wav");
	MainBGMDataHandle_ = audio_->LoadWave("audio/Game3.wav");
	EndBGMDataHandle_ = audio_->LoadWave("audio/zingle.wav");

	SEDataHandle_ = audio_->LoadWave("audio/break.wav");
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	
	TextureInitialize();

	SoundInitialize();
	
	MakeTexture();
	

	//モデル生成
	modelSkyDome_.reset(Model::CreateFromOBJ("skyDome", true));
	modelGround_.reset(Model::CreateFromOBJ("Ground", true));
	modelWall_.reset(Model::CreateFromOBJ("Wall", true));
	modelPlayerBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelPlayerTail_.reset(Model::CreateFromOBJ("float_Tail", true));
	modelPlayerL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelEnemy_.reset(Model::CreateFromOBJ("Enemy", true));
	modelEnemyL_parts_.reset(Model::CreateFromOBJ("EnemyParts", true));
	modelEnemyR_parts_.reset(Model::CreateFromOBJ("EnemyParts", true));
	modelPlayerBulletBody_.reset(Model::CreateFromOBJ("BulletBody", true));
	modelPlayerBulletFin_.reset(Model::CreateFromOBJ("tail", true));
	modelWaterFlowParticle_.reset(Model::CreateFromOBJ("WaterFlowParticle", true));


	viewProjection_.farZ = 2000.0f;
	viewProjection_.Initialize();

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// 自キャラの生成
	player_ = std::make_unique<Player>();
	//自キャラモデル配列
	std::vector<Model*> playerModels = {modelPlayerBody_.get(),   modelPlayerTail_.get(),
	                                    modelPlayerL_arm_.get(),  modelPlayerBulletBody_.get(),
	                                    modelPlayerBulletFin_.get(), modelWaterFlowParticle_.get()};
	// 自キャラの初期化
	player_->Initialize(playerModels);

	//敵キャラの生成
	enemy_ = std::make_unique<Enemy>();
	//敵キャラモデル配列
	std::vector<Model*> enemyModels = {
	    modelEnemy_.get(), 
		modelEnemyL_parts_.get(), 
		modelEnemyR_parts_.get()
	};
	//敵キャラの初期化
	enemy_->Initialize(enemyModels);
	enemy_->SetTarget(&player_->GetBodyWorldPosition());

	//天球の生成
	skyDome_ = std::make_unique<SkyDome>();
	//天球のモデル配列
	std::vector<Model*> skyDomeModels = {modelSkyDome_.get()};
	//天球の初期化
	skyDome_->Initialize(skyDomeModels);

	//地面の生成
	ground_ = std::make_unique<Ground>();
	//地面のモデル配列
	std::vector<Model*> groundModels_ = {modelGround_.get()};
	//地面の初期化
	ground_->Initialize(groundModels_);

	//壁の生成
	wall_ = std::make_unique<Wall>();
	//壁のモデル配列
	std::vector<Model*> wallModels_ = {
	    modelWall_.get(), modelWall_.get(), modelWall_.get(), modelWall_.get()};
	//壁の初期化
	wall_->Initialize(wallModels_);

	// デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());

	enemyCamera_ = std::make_unique<EnemyCamera>();
	enemyCamera_->Initialize();
	//エネミーのワールドトランスフォームをカメラにセット
	enemyCamera_->SetTarget(&enemy_->GetWorldTransform());

	player_->SetViewProjection(&followCamera_->GetViewProjection());

	//プレイヤー弾に敵のworldTransformをセット
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	for (PlayerBullet* bullet : playerBullets) {
		bullet->setTarget(&enemy_->GetWorldTransform());
	}

#ifdef _DEBUG
	////軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	////軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
#endif

}

void GameScene::Update() { 
	if (sceneRequest_) {
		// 振る舞いを変更する
		scene_ = sceneRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (scene_) {
		case Scene::Title:
			if (BeforeScene_ != Scene::Control) {
				TitleInitialize();
			}
			break;
		case Scene::Control:
			ControlInitialize();
			break;
		case Scene::Main:
			if (BeforeScene_!=Scene::Pose) {
				MainInitialize();
			}			
			break;
		case Scene::Pose:
			PoseInitialize();
			break;
		case Scene::End:
			EndInitialize();
			break;
		case Scene::GameOver:
			GameOverInitialize();
			break;
		}
	}
	BeforeScene_ = scene_;
	// 振る舞いリクエストをリセット
	sceneRequest_ = std::nullopt;

	switch (scene_) {
	case Scene::Title:
	default:
		TitleUpdate();
		break;
	case Scene::Main:
		MainUpdate();
		break;
	case Scene::Control:
		ControlUpdate();
		break;
	case Scene::Pose:
		PoseUpdate();
		break;
	case Scene::End:
		EndUpdate();
		break;
	case Scene::GameOver:
		GameOverUpdate();
		break;
	}
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) {
			selectMode--;
		}
	} 
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) {
			selectMode++;
		}
	}

	selectMode = (selectMode % 2) * (selectMode % 2);

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		preJoyState = joyState;
	}

	

	#ifdef _DEBUG
	/*if (Input::GetInstance()->GetJoystickState(0, joyState)) {
	    if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
	        isDebugCameraActive_ = !isDebugCameraActive_;
	    }
	}*/

	if (input_->TriggerKey(DIK_RETURN)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	ImGui::Begin("SELECTMODE");
	ImGui::Text("select = %d", selectMode);
	ImGui::End();

	ImGui::Begin("CheckBox");
	ImGui::Text("TensPlace = %d", CheckTensPlaceNumber(player_->GetBulletNum()));
	ImGui::Text("OensPlace = %d", CheckOensPlaceNumber(player_->GetBulletNum()));
	ImGui::End();
	/*ImGui::Begin("CameraInforMation");
	ImGui::DragFloat3("CameraRotate", &followCamera_->GetViewProjection().rotation_.x, 0.1f);
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();*/

#endif // _DEBUG

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	if (scene_ == Scene::Main || scene_ == Scene::Pose || scene_ == Scene::GameOver) {

		player_->Draw(viewProjection_);

		enemy_->Draw(viewProjection_);

		skyDome_->Draw(viewProjection_);

		ground_->Draw(viewProjection_);

		//wall_->Draw(viewProjection_);
	}
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	if (scene_ == Scene::Title || scene_ == Scene::Control) {
		
		sprite_[0]->Draw();
		//titleSprite_->Draw();
		sprite_[1]->Draw();
		sprite_[1]->SetColor({0, 0, 0, 0.1f});
		sprite_[3]->Draw();
		sprite_[4]->Draw();
	}
	if (scene_==Scene::Control) {
		sprite_[1]->SetColor({0, 0, 0, 0.8f});
		conSprite_[0]->Draw();
		conSprite_[1]->Draw();
		conSprite_[2]->Draw();
		conSprite_[3]->Draw();
		conSprite_[4]->Draw();
		conSprite_[5]->Draw();
		conSprite_[6]->Draw();
	}

	if (scene_ == Scene::Main || scene_ == Scene::Pose||scene_==Scene::GameOver) {
		sprite_[2]->SetSize({(enemy_->GetEnemyLifePer()), 36.0f});
		if (enemy_->GetEnemyLife()==170) {
			colorChangeEN = {1, 1, 0, 0.5};
		} else if (enemy_->GetEnemyLife() == 80) {
			colorChangeEN = {1, 0, 0, 0.5};
		} 
		sprite_[2]->SetColor(colorChangeEN);

		sprite_[2]->Draw();
		
		sprite_[12]->SetSize({(player_->GetPlayerLifePer() * 256.0f), 36.0f});
		sprite_[12]->Draw();

		NumberSprite_[0]->SetTextureHandle(
		    textureHandleNumber[CheckTensPlaceNumber(player_->GetBulletNum())]);
		NumberSprite_[0]->Draw();
		NumberSprite_[0]->SetPosition(numberTensPlacePosUP);

		NumberSprite_[1]->SetTextureHandle(
		    textureHandleNumber[CheckOensPlaceNumber(player_->GetBulletNum())]);
		NumberSprite_[1]->Draw();
		NumberSprite_[1]->SetPosition(numberOnesPlacePosUP);

		NumberSprite_[2]->SetTextureHandle(
		    textureHandleNumber[CheckTensPlaceNumber(player_->GetBulletNumMax())]);
		NumberSprite_[2]->Draw();
		NumberSprite_[2]->SetPosition(numberTensPlacePosDOWN);


		NumberSprite_[3]->SetTextureHandle(
		    textureHandleNumber[CheckOensPlaceNumber(player_->GetBulletNumMax())]);
		NumberSprite_[3]->Draw();
		NumberSprite_[3]->SetPosition(numberOnesPlacePosDOWN);

		slashSprite_->Draw();

		PLSprite_->Draw();
		ENSprite_->Draw();

		player_->DrawUI();
	}

	if (scene_==Scene::Pose) {
		sprite_[1]->SetColor({0, 0, 0, 0.8f});
		sprite_[1]->Draw();
		sprite_[5]->Draw();
		sprite_[6]->Draw();
		sprite_[7]->Draw();
		sprite_[9]->Draw();
		sprite_[9]->SetPosition({450.0f, (350.0f + (selectMode * 200.0f))});
	}

	if (scene_ == Scene::GameOver) {
		sprite_[1]->SetColor({0, 0, 0, 0.8f});
		sprite_[1]->Draw();
		sprite_[6]->Draw();
		sprite_[9]->Draw();
		sprite_[9]->SetPosition({450.0f, (350.0f + (selectMode * 200.0f))});
		sprite_[10]->Draw();
		sprite_[11]->Draw();
	}
	if (scene_==Scene::End) {
		sprite_[13]->Draw();
		sprite_[3]->Draw();
		sprite_[4]->Draw();
		sprite_[8]->Draw();
		
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {

	if (IsCollisionOBBViewFrustum(enemy_->GetOBB(), followCamera_->GetViewingFrustum())) {
		player_->SetcheckCameraHit();
	} else {
		player_->SetcheckCameraNoHit();
	}

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

#pragma region 自弾と全ての敵キャラの当たり判定
	if (enemy_->isDead == false) {

		for (PlayerBullet* bullet : playerBullets) {
			Sphere playerBullet_{.center = bullet->GetWorldPosition(), .radius = bullet->radius};
			if (isCollisionOBBSphere(enemy_->GetOBB(), playerBullet_)) {
				if (bullet->GetState() == PlayerBullet::PlayerBulletState::Move)
				{
					enemy_->OnCollision();
				}
				bullet->OnCollision();
				ENSEHandle_ = audio_->PlayWave(SEDataHandle_);
			}
		}
	}
#pragma endregion

	#pragma region 自機と敵弾の当たり判定
	if (enemy_->isDead == false) {

		for (EnemyBullet* bullet : enemyBullets) {
			Sphere enemyBullet_{
				.center = bullet->GetWorldPosition(), 
				.radius = bullet->radius
			};
			if (isCollisionOBBSphere(player_->GetOBB(),enemyBullet_)) {
				bullet->OnCollision();
				player_->OnCollision();
			}


		}
	}
#pragma endregion


#pragma region 自機と敵の当たり判定
	if (enemy_->isDead == false) {

		if (IsCollisionOBBOBB(player_->GetOBB(), enemy_->GetOBB())) {
				//bullet->OnCollision();
				player_->OnCollision();
			}
	}
#pragma endregion
}

bool GameScene::IsCollisionViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	std::unique_ptr<MyVector> vec = std::make_unique<MyVector>();
	std::unique_ptr<MyMatrix> mat = std::make_unique<MyMatrix>();
	/*ステップ1視錐台の生成*/
	// 頂点の個数
	const int OBBVertex = 8;
	const int FrustumVertex = 8;

	// 法線の個数
	const int normalLine = 6;

	// 当たる距離
	const float CollisionDistance = 0.00f;

	// それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};
	// 面の頂点
	Vector3 nearPlanePoints_[4] = {0};
	Vector3 farPlanePoints_[4] = {0};
	// 視錐台の行列
	Matrix4x4 FrustumMatWorld = mat->MakeAffineMatrix(
	    {1.0f, 1.0f, 1.0f}, {viewingFrustum.rotate_}, {viewingFrustum.translation_});

	// 向きベクトルnear
	Vector3 directionNear = vec->Normalize(viewingFrustum.direction);
	directionNear = directionNear * viewingFrustum.nearZ;
	// 向きベクトルfar
	Vector3 directionFar = vec->Normalize(viewingFrustum.direction);
	directionFar = directionFar * viewingFrustum.farZ;

	// 近平面の縦横
	nearPlane.y = vec->Length(directionNear) * std::tan(viewingFrustum.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum.aspectRatio;
	// 遠平面の縦横
	farPlane.y = vec->Length(directionFar) * std::tan(viewingFrustum.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum.aspectRatio;

	nearPlanePoints_[0] = {
	    directionNear.x + -nearPlane.x, directionNear.y + nearPlane.y, directionNear.z}; // 左上
	nearPlanePoints_[1] = {
	    directionNear.x + nearPlane.x, directionNear.y + nearPlane.y, directionNear.z}; // 右上
	nearPlanePoints_[2] = {
	    directionNear.x + -nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z}; // 左下
	nearPlanePoints_[3] = {
	    directionNear.x + nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z}; // 右下

	farPlanePoints_[0] = {
	    directionFar.x + -farPlane.x, directionFar.y + farPlane.y, directionFar.z}; // 左上
	farPlanePoints_[1] = {
	    directionFar.x + farPlane.x, directionFar.y + farPlane.y, directionFar.z}; // 右上
	farPlanePoints_[2] = {
	    directionFar.x + -farPlane.x, directionFar.y + -farPlane.y, directionFar.z}; // 左下
	farPlanePoints_[3] = {
	    directionFar.x + farPlane.x, directionFar.y + -farPlane.y, directionFar.z}; // 右下

	// 頂点
	Vector3 FrustumPoints[FrustumVertex]{0};
	// near
	FrustumPoints[0] = nearPlanePoints_[0];
	FrustumPoints[1] = nearPlanePoints_[1];
	FrustumPoints[2] = nearPlanePoints_[2];
	FrustumPoints[3] = nearPlanePoints_[3];
	// far
	FrustumPoints[4] = farPlanePoints_[0];
	FrustumPoints[5] = farPlanePoints_[1];
	FrustumPoints[6] = farPlanePoints_[2];
	FrustumPoints[7] = farPlanePoints_[3];

	/*ステップ2 OBBの生成*/

	Vector3 obbPoints[OBBVertex]{};

	// obbの行列
	Matrix4x4 worldMatrix = {
	    obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
	    obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
	    obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
	    obb.center.x,          obb.center.y,          obb.center.z,          1};

	obbPoints[0] = obb.size * -1;
	obbPoints[1] = {obb.size.x * -1, obb.size.y * -1, obb.size.z};
	obbPoints[2] = {obb.size.x, obb.size.y * -1, obb.size.z * -1};
	obbPoints[3] = {obb.size.x, obb.size.y * -1, obb.size.z};
	obbPoints[4] = {obb.size.x * -1, obb.size.y, obb.size.z * -1};
	obbPoints[5] = {obb.size.x * -1, obb.size.y, obb.size.z};
	obbPoints[6] = {obb.size.x, obb.size.y, obb.size.z * -1};
	obbPoints[7] = obb.size;

	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = vec->TransformNormal(obbPoints[i], worldMatrix);
		obbPoints[i] = obb.center + obbPoints[i];
	}

	/*ステップ3 OBBを視錐台のローカル座標に変換*/
	// 視錐台の逆行列
	Matrix4x4 FrustumInverceMat = mat->Inverce(FrustumMatWorld);
	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = vec->Transform(obbPoints[i], FrustumInverceMat);
	}

	/*ステップ4 当たり判定*/
	// near面から
	Vector3 v01 = FrustumPoints[1] - FrustumPoints[0];
	Vector3 v12 = FrustumPoints[2] - FrustumPoints[1];

	// far
	Vector3 v65 = FrustumPoints[5] - FrustumPoints[6];
	Vector3 v54 = FrustumPoints[4] - FrustumPoints[5];

	// left
	Vector3 v02 = FrustumPoints[2] - FrustumPoints[0];
	Vector3 v26 = FrustumPoints[6] - FrustumPoints[2];

	// right
	Vector3 v53 = FrustumPoints[3] - FrustumPoints[5];
	Vector3 v31 = FrustumPoints[1] - FrustumPoints[3];

	// up
	Vector3 v41 = FrustumPoints[1] - FrustumPoints[4];
	Vector3 v10 = FrustumPoints[0] - FrustumPoints[1];

	// down
	Vector3 v23 = FrustumPoints[3] - FrustumPoints[2];
	Vector3 v36 = FrustumPoints[6] - FrustumPoints[3];

	Vector3 normal[normalLine] = {};

	float distance[48] = {};
	// near
	normal[0] = vec->Normalize(vec->Cross(v01, v12));
	// far
	normal[1] = vec->Normalize(vec->Cross(v65, v54));
	// left
	normal[2] = vec->Normalize(vec->Cross(v02, v26));
	// right
	normal[3] = vec->Normalize(vec->Cross(v53, v31));
	// up
	normal[4] = vec->Normalize(vec->Cross(v41, v10));
	// down
	normal[5] = vec->Normalize(vec->Cross(v23, v36));
	for (int i = 0; i < 8; i++) {
		distance[0 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[0], normal[0]);
		distance[1 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[4], normal[1]);
		distance[2 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[0], normal[2]);
		distance[3 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[1], normal[3]);
		distance[4 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[0], normal[4]);
		distance[5 + i * normalLine] = vec->Dot(obbPoints[i] - FrustumPoints[2], normal[5]);
	}

	for (int i = 0; i < 8; i++) {
		if (distance[0 + i * normalLine] <= CollisionDistance &&
		    distance[1 + i * normalLine] <= CollisionDistance &&
		    distance[2 + i * normalLine] <= CollisionDistance &&
		    distance[3 + i * normalLine] <= CollisionDistance &&
		    distance[4 + i * normalLine] <= CollisionDistance &&
		    distance[5 + i * normalLine] <= CollisionDistance) {
			return true;
		}
	}

	return false;
}

bool GameScene::IsCollisionOBB(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	std::unique_ptr<MyVector> vec = std::make_unique<MyVector>();
	std::unique_ptr<MyMatrix> mat = std::make_unique<MyMatrix>();
	/*ステップ1視錐台の生成*/
	// 頂点の個数
	const int OBBVertex = 8;
	const int FrustumVertex = 8;

	// 法線の個数
	const int normalLine = 6;

	// 当たる距離
	const float CollisionDistance = 0.00f;

	// それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};
	// 面の頂点
	Vector3 nearPlanePoints_[4] = {0};
	Vector3 farPlanePoints_[4] = {0};
	// 視錐台の行列
	Matrix4x4 FrustumMatWorld = mat->MakeAffineMatrix(
	    {1.0f, 1.0f, 1.0f}, {viewingFrustum.rotate_}, {viewingFrustum.translation_});

	// 向きベクトルnear
	Vector3 directionNear = vec->Normalize(viewingFrustum.direction);
	directionNear = directionNear * viewingFrustum.nearZ;
	// 向きベクトルfar
	Vector3 directionFar = vec->Normalize(viewingFrustum.direction);
	directionFar = directionFar * viewingFrustum.farZ;

	// 近平面の縦横
	nearPlane.y = vec->Length(directionNear) * std::tan(viewingFrustum.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum.aspectRatio;
	// 遠平面の縦横
	farPlane.y = vec->Length(directionFar) * std::tan(viewingFrustum.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum.aspectRatio;

	nearPlanePoints_[0] = {
	    directionNear.x + -nearPlane.x, directionNear.y + nearPlane.y, directionNear.z}; // 左上
	nearPlanePoints_[1] = {
	    directionNear.x + nearPlane.x, directionNear.y + nearPlane.y, directionNear.z}; // 右上
	nearPlanePoints_[2] = {
	    directionNear.x + -nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z}; // 左下
	nearPlanePoints_[3] = {
	    directionNear.x + nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z}; // 右下

	farPlanePoints_[0] = {
	    directionFar.x + -farPlane.x, directionFar.y + farPlane.y, directionFar.z}; // 左上
	farPlanePoints_[1] = {
	    directionFar.x + farPlane.x, directionFar.y + farPlane.y, directionFar.z}; // 右上
	farPlanePoints_[2] = {
	    directionFar.x + -farPlane.x, directionFar.y + -farPlane.y, directionFar.z}; // 左下
	farPlanePoints_[3] = {
	    directionFar.x + farPlane.x, directionFar.y + -farPlane.y, directionFar.z}; // 右下

	for (int i = 0; i < 4; i++) {
		nearPlanePoints_[i] = vec->TransformNormal(nearPlanePoints_[i], FrustumMatWorld);
		farPlanePoints_[i] = vec->TransformNormal(farPlanePoints_[i], FrustumMatWorld);
	}

	// 頂点
	Vector3 FrustumPoints[FrustumVertex]{0};
	// near
	FrustumPoints[0] = viewingFrustum.translation_ + nearPlanePoints_[0];
	FrustumPoints[1] = viewingFrustum.translation_ + nearPlanePoints_[1];
	FrustumPoints[2] = viewingFrustum.translation_ + nearPlanePoints_[2];
	FrustumPoints[3] = viewingFrustum.translation_ + nearPlanePoints_[3];
	// far
	FrustumPoints[4] = viewingFrustum.translation_ + farPlanePoints_[0];
	FrustumPoints[5] = viewingFrustum.translation_ + farPlanePoints_[1];
	FrustumPoints[6] = viewingFrustum.translation_ + farPlanePoints_[2];
	FrustumPoints[7] = viewingFrustum.translation_ + farPlanePoints_[3];

	/*ステップ2 OBBの生成*/

	Vector3 obbPoints[OBBVertex]{};

	// obbの行列
	Matrix4x4 worldMatrix = {
	    obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
	    obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
	    obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
	    obb.center.x,          obb.center.y,          obb.center.z,          1};

	// 手前
	obbPoints[0] = {obb.size.x * -1, obb.size.y, obb.size.z * -1}; // 左上
	obbPoints[1] = {obb.size.x, obb.size.y, obb.size.z * -1};      // 右上
	obbPoints[2] = obb.size * -1;                                  // 左下
	obbPoints[3] = {obb.size.x, obb.size.y * -1, obb.size.z * -1}; // 右下
	// 奥
	obbPoints[4] = {obb.size.x * -1, obb.size.y, obb.size.z};      // 左上
	obbPoints[5] = obb.size;                                       // 右上
	obbPoints[6] = {obb.size.x * -1, obb.size.y * -1, obb.size.z}; // 左下
	obbPoints[7] = {obb.size.x, obb.size.y * -1, obb.size.z};      // 右下

	/*ステップ3 視錐台をOBBのローカル座標に変換*/
	// OBBの逆行列
	Matrix4x4 OBBInverceMat = mat->Inverce(worldMatrix);
	for (int i = 0; i < OBBVertex; i++) {
		FrustumPoints[i] = vec->Transform(FrustumPoints[i], OBBInverceMat);
	}

	/*ステップ4 当たり判定*/
	// near面から
	Vector3 v01 = obbPoints[1] - obbPoints[0];
	Vector3 v12 = obbPoints[2] - obbPoints[1];

	// far
	Vector3 v65 = obbPoints[5] - obbPoints[6];
	Vector3 v54 = obbPoints[4] - obbPoints[5];

	// left
	Vector3 v02 = obbPoints[2] - obbPoints[0];
	Vector3 v26 = obbPoints[6] - obbPoints[2];

	// right
	Vector3 v53 = obbPoints[3] - obbPoints[5];
	Vector3 v31 = obbPoints[1] - obbPoints[3];

	// up
	Vector3 v41 = obbPoints[1] - obbPoints[4];
	Vector3 v10 = obbPoints[0] - obbPoints[1];

	// down
	Vector3 v23 = obbPoints[3] - obbPoints[2];
	Vector3 v36 = obbPoints[6] - obbPoints[3];

	Vector3 normal[normalLine] = {};

	float distance[48] = {};
	// near
	normal[0] = vec->Normalize(vec->Cross(v01, v12));
	// far
	normal[1] = vec->Normalize(vec->Cross(v65, v54));
	// left
	normal[2] = vec->Normalize(vec->Cross(v02, v26));
	// right
	normal[3] = vec->Normalize(vec->Cross(v53, v31));
	// up
	normal[4] = vec->Normalize(vec->Cross(v41, v10));
	// down
	normal[5] = vec->Normalize(vec->Cross(v23, v36));
	for (int i = 0; i < 8; i++) {
		distance[0 + i * normalLine] = vec->Dot(FrustumPoints[i] - obbPoints[0], normal[0]);
		distance[1 + i * normalLine] = vec->Dot(FrustumPoints[i] - obbPoints[4], normal[1]);
		distance[2 + i * normalLine] = vec->Dot(FrustumPoints[i] - obbPoints[0], normal[2]);
		distance[3 + i * normalLine] = vec->Dot(FrustumPoints[i] - obbPoints[1], normal[3]);
		distance[4 + i * normalLine] = vec->Dot(FrustumPoints[i] - obbPoints[0], normal[4]);
		distance[5 + i * normalLine] = vec->Dot(FrustumPoints[i] - obbPoints[2], normal[5]);
	}

	for (int i = 0; i < 8; i++) {
		if (distance[0 + i * normalLine] <= CollisionDistance &&
		    distance[1 + i * normalLine] <= CollisionDistance &&
		    distance[2 + i * normalLine] <= CollisionDistance &&
		    distance[3 + i * normalLine] <= CollisionDistance &&
		    distance[4 + i * normalLine] <= CollisionDistance &&
		    distance[5 + i * normalLine] <= CollisionDistance) {
			return true;
		}
	}

	return false;
}

bool GameScene::IsCollisionOBBViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	if (IsCollisionOBB(obb, viewingFrustum) || IsCollisionViewFrustum(obb, viewingFrustum)) {
		return true;
	} else {
		return false;
	}
}

bool GameScene::isCollisionOBBSphere(const OBB& obb, const Sphere& sphere) {
	std::unique_ptr<MyVector> vec = std::make_unique<MyVector>();
	std::unique_ptr<MyMatrix> mat = std::make_unique<MyMatrix>();
	/*ステップ1視錐台の生成*/
	// 頂点の個数
	const int OBBVertex = 8;
	Vector3 obbPoints[OBBVertex]{};

	// obbの行列
	Matrix4x4 worldMatrix = {
	    obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0,
	    obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0,
	    obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0,
	    obb.center.x,          obb.center.y,          obb.center.z,          1};

	// 手前
	obbPoints[0] = {obb.size.x * -1, obb.size.y, obb.size.z * -1}; // 左上
	obbPoints[1] = {obb.size.x, obb.size.y, obb.size.z * -1};      // 右上
	obbPoints[2] = obb.size * -1;                                  // 左下
	obbPoints[3] = {obb.size.x, obb.size.y * -1, obb.size.z * -1}; // 右下
	// 奥
	obbPoints[4] = {obb.size.x * -1, obb.size.y, obb.size.z};      // 左上
	obbPoints[5] = obb.size;                                       // 右上
	obbPoints[6] = {obb.size.x * -1, obb.size.y * -1, obb.size.z}; // 左下
	obbPoints[7] = {obb.size.x, obb.size.y * -1, obb.size.z};      // 右下

	/*ステップ2 球の生成*/
	Vector3 SpherePoint = sphere.center;

	Matrix4x4 OBBInverceMat = mat->Inverce(worldMatrix);

	SpherePoint = vec->Transform(SpherePoint, OBBInverceMat);

	/*ステップ4 当たり判定*/
	// near面から
	Vector3 v01 = obbPoints[1] - obbPoints[0];
	Vector3 v12 = obbPoints[2] - obbPoints[1];

	// far
	Vector3 v65 = obbPoints[5] - obbPoints[6];
	Vector3 v54 = obbPoints[4] - obbPoints[5];

	// left
	Vector3 v02 = obbPoints[2] - obbPoints[0];
	Vector3 v26 = obbPoints[6] - obbPoints[2];

	// right
	Vector3 v53 = obbPoints[3] - obbPoints[5];
	Vector3 v31 = obbPoints[1] - obbPoints[3];

	// up
	Vector3 v41 = obbPoints[1] - obbPoints[4];
	Vector3 v10 = obbPoints[0] - obbPoints[1];

	// down
	Vector3 v23 = obbPoints[3] - obbPoints[2];
	Vector3 v36 = obbPoints[6] - obbPoints[3];

	Vector3 normal[6] = {};

	float distance[6] = {};
	// near
	normal[0] = vec->Normalize(vec->Cross(v01, v12));
	// far
	normal[1] = vec->Normalize(vec->Cross(v65, v54));
	// left
	normal[2] = vec->Normalize(vec->Cross(v02, v26));
	// right
	normal[3] = vec->Normalize(vec->Cross(v53, v31));
	// up
	normal[4] = vec->Normalize(vec->Cross(v41, v10));
	// down
	normal[5] = vec->Normalize(vec->Cross(v23, v36));

	distance[0] = vec->Dot(SpherePoint - obbPoints[0], normal[0]);
	distance[1] = vec->Dot(SpherePoint - obbPoints[4], normal[1]);
	distance[2] = vec->Dot(SpherePoint - obbPoints[0], normal[2]);
	distance[3] = vec->Dot(SpherePoint - obbPoints[1], normal[3]);
	distance[4] = vec->Dot(SpherePoint - obbPoints[0], normal[4]);
	distance[5] = vec->Dot(SpherePoint - obbPoints[2], normal[5]);


	if (distance[0] <= sphere.radius && distance[1] <= sphere.radius && distance[2] <= sphere.radius &&
	    distance[3] <= sphere.radius && distance[4] <= sphere.radius && distance[5] <= sphere.radius) {
		return true;
	}

	return false;
}


bool GameScene::IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2) {
	static MyVector vector;
	static MyMatrix matrix;

	Vector3 separatingAxis[15];
	separatingAxis[0] = obb1.orientations[0];
	separatingAxis[1] = obb1.orientations[1];
	separatingAxis[2] = obb1.orientations[2];
	separatingAxis[3] = obb2.orientations[0];
	separatingAxis[4] = obb2.orientations[1];
	separatingAxis[5] = obb2.orientations[2];
	int axisNum = 6;
	for (int index1 = 0; index1 < 3; index1++) {
		for (int index2 = 0; index2 < 3; index2++) {
			separatingAxis[axisNum] = vector.Cross(obb1.orientations[index1], obb2.orientations[index2]);
			axisNum++;
		}
	}

	Vector3 obb1Vetyex[8];
	// bottom
	obb1Vetyex[0] = Vector3{-obb1.size.x, -obb1.size.y, -obb1.size.z};
	obb1Vetyex[1] = Vector3{+obb1.size.x, -obb1.size.y, -obb1.size.z};
	obb1Vetyex[2] = Vector3{-obb1.size.x, -obb1.size.y, +obb1.size.z};
	obb1Vetyex[3] = Vector3{+obb1.size.x, -obb1.size.y, +obb1.size.z};
	// top
	obb1Vetyex[4] = Vector3{-obb1.size.x, +obb1.size.y, -obb1.size.z};
	obb1Vetyex[5] = Vector3{+obb1.size.x, +obb1.size.y, -obb1.size.z};
	obb1Vetyex[6] = Vector3{-obb1.size.x, +obb1.size.y, +obb1.size.z};
	obb1Vetyex[7] = Vector3{+obb1.size.x, +obb1.size.y, +obb1.size.z};

	Matrix4x4 rotateMatrix1 = GetRotate(obb1);

	Vector3 obb2Vetyex[8];
	// bottom
	obb2Vetyex[0] = Vector3{-obb2.size.x, -obb2.size.y, -obb2.size.z};
	obb2Vetyex[1] = Vector3{+obb2.size.x, -obb2.size.y, -obb2.size.z};
	obb2Vetyex[2] = Vector3{-obb2.size.x, -obb2.size.y, +obb2.size.z};
	obb2Vetyex[3] = Vector3{+obb2.size.x, -obb2.size.y, +obb2.size.z};
	// top
	obb2Vetyex[4] = Vector3{-obb2.size.x, +obb2.size.y, -obb2.size.z};
	obb2Vetyex[5] = Vector3{+obb2.size.x, +obb2.size.y, -obb2.size.z};
	obb2Vetyex[6] = Vector3{-obb2.size.x, +obb2.size.y, +obb2.size.z};
	obb2Vetyex[7] = Vector3{+obb2.size.x, +obb2.size.y, +obb2.size.z};

	Matrix4x4 rotateMatrix2 = GetRotate(obb2);

	for (int index = 0; index < 8; index++) {
		obb1Vetyex[index] = vector.Transform( obb1Vetyex[index] , rotateMatrix1);
		obb1Vetyex[index] = (obb1Vetyex[index]+ obb1.center);
		obb2Vetyex[index] = vector.Transform( obb2Vetyex[index] , rotateMatrix2);
		obb2Vetyex[index] = (obb2Vetyex[index]+ obb2.center);
	}

	for (axisNum = 0; axisNum < 15; axisNum++) {
		float projectionPoint1[8];
		float projectionPoint2[8];
		float min1, max1;
		float min2, max2;
		min1 = vector.Dot(obb1Vetyex[0], vector.Normalize(separatingAxis[axisNum]));
		min2 = vector.Dot(obb2Vetyex[0], vector.Normalize(separatingAxis[axisNum]));
		max1 = min1;
		max2 = min2;
		for (int index = 0; index < 8; index++) {
			projectionPoint1[index] =
			    vector.Dot(obb1Vetyex[index], vector.Normalize(separatingAxis[axisNum]));
			projectionPoint2[index] =
			    vector.Dot(obb2Vetyex[index], vector.Normalize(separatingAxis[axisNum]));
			if (index == 0) {
				min1 = projectionPoint1[index];
				min2 = projectionPoint2[index];
				max1 = min1;
				max2 = min2;
			} else {
				min1 = min(min1, projectionPoint1[index]);
				min2 = min(min2, projectionPoint2[index]);
				max1 = max(max1, projectionPoint1[index]);
				max2 = max(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = max(max1, max2) - min(min1, min2);
		if (sumSpan < longSpan) {
			return false;
		}
	}
	return true;
}

void GameScene::TitleInitialize() { 
	//TitleBGMHandle_ = audio_->PlayWave(TitleBGMDataHandle_, true);
	selectMode = 0;

}

void GameScene::TitleUpdate() {
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
			sceneRequest_ = Scene::Control;
		}
	}
	
}

void GameScene::ControlInitialize() {
}

void GameScene::ControlUpdate() {

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
			//audio_->StopWave(TitleBGMHandle_);

			sceneRequest_ = Scene::Main;
		}
	}
}



void GameScene::MainInitialize() { 
	Initialize(); 
	//MainBGMHandle_ = audio_->PlayWave(MainBGMDataHandle_, true);
	//audio_->SetVolume(MainBGMHandle_, 0.5f);
}

void GameScene::MainUpdate() { 
	player_->Update();

	enemy_->Update();

	skyDome_->Update();

	ground_->Update();

	wall_->Update();

	followCamera_->Update();

	enemyCamera_->Update();

#ifdef _DEBUG
	/*if (Input::GetInstance()->GetJoystickState(0, joyState)) {
	    if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
	        isDebugCameraActive_ = !isDebugCameraActive_;
	    }
	}*/

	if (input_->TriggerKey(DIK_RETURN)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	ImGui::Begin("SELECTMODE");
	ImGui::Text("select = %d", selectMode);
	ImGui::End();
	/*ImGui::Begin("CameraInforMation");
	ImGui::DragFloat3("CameraRotate", &followCamera_->GetViewProjection().rotation_.x, 0.1f);
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();*/

#endif // _DEBUG

	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		viewProjection_.matView = enemyCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = enemyCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}

	CheckAllCollisions();
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
			sceneRequest_ = Scene::Pose;
		}
	}

	if (enemy_->GetEnemyLife()<=0.0f) {
		//audio_->StopWave(MainBGMHandle_);
		sceneRequest_ = Scene::End;
	}

	if (player_->GetPlayerLife() <= 0.0f) {
		sceneRequest_ = Scene::GameOver;
	}

}

void GameScene::PoseInitialize() {

}

void GameScene::PoseUpdate() {
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
				sceneRequest_ = Scene::Main;
		}
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
			if (selectMode == 0) {
				sceneRequest_ = Scene::Main;
			} else {
				//audio_->StopWave(MainBGMHandle_);
				sceneRequest_ = Scene::Title;
			}
		}
	}
}

void GameScene::EndInitialize() {
	// EndBGMHandle_ = audio_->PlayWave(EndBGMDataHandle_);
}

void GameScene::EndUpdate() {
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
			//audio_->StopWave(EndBGMHandle_);
			sceneRequest_ = Scene::Title;
		}
	}
	
}

void GameScene::GameOverInitialize() {  }

void GameScene::GameOverUpdate() {
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) &&
		    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
			audio_->StopWave(EndBGMHandle_);
			if (selectMode==0) {
				//audio_->StopWave(MainBGMHandle_);
				sceneRequest_ = Scene::Main;
			} else {
				//audio_->StopWave(MainBGMHandle_);
				sceneRequest_ = Scene::Title;
			}
		}
	}
}
