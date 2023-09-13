#include "Player.h"
#include<assert.h>
#include<imgui.h>
#include <WinApp.h>

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	
	delete sprite2DReticle_;
}

void Player::Initialize(const std::vector<Model*>& models) {
	InitializeFloatingGimmick();
	initializeMoveArm();
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";
	const char* groupName2 = "PlayerPrats";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	adjustment_item->CreateGroup(groupName2);

	adjustment_item->AddItem(groupName, "CharacterSpeed", kCharacterSpeedBase);
	adjustment_item->AddItem(groupName, "floatingCycle", floatingCycle_);
	adjustment_item->AddItem(groupName, "floatingAmplitude", floatingAmplitude);
	adjustment_item->AddItem(groupName, "armAmplitude", armAmplitude);
	adjustment_item->AddItem(groupName, "DashSpeed", DashSpeed);


	adjustment_item->AddItem(groupName2, "Tail_offset", Tail_offset_Base);
	adjustment_item->AddItem(groupName2, "ArmL_offset", L_arm_offset_Base);


	BaseCharacter::Initialize(models);
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransformBody_.Initialize();
	worldTransformBody_.translation_ = {0.0f, 4.0f, 0.0f};

	worldTransformTail_.Initialize();
	worldTransformTail_.parent_ = &worldTransformBody_;

	worldTransformL_arm_.Initialize();

	uint32_t textureReticle = TextureManager::Load("Magic.png");

	sprite2DReticle_ = Sprite::Create(textureReticle, {640.0f, 320.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});

	worldTransform3DReticle_.Initialize();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	dashCoolTime = kDashCoolTime;

	PlayerLife = kPlayerLifeMax;

	//基本挙動初期化
	BehaviorRootInitialize();
	
	//hpが0以下になったときに一度だけ呼び出す
	collDeath_ = true;

	textureHandleEgg_ = TextureManager::Load("egg.png");
	uint32_t textureback = TextureManager::Load("blackEgg.png");
	uint32_t textureLB = TextureManager::Load("LB.png");
	spriteEnergy_ = Sprite::Create(textureHandleEgg_, energyPosition, {1, 1, 1, 1}, {0.5f, 1.0f});
	spriteEnergiBack_ = Sprite::Create(textureback, energyPosition, {1, 1, 1, 1}, {0.5f, 1.0f});
	spriteEnergyButton_ = Sprite::Create(textureLB, energyPosition, {1, 1, 1, 1}, {0.5f, 0.5f});

	//ダッシュエフェクト
	textureConcentrationLine = TextureManager::Load("concentrationLine.png");
	concentrationLine_ = std::make_unique<ConcentrationLine>();
	concentrationLine_->Initialize(models_[5], textureConcentrationLine);
	concentrationLine_->SetPlayerVelocity(&move);
	concentrationLine_->SetPlayerWorldTransform(&worldTransform_);


	textureLiner_ = TextureManager::Load("straight.png");
	textureChaser_ = TextureManager::Load("tracking.png");
	textureArrow_ = TextureManager::Load("change.png");
	textureX_ = TextureManager::Load("text/X.png");

	changeUIBaseScale_={128.0f, 128.0f};
	changeUIBaseRotation_ = 0.0f;
	changeUIBaseTranslation_={120, 480};
	liner_={0, 0};
	chaser_={0, 0};
	xButtom_={0, 0};

	spriteLiner_ = Sprite::Create(textureLiner_, changeUIBaseTranslation_ + liner_, {1, 1, 1, 1}, {0.5f, 0.5f});
	spriteChaser_ = Sprite::Create(
	    textureChaser_, changeUIBaseTranslation_ + chaser_, {1, 1, 1, 1}, {0.5f, 0.5f});
	spriteArrow_ = Sprite::Create(
	    textureArrow_, changeUIBaseTranslation_, {1, 1, 1, 1}, {0.5f, 0.5f});
	spriteX_ = Sprite::Create(
	    textureX_, changeUIBaseTranslation_, {0.19f, 0.82f, 0.89f, 1}, {0.5f, 0.5f});
}

