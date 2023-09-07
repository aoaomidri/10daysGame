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
	adjustment_item->AddItem(groupName, "Weapon_offset", Weapon_offset_Base);
	adjustment_item->AddItem(groupName, "floatingCycle", floatingCycle_);
	adjustment_item->AddItem(groupName, "floatingAmplitude", floatingAmplitude);
	adjustment_item->AddItem(groupName, "armAmplitude", armAmplitude);
	adjustment_item->AddItem(groupName2, "Head_offset", Head_offset_Base);
	adjustment_item->AddItem(groupName2, "ArmL_offset", L_arm_offset_Base);
	adjustment_item->AddItem(groupName2, "ArmR_offset", R_arm_offset_Base);


	BaseCharacter::Initialize(models);
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransformBody_.Initialize();
	worldTransformBody_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransformHead_.Initialize();

	worldTransformL_arm_.Initialize();

	worldTransformR_arm_.Initialize();

	worldTransformWeapon_.Initialize();
	worldTransformWeapon_.translation_.y = 1.0f;

	uint32_t textureReticle = TextureManager::Load("Magic.png");

	sprite2DReticle_ = Sprite::Create(textureReticle, {640.0f, 320.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});

	worldTransform3DReticle_.Initialize();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	dashCoolTime = kDashCoolTime;

	PlayerLife = kPlayerLifeMax;

	for (int index = 0;index < kBulletNum;index++)
	{
		//const float kBulletSpeed = 0.0f;
		
		Vector3 velocity{0.0f,0.0f,0.0f};
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		
		newBullet->Initialize(
		    models_[5], models_[6], models_[7], GetWorldPosition(worldTransform_.matWorld_),
		    Vector3{0.0f, 0.0f, 0.0f}, velocity);
		
		newBullet->SetState(PlayerBullet::PlayerBulletState::Idle);
		newBullet->SetPlayer(this);
		// 弾を登録する
		bullets_.push_back(newBullet);
		
	}
}

void Player::Update() {
	ApplyGlobalVariables();

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
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Behavior::kShot:
			BehaviorShotInitialize();
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
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Behavior::kShot:
		BehaviorShotUpdate();
		break;
	}



	Matrix4x4 PlayerRotateMatrix = matrix.MakeRotateMatrix(worldTransformBody_.rotation_);

	for (int i = 0; i < 3; i++) {
		obb.orientations[i].x = PlayerRotateMatrix.m[i][0];
		obb.orientations[i].y = PlayerRotateMatrix.m[i][1];
		obb.orientations[i].z = PlayerRotateMatrix.m[i][2];
	}

	obb.center = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y + 4.5f,
	    worldTransform_.translation_.z};

	Head_offset = vector.TransformNormal(Head_offset_Base, PlayerRotateMatrix);
	L_arm_offset = vector.TransformNormal(L_arm_offset_Base, PlayerRotateMatrix);
	R_arm_offset = vector.TransformNormal(R_arm_offset_Base, PlayerRotateMatrix);
	Weapon_offset = vector.TransformNormal(Weapon_offset_Base, PlayerRotateMatrix);
	// 座標をコピーしてオフセット分ずらす
	worldTransformHead_.translation_ = worldTransformBody_.translation_ + Head_offset;
	worldTransformL_arm_.translation_ = worldTransformBody_.translation_ + L_arm_offset;
	worldTransformR_arm_.translation_ = worldTransformBody_.translation_ + R_arm_offset;
	worldTransformWeapon_.translation_ = worldTransformBody_.translation_ + Weapon_offset;
	
	

	// 座標を転送
	worldTransformBody_.UpdateMatrix(bodyScale);
	worldTransformHead_.UpdateMatrix(headScale);
	worldTransformL_arm_.UpdateMatrix(leftArmScale);
	worldTransformR_arm_.UpdateMatrix(scale);
	worldTransformWeapon_.UpdateMatrix(scale);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	

}

