﻿#include "Player.h"
#include<assert.h>
#include<imgui.h>
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
	adjustment_item->AddItem(groupName2, "Head_offset", Head_offset_Base);
	adjustment_item->AddItem(groupName2, "ArmL_offset", L_arm_offset_Base);
	adjustment_item->AddItem(groupName2, "ArmR_offset", R_arm_offset_Base);
	adjustment_item->AddItem(groupName, "Weapon_offset", Weapon_offset_Base);
	adjustment_item->AddItem(groupName, "floatingCycle", floatingCycle_);
	adjustment_item->AddItem(groupName, "floatingAmplitude", floatingAmplitude);
	adjustment_item->AddItem(groupName, "armAmplitude", armAmplitude);
	

	BaseCharacter::Initialize(models);
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransformBody_.Initialize();
	worldTransformBody_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransformHead_.Initialize();

	worldTransformL_arm_.Initialize();

	worldTransformR_arm_.Initialize();

	worldTransformWeapon_.Initialize();
	worldTransformWeapon_.translation_.y = 1.0f;

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

}

void Player::Update() {
	ApplyGlobalVariables();
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
		}
		
		// 振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
		
	}
	
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	if (input_->TriggerKey(DIK_A)) {
		Adjustment_Item::GetInstance()->SaveFile("Player");

	}
	

	Matrix4x4 PlayerRotateMatrix = matrix.MakeRotateMatrix(worldTransformBody_.rotation_);

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
	worldTransformBody_.UpdateMatrix(scale);
	worldTransformHead_.UpdateMatrix(scale);
	worldTransformL_arm_.UpdateMatrix(scale);
	worldTransformR_arm_.UpdateMatrix(scale);
	worldTransformWeapon_.UpdateMatrix(scale);

	ImGui::Begin("Player");
	ImGui::SliderFloat3("Head Translation", &worldTransformHead_.translation_.x, 4.0f, 10.0f);
	ImGui::SliderFloat3("ArmL Translation", &worldTransformL_arm_.translation_.x, 3.0f, 10.0f);
	ImGui::SliderFloat3("ArmR Translation", &worldTransformR_arm_.translation_.x, 3.0f, 10.0f);
	ImGui::SliderInt("floatingCycle_", &floatingCycle_, 10, 180);
	ImGui::SliderFloat("Amplitude", &floatingAmplitude, 0.1f, 1.0f);
	ImGui::SliderFloat("DisGround", &disGround, 0.1f, 1.0f);
	ImGui::End();

	ImGui::Begin("PlayerRotate");
	ImGui::SliderFloat3("ArmL Translation", &worldTransformL_arm_.rotation_.x, -90.0f, 90.0f);
	ImGui::SliderFloat3("ArmR Translation", &worldTransformR_arm_.rotation_.x, -90.0f, 90.0f);
	ImGui::End();

	ImGui::Begin("Weapon");
	ImGui::SliderFloat("Rotate", &weapon_Rotate, -0.5f, 1.58f);
	ImGui::SliderInt("WaitTime", &WaitTimeBase, 0, 120);
	ImGui::DragFloat("arm_Rotate", &arm_Rotate, 0.01f);
	ImGui::DragFloat3("Weapon_Rotate", &worldTransformWeapon_.rotation_.x, 0.01f);
	ImGui::DragFloat3("arm_L_Rotate", &worldTransformL_arm_.rotation_.x, 0.01f);
	ImGui::DragFloat3("arm_R_Rotate", &worldTransformR_arm_.rotation_.x, 0.01f);
	ImGui::End();

}

void Player::Draw(const ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransformBody_, viewProjection);
	models_[1]->Draw(worldTransformHead_, viewProjection);
	models_[2]->Draw(worldTransformL_arm_, viewProjection);
	models_[3]->Draw(worldTransformR_arm_, viewProjection);
	if (behavior_==Behavior::kAttack) {
		models_[4]->Draw(worldTransformWeapon_, viewProjection);

	}
}

void Player::BehaviorRootInitialize() { 
	move = {0.0f,0.0f,0.0f};
	worldTransformL_arm_.rotation_ = {0};
	worldTransformR_arm_.rotation_ = {0};
	worldTransformWeapon_.translation_.y = 100.0f;
	
}

void Player::BehaviorRootUpdate() {
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		kCharacterSpeed = kCharacterSpeedBase * 2.0f;
	} else {
		kCharacterSpeed = kCharacterSpeedBase;
	}

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

		ImGui::Begin("Flag");
		ImGui::DragFloat("move", &moveLength, 0.01f);
		ImGui::End();
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
			move = {0.0f};
		}
	}
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

	// 座標を加算
	worldTransform_.AddTransform(move);
	worldTransformBody_.AddTransform(move);

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B){
		behaviorRequest_ = Behavior::kAttack;
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

	if (weapon_Rotate<=MinRotate) {
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