void Player::Update() {
	ApplyGlobalVariables();
	bulletCreateCoolTime--;
	homingCoolTime--;
	if (bulletCreateCoolTime<0)
	{
		bulletCreateCoolTime = 0;
	}
	// デスフラグの立った球を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
#ifdef _DEBUG
	
	DrawImgui();
#endif
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Behavior::kShot:
			BehaviorShotInitialize();
			break;
		case Behavior::kDead:
			BehaviorDeathInitialize();
			break;
		}	
		
	}	
		// 振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;	
	
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Behavior::kShot:
		BehaviorShotUpdate();
		break;
	case Behavior::kDead:
		BehaviorDeathUpdate();
		break;
	}

	FinAnimationUpdate();

	Matrix4x4 PlayerRotateMatrix = matrix.MakeRotateMatrix(worldTransformBody_.rotation_);

	for (int i = 0; i < 3; i++) {
		obb.orientations[i].x = PlayerRotateMatrix.m[i][0];
		obb.orientations[i].y = PlayerRotateMatrix.m[i][1];
		obb.orientations[i].z = PlayerRotateMatrix.m[i][2];
	}

	obb.center = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y + 4.5f,
	    worldTransform_.translation_.z};

	Head_offset = vector.TransformNormal(Tail_offset_Base, PlayerRotateMatrix);
	L_arm_offset = vector.TransformNormal(L_arm_offset_Base, PlayerRotateMatrix);
	// 座標をコピーしてオフセット分ずらす
	worldTransformTail_.translation_ = Tail_offset_Base;
	worldTransformL_arm_.translation_ = worldTransformBody_.translation_ + L_arm_offset;
	

	// 座標を転送
	worldTransformBody_.UpdateMatrix(bodyScale);
	worldTransformTail_.UpdateMatrix(tailScale);
	worldTransformL_arm_.UpdateMatrix(leftArmScale);

	BulletNum = CheckBullet();
	BulletMax = CheckBulletAll();
	//行列更新
	worldTransform_.UpdateMatrix(worldTransform_.scale_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}	

	//死亡アニメーション再生(まだデスフラグは立てない)
	if (GetPlayerLife() <= 0.0f && collDeath_) {
		behaviorRequest_ = Behavior::kDead;
		collDeath_ = false;
	}

	// 無敵時間の処理
	if (--invincibleTime_ < 0) {
		isInvincible_ = false;
	}

	//弾を増やす
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER && 
		behavior_ != Behavior::kDead) {
		if (bulletCreateCoolTime<=0 && kBulletMax > GetBulletNumMax())
		{
			bulletCreateCoolTime = kBulletCreateCoolTime;
			AddBullet(kCreateBulletNum);
		}
	}

	// 発射モード切り替え
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) &&
	    !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_X)) {
		HomingMode_ = !HomingMode_;
		changeAnimation_ = 0.0f;
	}

	EnergyUpdate();
	ChangeUIUpdate();

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		preJoyState = joyState;
	}

	//ダッシュエフェクト
	if (behavior_ == Behavior::kDash) {
		concentrationLine_->SetIsPop(true);
		concentrationLine_->SetIsDrawSprite(true);
	} else {
		concentrationLine_->SetIsPop(false);
		concentrationLine_->SetIsDrawSprite(false);
	}

	concentrationLine_->Update();
}

void Player::Draw(const ViewProjection& viewProjection) {
	if (!isInvincible_ || invincibleTime_%2 == 0)
	{
		models_[0]->Draw(worldTransformBody_, viewProjection);
		models_[1]->Draw(worldTransformTail_, viewProjection);
	}
	//models_[0]->Draw(worldTransformBody_, viewProjection);
	//models_[1]->Draw(worldTransformTail_, viewProjection);
	//models_[2]->Draw(worldTransformL_arm_, viewProjection);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
	
	concentrationLine_->Draw(viewProjection);
}

void Player::EnergyUpdate()
{
	ratio = float(kBulletCreateCoolTime - bulletCreateCoolTime) / float(kBulletCreateCoolTime);
	energyColor_ = kGray;
	if (ratio == 1.0f) {
		energyColor_ = kWhite;
	}
	spriteEnergy_->SetColor(energyColor_);
	spriteEnergy_->SetPosition(energyPosition);
	spriteEnergy_->SetAnchorPoint(ancor);
	spriteEnergy_->SetSize({eggSize.x * 2.0f, eggSize.y * ratio * 2.0f});
	spriteEnergiBack_->SetSize({eggSize.x * 2.0f, eggSize.y * 2.0f});
	spriteEnergy_->SetTextureRect(
	    Vector2{0.0f, 64.0f * (1.0f - ratio)}, {eggSize.x, eggSize.y * ratio});

	spriteEnergyButton_->SetSize(buttonSize);
	spriteEnergyButton_->SetPosition(buttonPosition);
	spriteEnergyButton_->SetColor(buttonColor_);
}