void Player::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransformBody_, viewProjection);
	models_[1]->Draw(worldTransformHead_, viewProjection);
	models_[2]->Draw(worldTransformL_arm_, viewProjection);
	models_[3]->Draw(worldTransformR_arm_, viewProjection);
	if (behavior_== Behavior::kAttack) {
		models_[4]->Draw(worldTransformWeapon_, viewProjection);
	}

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
	
}

void Player::DrawUI() { 
	if (joyState.Gamepad.bLeftTrigger != 0) {
		sprite2DReticle_->Draw();
	}
	
}

void Player::BehaviorRootInitialize() { 
	move = {0.0f,0.0f,0.0f};
	worldTransformL_arm_.rotation_ = {0};
	worldTransformR_arm_.rotation_ = {0};
	worldTransformWeapon_.translation_.y = 100.0f;
	
}

void Player::BehaviorRootUpdate() {
	
	kCharacterSpeed = kCharacterSpeedBase;

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
	worldTransformHead_.rotation_.y = worldTransformBody_.rotation_.y;

	worldTransformL_arm_.rotation_.y = worldTransformBody_.rotation_.y;
	worldTransformR_arm_.rotation_.y = worldTransformBody_.rotation_.y;
	UpdateFloatingGimmick();
	UpdateMoveArm();

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

void Player::BehaviorAttackInitialize() {
	worldTransformWeapon_.rotation_.y = worldTransformBody_.rotation_.y;
	worldTransformWeapon_.translation_.y = 1.0f;
	WaitTime = WaitTimeBase;
	weapon_Rotate = 0.5f;
	isShakeDown = false;

}

void Player::BehaviorAttackUpdate() {

	if (weapon_Rotate <= MinRotate){
		isShakeDown = true;
	} 
	else if (weapon_Rotate >= MaxRotate) {
		WaitTime -= 1;
		weapon_Rotate = 1.58f;
	}
	
	if (!isShakeDown && weapon_Rotate > MinRotate){
		weapon_Rotate -= moveWeapon;
	} 
	else if (isShakeDown && weapon_Rotate < MaxRotate) {
		weapon_Rotate += moveWeaponShakeDown;
	}

	worldTransformWeapon_.rotation_.x = weapon_Rotate;
	worldTransformL_arm_.rotation_.x = arm_Rotate + weapon_Rotate;
	worldTransformR_arm_.rotation_.x = arm_Rotate + weapon_Rotate;

	if (WaitTime<=0) {
		behavior_ = Behavior::kRoot;
	}
	//weapon_Rotate==0.0fの時arm_Rotate-3.15f

	
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
	worldTransformR_arm_.rotation_.x = std::sin(armParameter_) * armAmplitude;
	

}

void Player::ApplyGlobalVariables() {
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";
	const char* groupName2 = "PlayerPrats";

	Head_offset_Base = adjustment_item->GetVector3Value(groupName2, "Head_offset");
	L_arm_offset_Base = adjustment_item->GetVector3Value(groupName2, "ArmL_offset");
	R_arm_offset_Base = adjustment_item->GetVector3Value(groupName2, "ArmR_offset");

	Weapon_offset_Base = adjustment_item->GetVector3Value(groupName, "Weapon_offset");
	floatingCycle_ = adjustment_item->GetIntValue(groupName, "floatingCycle");
	floatingAmplitude = adjustment_item->GetfloatValue(groupName, "floatingAmplitude");
	armAmplitude = adjustment_item->GetfloatValue(groupName, "armAmplitude");
	kCharacterSpeedBase = adjustment_item->GetfloatValue(groupName, "CharacterSpeed");
}

void Player::BehaviorDashInitialize() { 
	workDash_.dashParameter_ = 0;
	worldTransformBody_.rotation_.y = target_angle;
}

void Player::BehaviorDashUpdate() { 
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
	worldTransformHead_.rotation_.y = worldTransformBody_.rotation_.y;

	worldTransformL_arm_.rotation_.y = worldTransformBody_.rotation_.y;
	worldTransformR_arm_.rotation_.y = worldTransformBody_.rotation_.y;

	//既定の時間経過で通常状態に戻る
	if (++workDash_.dashParameter_>=behaviorDashTime) {
		dashCoolTime = kDashCoolTime;
		behaviorRequest_ = Behavior::kRoot;
	}
	UpdateFloatingGimmick();
	UpdateMoveArm();
}

void Player::BehaviorShotInitialize() { kCharacterSpeed = 0.2f; }

void Player::BehaviorShotUpdate() {

	ShotReticle(viewProjection_->matView, viewProjection_->matProjection);

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

	
	worldTransformHead_.rotation_.y = worldTransformBody_.rotation_.y;

	worldTransformL_arm_.rotation_.x = 1.57f + (viewProjection_->rotation_.x * 1.1f);
	worldTransformL_arm_.rotation_.y =worldTransformBody_.rotation_.y;
	
	worldTransformR_arm_.rotation_.y = worldTransformBody_.rotation_.y;

	floatingAmplitude = 0.1f;
	UpdateFloatingGimmick();

	worldTransform_.rotation_.y = worldTransformBody_.rotation_.y;

	L_arm_offset_Base.z = 0.5f;

	Attack();

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

	// 座標を加算
	worldTransform_.AddTransform(move);
	worldTransformBody_.AddTransform(move);
}

void Player::OnCollision() { 
	chackCollision = 1;
	PlayerLife--;
}

void Player::DrawImgui() {
	ImGui::Begin("Player");
	ImGui::DragFloat3("Body Translation", &worldTransformBody_.translation_.x);
	ImGui::SliderFloat3("ArmL Translation", &worldTransformL_arm_.translation_.x, 3.0f, 10.0f);
	ImGui::SliderFloat3("ArmR Translation", &worldTransformR_arm_.translation_.x, 3.0f, 10.0f);
	ImGui::SliderInt("floatingCycle_", &floatingCycle_, 10, 180);
	ImGui::SliderFloat("Amplitude", &floatingAmplitude, 0.1f, 1.0f);
	ImGui::SliderFloat("DisGround", &disGround, 0.1f, 1.0f);
	ImGui::DragInt("chackCollision", &chackCollision);
	ImGui::End();

	ImGui::Begin("PlayerRotate");
	ImGui::SliderFloat3("ArmL Rotate", &worldTransformL_arm_.rotation_.x, -3.0f, 3.0f);
	ImGui::SliderFloat3("ArmR Rotate", &worldTransformR_arm_.rotation_.x, -3.0f, 3.0f);
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
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
		
	}
#ifdef _DEBUG

	ImGui::Begin("Shot");
	ImGui::DragFloat3("ArmL Rotate", &offset.x);
	ImGui::End();
#endif
	
	// ここまで↑
}

void Player::Attack() {
	if (joyState.Gamepad.bRightTrigger != 0 || (joyState.Gamepad.wButtons&&XINPUT_GAMEPAD_B)) {
		bulletTime += 1;
		if (bulletTime % bulletInterval == 1) {
			
			// 弾の速度
			const float kBulletSpeed = 5.0f;
			Vector3 world3DReticlePos = GetWorldPosition(worldTransform3DReticle_.matWorld_);

			Vector3 velocity = world3DReticlePos - GetWorldPosition(worldTransformL_arm_.matWorld_);
			velocity = vector.NormalizePlus(velocity, kBulletSpeed);
			
			
			/*
			// 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			
			newBullet->Initialize(
			    models_[5], GetWorldPosition(worldTransformL_arm_.matWorld_),
			    viewProjection_->rotation_, velocity);
			// 弾を登録する
			bullets_.push_back(newBullet);
			*/
			for (PlayerBullet* bullet : bullets_)
			{
				if (bullet->GetState() == PlayerBullet::PlayerBulletState::Idle)
				{
					bullet->SetShot(
					    GetWorldPosition(worldTransformL_arm_.matWorld_),
					    viewProjection_->rotation_, velocity);
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