void Player::DrawUI() { 
	concentrationLine_->Draw();
	if (joyState.Gamepad.bLeftTrigger != 0) {
		sprite2DReticle_->Draw();
	}
	
	spriteEnergiBack_->Draw();
	spriteEnergy_->Draw();
	if (ratio==1.0f)
	{
		spriteEnergyButton_->Draw();
	}

	if (HomingMode_)
	{
		spriteLiner_->Draw();
		spriteChaser_->Draw();
	}
	else
	{
		spriteChaser_->Draw();
		spriteLiner_->Draw();
	}
	spriteArrow_->Draw();
	spriteX_->Draw();
}

void Player::BehaviorRootInitialize() { 
	move = {0.0f,0.0f,0.0f};
	worldTransformL_arm_.rotation_ = {0};
	kCharacterSpeed = kCharacterSpeedBase;
}

void Player::BehaviorRootUpdate() {
	
	//構え状態の弾を解除
	for (PlayerBullet* bullet : bullets_) {
		if (bullet->GetState() == PlayerBullet::PlayerBulletState::Stance) {
			bullet->StanceCancel();
			break;
		}
	}

	//kCharacterSpeed = kCharacterSpeedBase;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float threshold = 0.7f;
		bool isMoveing = false;
		float moveLength = 0.0f;


		// 移動量
		Vector3 move_ = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX,
		    0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX,
		};
		moveLength = vector.Length(move_);
		
		if (moveLength > threshold){
			isMoveing = true;
		} 
		if (isMoveing) {
			move = {
			    (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed,
			    0.0f,
			    (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed,
			};			
		} 
		else {
			if (input_->TriggerKey(DIK_W)) {
				move.z += kCharacterSpeed;
			} else if (input_->TriggerKey(DIK_S)) {
				move.z -= kCharacterSpeed;
			} else if (input_->TriggerKey(DIK_D)) {
				move.x += kCharacterSpeed;
			} else if (input_->TriggerKey(DIK_A)) {
				move.x -= kCharacterSpeed;
			} else {
				move = {0.0f};
			}
		}
	}
	/*if (input_->PushKey(DIK_W)) {
		move.z = kCharacterSpeed;
	} else if (input_->PushKey(DIK_S)) {
		move.z = -kCharacterSpeed;
	} 
	else {
		move.z = 0;
	}
	if (input_->PushKey(DIK_D)) {
		move.x = kCharacterSpeed;
	}  else if (input_->PushKey(DIK_A)) {
		move.x = -kCharacterSpeed;
	} else {
		move.x = 0;
	} */
	
	


	Matrix4x4 newRotateMatrix = matrix.MakeRotateMatrixY(viewProjection_->rotation_);

	move = vector.TransformNormal(move, newRotateMatrix);

	// プレイヤーの移動方向に見た目を合わせる
	if (move.x != 0.0f || move.z != 0.0f) {
		target_angle = std::atan2(move.x, move.z);
		//worldTransformBody_.rotation_.y = std::atan2(move.x, move.z);	
	}
	
	worldTransformBody_.rotation_.y =
	    vector.LerpShortAngle(worldTransformBody_.rotation_.y, target_angle, 0.1f);
	worldTransformTail_.rotation_.y = worldTransformBody_.rotation_.y;

	worldTransformL_arm_.rotation_.y = worldTransformBody_.rotation_.y;
	//UpdateFloatingGimmick();

	worldTransform_.rotation_.y = worldTransformBody_.rotation_.y;

	dashCoolTime -= 1;

	// 座標を加算

	if ((worldTransform_.translation_ + move).x > MoveMax) {
		move.x = {0};
		worldTransform_.translation_.x = MoveMax;
		worldTransformBody_.translation_.x = MoveMax;
	} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
		move.x = {0};
		worldTransform_.translation_.x = -MoveMax;
		worldTransformBody_.translation_.x = -MoveMax;
	}

	if ((worldTransform_.translation_ + move).z > MoveMax) {
		move.z = {0};
		worldTransform_.translation_.z = MoveMax;
		worldTransformBody_.translation_.z = MoveMax;
	} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
		move.z = {0};
		worldTransform_.translation_.z = -MoveMax;
		worldTransformBody_.translation_.z = -MoveMax;
	}

	worldTransform_.AddTransform(move);
	worldTransformBody_.AddTransform(move);
	if (((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
	     input_->TriggerKey(DIK_SPACE))&&dashCoolTime<=0) {
		behaviorRequest_ = Behavior::kDash;
	} 
	/*if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B){
		behaviorRequest_ = Behavior::kAttack;
	}*/
	if (joyState.Gamepad.bLeftTrigger != 0) {
		behaviorRequest_ = Behavior::kShot;
	}

}

void Player::InitializeFloatingGimmick() { 
	floatingParameter_ = 0.0f;
	floatingCycle_ = 90;
	floatingAmplitude = 0.7f;
	disGround = 0.3f;
}

void Player::UpdateFloatingGimmick() {
	
	//1フレームでのパラメータ加算値
	const float step = 2.0f * static_cast<float>(M_PI) / floatingCycle_;

	//パラメータを1ステップ分加算
	floatingParameter_ += step;

	//2πを超えたら0に戻す
	floatingParameter_ =
	    static_cast<float>(std::fmod(floatingParameter_, 2.0 * static_cast<float>(M_PI)));	

	//浮遊を座標に反映
	worldTransformBody_.translation_.y =
	    std::sin(floatingParameter_) * floatingAmplitude + disGround;
	
}

void Player::FinAnimationUpdate() {
		finRotate += 0.05f;
		worldTransformTail_.rotation_.y = (std::sin(finRotate)) * float(M_PI) / 4.0f;
}

void Player::initializeMoveArm() {
	armParameter_ = 0.0f;
	armPeriod = 75;
	armAmplitude = 0.5f;

}

void Player::UpdateMoveArm() {
	// 1フレームでのパラメータ加算値
	const float step = 2.0f * static_cast<float>(M_PI) / armPeriod;

	// パラメータを1ステップ分加算
	armParameter_ += step;

	// 2πを超えたら0に戻す
	armParameter_ =
	    static_cast<float>(std::fmod(armParameter_, 2.0 * static_cast<float>(M_PI)));	

	worldTransformL_arm_.rotation_.x = std::sin(armParameter_) * armAmplitude;
	

}

void Player::ApplyGlobalVariables() {
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";
	const char* groupName2 = "PlayerPrats";

	Tail_offset_Base = adjustment_item->GetVector3Value(groupName2, "Head_offset");
	L_arm_offset_Base = adjustment_item->GetVector3Value(groupName2, "ArmL_offset");

	floatingCycle_ = adjustment_item->GetIntValue(groupName, "floatingCycle");
	floatingAmplitude = adjustment_item->GetfloatValue(groupName, "floatingAmplitude");
	armAmplitude = adjustment_item->GetfloatValue(groupName, "armAmplitude");
	kCharacterSpeedBase = adjustment_item->GetfloatValue(groupName, "CharacterSpeed");
	DashSpeed = adjustment_item->GetfloatValue(groupName, "DashSpeed");
	
}

void Player::BehaviorDashInitialize() { 
	workDash_.dashParameter_ = 0;
	worldTransformBody_.rotation_.y = target_angle;
	concentrationLine_->SetIsPop(true);
}

void Player::BehaviorDashUpdate() { 

	kCharacterSpeed = DashSpeed;
	/*
	Matrix4x4 newRotateMatrix_ = matrix.MakeRotateMatrixY(worldTransformBody_.rotation_);
	move = {0, 0, kCharacterSpeed * kDashSpeed};

	move = vector.TransformNormal(move, newRotateMatrix_);

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 5;
	if ((worldTransform_.translation_ + move).x > MoveMax) {
		move.x = {0};
		worldTransform_.translation_.x = MoveMax;
		worldTransformBody_.translation_.x = MoveMax;
	} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
		move.x = {0};
		worldTransform_.translation_.x = -MoveMax;
		worldTransformBody_.translation_.x = -MoveMax;
	}

	if ((worldTransform_.translation_ + move).z > MoveMax) {
		move.z = {0};
		worldTransform_.translation_.z = MoveMax;
		worldTransformBody_.translation_.z = MoveMax;
	} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
		move.z = {0};
		worldTransform_.translation_.z = -MoveMax;
		worldTransformBody_.translation_.z = -MoveMax;
	}

	worldTransform_.AddTransform(move);
	worldTransformBody_.AddTransform(move);
	worldTransformTail_.rotation_.y = worldTransformBody_.rotation_.y;

	worldTransformL_arm_.rotation_.y = worldTransformBody_.rotation_.y;

	//既定の時間経過で通常状態に戻る
	if (++workDash_.dashParameter_>=behaviorDashTime) {
		dashCoolTime = kDashCoolTime;
		behaviorRequest_ = Behavior::kRoot;
	}
	*/
	BehaviorRootUpdate();
	if ((!(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
	     input_->TriggerKey(DIK_SPACE))) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorShotInitialize() { 
	kCharacterSpeed = 0.2f;
	for (PlayerBullet* bullet : bullets_) {
		bullet->MoveOn(); 
	}
}

void Player::BehaviorShotUpdate() {

	ShotReticle(viewProjection_->matView, viewProjection_->matProjection);
	ReticleUpdate();
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float threshold = 0.7f;
		bool isMoveing = false;
		float moveLength = 0.0f;
		if (joyState.Gamepad.bLeftTrigger == 0) {
			behaviorRequest_ = Behavior::kRoot;
		}

		// 移動量
		Vector3 move_ = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX,
		    0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX,
		};
		moveLength = vector.Length(move_);
		#ifdef _DEBUG
		ImGui::Begin("Flag");
		ImGui::DragFloat("move", &moveLength, 0.01f);
		ImGui::End();
		#endif

		if (moveLength > threshold) {
			isMoveing = true;
		}
		if (isMoveing) {
			move = {
			    (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed,
			    0.0f,
			    (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed,
			};
		} else {
			if (input_->TriggerKey(DIK_W)) {
				move.z += kCharacterSpeed;
			} else if (input_->TriggerKey(DIK_S)) {
				move.z -= kCharacterSpeed;
			} else if (input_->TriggerKey(DIK_D)) {
				move.x += kCharacterSpeed;
			} else if (input_->TriggerKey(DIK_A)) {
				move.x -= kCharacterSpeed;
			} else {
				move = {0.0f};
			}
		}
	}
	/*if (input_->PushKey(DIK_W)) {
	    move.z = kCharacterSpeed;
	} else if (input_->PushKey(DIK_S)) {
	    move.z = -kCharacterSpeed;
	}
	else {
	    move.z = 0;
	}
	if (input_->PushKey(DIK_D)) {
	    move.x = kCharacterSpeed;
	}  else if (input_->PushKey(DIK_A)) {
	    move.x = -kCharacterSpeed;
	} else {
	    move.x = 0;
	} */

	Matrix4x4 newRotateMatrix = matrix.MakeRotateMatrixY(viewProjection_->rotation_);

	move = vector.TransformNormal(move, newRotateMatrix);

	target_angle = viewProjection_->rotation_.y;

	if (worldTransformBody_.rotation_.y != target_angle) {
		worldTransformBody_.rotation_.y =
		    vector.LerpShortAngle(worldTransformBody_.rotation_.y, target_angle, 0.1f);
	}
	else{
		worldTransformBody_.rotation_.y = target_angle;
	}

	
	worldTransformTail_.rotation_.y = worldTransformBody_.rotation_.y;

	worldTransformL_arm_.rotation_.x = 1.57f + (viewProjection_->rotation_.x * 1.1f);
	worldTransformL_arm_.rotation_.y =worldTransformBody_.rotation_.y;
	

	floatingAmplitude = 0.1f;
	//UpdateFloatingGimmick();

	worldTransform_.rotation_.y = worldTransformBody_.rotation_.y;

	L_arm_offset_Base.z = 0.5f;

	isShotBullet_ = false;
	if (!HomingMode_) {
		Attack();
	} else {
		AttackHoming();
	}
	if ((worldTransform_.translation_ + move).x > MoveMax) {
		move.x = {0};
		worldTransform_.translation_.x = MoveMax;
		worldTransformBody_.translation_.x = MoveMax;
	} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
		move.x = {0};
		worldTransform_.translation_.x = -MoveMax;
		worldTransformBody_.translation_.x = -MoveMax;
	}

	if ((worldTransform_.translation_ + move).z > MoveMax) {
		move.z = {0};
		worldTransform_.translation_.z = MoveMax;
		worldTransformBody_.translation_.z = MoveMax;
	} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
		move.z = {0};
		worldTransform_.translation_.z = -MoveMax;
		worldTransformBody_.translation_.z = -MoveMax;
	}

	if (!HomingMode_) {
		// 待機状態になっている奴の先頭を手元に持ってくる
		for (PlayerBullet* bullet : bullets_) {
			if (bullet->GetState() == PlayerBullet::PlayerBulletState::Idle ||
			    bullet->GetState() == PlayerBullet::PlayerBulletState::Stance) {
				bullet->SetShotIdle(L_arm_offset_Base);
				break;
			}
		}
	}
	// 座標を加算
	worldTransform_.AddTransform(move);
	worldTransformBody_.AddTransform(move);
}

void Player::BehaviorDeathInitialize()
{
	t = 0.0f;
	rotate_ = worldTransformBody_.rotation_;
}

void Player::BehaviorDeathUpdate(){
	//static MyVector myvector;
	//color_ = textureHandleRed_;
	rotate_ = Vector3{0.0f, 0.0f, float(M_PI) / 2.0f};
	t += 0.02f;
	worldTransformBody_.rotation_.z = vector.Multiply(min(t, 1.0f), rotate_).z;
	worldTransformBody_.translation_.y -= 0.2f;
	if (t >= 1.0f) {
		scale = vector.Multiply((1.0f - (t - 1.0f)), scale);
	}
	if (t >= 2.0f) {
		isDead_ = true;
	}
}

void Player::OnCollision() { 
	if (!isInvincible_)
	{
		isInvincible_ = true;
		invincibleTime_ = kHitInvincible;
		for (PlayerBullet* bullet : bullets_) {
			if (bullet->GetState() == PlayerBullet::PlayerBulletState::Idle) {
				bullet->OnCollision();
				return;
			}
		}
		PlayerLife -= 50;
	}
}
void Player::OnCollision(int Damage) {
	if (!isInvincible_) {
		int num = Damage;
		for (PlayerBullet* bullet : bullets_) {
			if (bullet->GetState() == PlayerBullet::PlayerBulletState::Idle) {
				bullet->OnCollision();
				num--;
				if (num<=0)
				{
					isInvincible_ = true;
					invincibleTime_ = kHitInvincible;
					return;
				}
			}
		}
		if (num != Damage)
		{
			isInvincible_ = true;
			invincibleTime_ = kHitInvincible;
			return;
		}
		PlayerLife -= 50;
	}
}	

void Player::DrawImgui() {
	ImGui::Begin("Player");
	ImGui::DragFloat3("Body Translation", &worldTransformBody_.translation_.x);
	ImGui::SliderFloat3("ArmL Translation", &worldTransformL_arm_.translation_.x, 3.0f, 10.0f);
	ImGui::SliderInt("floatingCycle_", &floatingCycle_, 10, 180);
	ImGui::SliderFloat("Amplitude", &floatingAmplitude, 0.1f, 1.0f);
	ImGui::SliderFloat("DisGround", &disGround, 0.1f, 1.0f);
	ImGui::DragInt("bulletNum", &BulletNum);
	ImGui::DragInt("checkCamera", &checkCamera);
	ImGui::End();

	ImGui::Begin("BulletCreateCoolTime");
	ImGui::Text("CoolTime %d",bulletCreateCoolTime/60);
	ImGui::DragInt("CoolTimeMax", &kBulletCreateCoolTime);
	ImGui::End();

	ImGui::Begin("PlayerRotate");
	ImGui::SliderFloat3("ArmL Rotate", &worldTransformL_arm_.rotation_.x, -3.0f, 3.0f);
	ImGui::End();

	ImGui::Begin("FPS");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("UIs");
	ImGui::DragFloat2("position",&energyPosition.x,1.0f);
	ImGui::DragFloat2("Ancor", &ancor.x, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat("ratio", &ratio, 0.1f, 0.0f, 1.0f);
	ImGui::DragFloat2("buttonposition", &buttonPosition.x, 1.0f);
	ImGui::DragFloat2("buttonSize", &buttonSize.x, 1.0f);
	ImGui::End();

	
	ImGui::Begin("change");
	ImGui::DragFloat2("scale", &changeUIBaseScale_.x, 1.0f);
	ImGui::DragFloat("rotate", &changeUIBaseRotation_, 1.0f);
	ImGui::DragFloat2("position", &changeUIBaseTranslation_.x, 1.0f);
	ImGui::DragFloat2("linerposition", &liner_.x, 1.0f);
	ImGui::DragFloat2("chaserposition", &chaser_.x, 1.0f);
	ImGui::DragFloat2("valid", &valid_.x, 1.0f);
	ImGui::DragFloat2("notValid", &notValid_.x, 1.0f);

	ImGui::End();

}

void Player::ShotReticle(const Matrix4x4& matView, const Matrix4x4& matProjection) {
	// 3Dから2Dへのレティクルの変換↓

	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 100.0f;
	// 自機から3Dレティクルへのオフセット
	Vector3 offset = {0.0f, 0.0f, 1.0f};
	// 自機のワールド行列の回転を反映

	Matrix4x4 newRotateMatrix = matrix.MakeRotateMatrix(viewProjection_->rotation_);

	offset = vector.TransformNormal(offset, newRotateMatrix);
	// ベクトルの長さを整える
	offset = vector.NormalizePlus(offset, kDistancePlayerTo3DReticle);
	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ =
	    GetWorldPosition(worldTransformL_arm_.matWorld_) + offset;
	worldTransform3DReticle_.UpdateMatrix(reticleScale_);

	
	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	if(HomingMode_){
		Vector3 positionEnemy = *enemyCenter_;
		/* Matrix4x4 mat = matrix.MakeAffineMatrix(
		    worldTransformEnemy_->scale_, worldTransformEnemy_->rotation_,
		    worldTransformEnemy_->translation_);
		Vector3 positionEnemy = {mat.m[3][0], mat.m[3][1], mat.m[3][2]};*/
		//Vector3 positionEnemy = {0,0,0};
		// ビューポート行列
		Matrix4x4 matViewport =
		    matrix.MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport =
		    matrix.Multiply(matrix.Multiply(matView, matProjection), matViewport);

		// ワールド->スクリーン座標変換(ここで3Dから2Dになる)
		positionEnemy = vector.Transform(positionEnemy, matViewProjectionViewport);

		// スプライトのレティクルに座標設定
		//sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
		reticleEnemy_ = {positionEnemy.x,positionEnemy.y};
	}
	else
	{
		Vector3 positionReticle = GetWorldPosition(worldTransform3DReticle_.matWorld_);
		// ビューポート行列
		Matrix4x4 matViewport =
		    matrix.MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport =
		    matrix.Multiply(matrix.Multiply(matView, matProjection), matViewport);

		// ワールド->スクリーン座標変換(ここで3Dから2Dになる)
		positionReticle = vector.Transform(positionReticle, matViewProjectionViewport);

		// スプライトのレティクルに座標設定
		reticleCenter_ = (Vector2(positionReticle.x, positionReticle.y));
		
	}
	
#ifdef _DEBUG

	ImGui::Begin("Shot");
	ImGui::DragFloat3("ArmL Rotate", &offset.x);
	ImGui::End();
#endif
	
	// ここまで↑
}

void Player::ReticleUpdate() {
	reticleTarget_ = reticleCenter_;
	if (HomingMode_ && GetcheckCamera()) {
		reticleTarget_ = reticleEnemy_;
	}
	Vector2 position =  sprite2DReticle_->GetPosition();
	position = vector.Lerp(position,reticleTarget_,0.2f);
	sprite2DReticle_->SetPosition(position);
	//sprite2DReticle_->SetPosition(reticleEnemy_);
}

void Player::Attack() {
	if (joyState.Gamepad.bRightTrigger != 0/* || (joyState.Gamepad.wButtons&&XINPUT_GAMEPAD_B)*/) {
		bulletTime += 1;
		if (bulletTime % bulletInterval == 1) {

			// 弾の速度
			const float kBulletSpeed = 5.0f;
			Vector3 world3DReticlePos = GetWorldPosition(worldTransform3DReticle_.matWorld_);

			Vector3 velocity = world3DReticlePos - GetWorldPosition(worldTransformL_arm_.matWorld_);
			velocity = vector.NormalizePlus(velocity, kBulletSpeed);
			
			for (PlayerBullet* bullet : bullets_)
			{
				if (bullet->GetState() == PlayerBullet::PlayerBulletState::Stance)
				{
					isShotBullet_ = true;
					bullet->SetShot(
					    viewProjection_->rotation_, velocity, (HomingMode_ && GetcheckCamera()));
					bullet->SetState(PlayerBullet::PlayerBulletState::Move);
					break;
				}
				//assert(false);
			}
		}
	} else {
		bulletTime = 0;
	}
}

void Player::AttackHoming() {
	if (joyState.Gamepad.bRightTrigger != 0 /* || (joyState.Gamepad.wButtons&&XINPUT_GAMEPAD_B)*/) {
		if (homingCoolTime <= 0) {

			// 弾の速度
			const float kBulletSpeed = 5.0f;
			Vector3 world3DReticlePos = GetWorldPosition(worldTransform3DReticle_.matWorld_);

			Vector3 velocity = world3DReticlePos - GetWorldPosition(worldTransformL_arm_.matWorld_);
			velocity = vector.NormalizePlus(velocity, kBulletSpeed);
			int num = kSimuMax;
			for (PlayerBullet* bullet : bullets_) {
				if (bullet->GetState() == PlayerBullet::PlayerBulletState::Idle) {
					if (num<=0)
					{
						break;
					}
					bullet->SetShot(
					    viewProjection_->rotation_, velocity, (HomingMode_ && GetcheckCamera()));
					bullet->SetState(PlayerBullet::PlayerBulletState::Move);
					num--;
				}
				// assert(false);
			}
			if (num != kSimuMax) {
				isShotBullet_ = true;
				homingCoolTime = bulletIntervalHoming;
			}
		}
	}
}
/*
const Vector3& Player::GetMyWorldPosition() {
	Vector3 worldPos(0, 0, 0);

	worldPos.x = worldTransformBody_.matWorld_.m[3][0];
	worldPos.y = worldTransformBody_.matWorld_.m[3][1];
	worldPos.z = worldTransformBody_.matWorld_.m[3][2];

	return worldPos;
}
*/

Vector3 Player::GetWorldPosition(Matrix4x4 mat) {

	Vector3 worldPos(0, 0, 0);

	worldPos.x = mat.m[3][0];
	worldPos.y = mat.m[3][1];
	worldPos.z = mat.m[3][2];

	return worldPos;
}

int Player::CheckBullet() { 
	int nowBullet = 0;
	for (PlayerBullet* bullet : bullets_) {
		if (bullet->GetState() == PlayerBullet::PlayerBulletState::Idle ||
		    bullet->GetState() == PlayerBullet::PlayerBulletState::Stance) {
			nowBullet++;
		}
	}
	return nowBullet;

}

int Player::CheckBulletAll() {
	int nowBullet = 0;
	for (PlayerBullet* bullet : bullets_) {
		if (bullet->IsDead() == false && bullet->GetState() != PlayerBullet::PlayerBulletState::Death) {
			nowBullet++;
		}
	}
	return nowBullet;
}

void Player::AddBullet(){
	Vector3 velocity{0.0f, 0.0f, 0.0f};
	// 弾を生成し、初期化
	PlayerBullet* newBullet = new PlayerBullet();

	newBullet->Initialize(
	    models_[3], models_[4], models_[5], GetWorldPosition(worldTransform_.matWorld_),
	    Vector3{0.0f, 0.0f, 0.0f}, velocity);

	// newBullet->SetState(PlayerBullet::PlayerBulletState::Return);
	newBullet->SetPlayer(this);
	newBullet->setEnemy(worldTransformEnemy_);
	// 弾を登録する
	bullets_.push_back(newBullet);
}
void Player::AddBullet(int n)
{
	int num = GetBulletNumMax();
	if (kBulletMax < num+n)
	{
		n = kBulletMax - num;
	}
	for (int index = 0; index < n; index++) {
		// const float kBulletSpeed = 0.0f;

		AddBullet();
	}
}

void Player::ChangeUIUpdate() {
	if (HomingMode_)
	{
		chaser_ = {
		    (1.0f - changeAnimation_) * chaser_.x + (changeAnimation_ * valid_.x),
		    (1.0f - changeAnimation_) * chaser_.y + (changeAnimation_ * valid_.y)};
		liner_ = {
		    (1.0f - changeAnimation_) * liner_.x + (changeAnimation_ * notValid_.x),
		    (1.0f - changeAnimation_) * liner_.y + (changeAnimation_ * notValid_.y)};
		spriteChaser_->SetColor(kWhite);
		spriteLiner_->SetColor(kGray);
	}
	else
	{
		chaser_ = {
		    (1.0f - changeAnimation_) * chaser_.x + (changeAnimation_ * notValid_.x),
		    (1.0f - changeAnimation_) * chaser_.y + (changeAnimation_ * notValid_.y)};
		liner_ = {
		    (1.0f - changeAnimation_) * liner_.x + (changeAnimation_ * valid_.x),
		    (1.0f - changeAnimation_) * liner_.y + (changeAnimation_ * valid_.y)};
		spriteChaser_->SetColor(kGray);
		spriteLiner_->SetColor(kWhite);
	}

	arrowRotate_ = float(M_PI)*2.0f*changeAnimation_;

	//int fase = int(changeAnimation_ * 10.0f) / 5;
	float size_ = 1.0f;
	/* if (!fase)
	{
		size_ = 1.0f + (float(int(changeAnimation_ * 10.0f) % 5) /10.0f);
	}
	else
	{
		size_ = 1.0f + (1.0f-float(int(changeAnimation_ * 10.0f) % 5) / 10.0f);
	}*/
	spriteLiner_->SetPosition(changeUIBaseTranslation_ + liner_);
	spriteChaser_->SetPosition(changeUIBaseTranslation_ + chaser_);
	spriteArrow_->SetPosition(changeUIBaseTranslation_);
	spriteX_->SetPosition(changeUIBaseTranslation_);

	spriteLiner_->SetSize(changeUIBaseScale_);
	spriteChaser_->SetSize(changeUIBaseScale_);
	spriteArrow_->SetSize(changeUIBaseScale_);
	Vector2 child = {changeUIBaseScale_.x * buttonScale_.x*size_, changeUIBaseScale_.y * buttonScale_.y*size_};
	spriteX_->SetSize(child);

	spriteArrow_->SetRotation(arrowRotate_);

	changeAnimation_ += 1.0f / 30.0f;
	if (changeAnimation_>1.0f)
	{
		changeAnimation_ = 1.0f;
	}
}