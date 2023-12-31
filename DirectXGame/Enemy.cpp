﻿#include "Enemy.h"
#include<imgui.h>
#include<assert.h>
Enemy::~Enemy() {
	{
		for (EnemyEffect* effect : effect_) {
			delete effect;
		}

		for (EnemyBullet* bullet : bullets_) {
			delete bullet;
		}

		delete model_;
	}
}

void Enemy::Initialize(const std::vector<Model*>& models) { 
	BaseCharacter::Initialize(models);
	worldTransform_.translation_ = {0.0f, 5.0f, 150.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	move = {0.5f, 0.0f, 0.5f};

	enemyMoveCount = 600;
	enemyMoveInterval = 90;

	worldTransformL_parts_.Initialize();
	worldTransformL_parts_.rotation_ = {1.57f, 0.0f, 0.0f};
	worldTransformR_parts_.Initialize();
	worldTransformR_parts_.rotation_ = {1.57f, 0.0f, 0.0f};

	worldTransformAir.Initialize();

	AirOffset = {0, 30.0f, 0};

	model_ = Model::Create();

	fireTimer = kFireInterval;

	EnemyLife = kMaxEnemyLife;

	input_ = Input::GetInstance();

}

void Enemy::Update() {
	// デスフラグの立ったエフェクトを削除
	effect_.remove_if([](EnemyEffect* effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
#ifdef _DEBUG
	ImGui::Begin("Enemy");
	ImGui::DragFloat3("Position", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragInt("chackCollision", &chackCollision);
	ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::Text(
	    "targetTranslation = %.1f, %.1f, %.1f", target_->translation_.x, target_->translation_.y,
	    target_->translation_.z);
	ImGui::Text("EnemyLife = %d", EnemyLife);
	ImGui::Text("EnemyMoveCount = %d", enemyMoveCount);
	ImGui::Text("EnemyMoveInterval = %d", enemyMoveInterval);
	ImGui::Text("EnemyLifePer = %.1f", enemyLifePer);
	ImGui::End();

	if (input_->TriggerKey(DIK_E)) {
		EnemyLife = 10.0f;
	}
	#endif

	if (EnemyLife == 0.0f) {
		isDead = true;
	} else {
		isDead = false;
	}
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kFirst:
			BehaviorFirstInitialize();
			break;
		case Behavior::kSecond:
			BehaviorSecondInitialize();
			break;
		case Behavior::kThird:
			BehaviorThirdInitialize();
			break;
		case Behavior::kLast:
			BehaviorLastInitialize();
			break;
		}
	}	
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;

	switch (behavior_) {
	case Behavior::kFirst:
	default:
		BehaviorFirstUpdate();
		break;
	case Behavior::kSecond:
		BehaviorSecondUpdate();
		break;
	case Behavior::kThird:
		BehaviorThirdUpdate();
		break;
	case Behavior::kLast:
		BehaviorLastUpdate();
		break;
	}

	

	/*ImGui::Begin("EnemyParts");
	ImGui::DragFloat3("EnemyPosition", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("Enemy_Rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Move", &move.x, 0.01f);*/
	// ImGui::DragFloat("EnemyPartsL_Rotate", &worldTransformL_parts_.rotation_.x, 0.01f);
	// ImGui::DragFloat("EnemyPartsR_Rotate", &worldTransformR_parts_.rotation_.x ,0.01f);
	// ImGui::End();

	Matrix4x4 EnemyRotateMatrix = matrix.MakeRotateMatrix(worldTransform_.rotation_);
	Vector3 L_parts_offset = {7.0f, 7.0f, 0.0f};
	Vector3 R_parts_offset = {-7.0f, 7.0f, 0.0f};

	L_parts_offset = vector_.TransformNormal(L_parts_offset, EnemyRotateMatrix);
	R_parts_offset = vector_.TransformNormal(R_parts_offset, EnemyRotateMatrix);

	worldTransformL_parts_.translation_ = worldTransform_.translation_ + L_parts_offset;
	worldTransformR_parts_.translation_ = worldTransform_.translation_ + R_parts_offset;

	worldTransformL_parts_.rotation_.x += 1.0f / (static_cast<float>(M_PI) * 2.0f);
	worldTransformR_parts_.rotation_.x += 1.0f / (static_cast<float>(M_PI) * 2.0f);

	 Vector3 vector = {
	    target_->translation_.x - worldTransform_.translation_.x,
	    target_->translation_.y - worldTransform_.translation_.y,
	    target_->translation_.z - worldTransform_.translation_.z};
	if (!isRotate) {
		worldTransform_.rotation_.y = std::atan2(vector.x, vector.z);

	} else {
		worldTransform_.rotation_.y = vector_.LerpShortAngle(
		    worldTransform_.rotation_.y, std::atan2(vector.x, vector.z), 0.01f);
	}

	worldTransformL_parts_.rotation_.y=worldTransform_.rotation_.y; 
	worldTransformR_parts_.rotation_.y=worldTransform_.rotation_.y;

	worldTransform_.UpdateMatrix(scale);
	worldTransformL_parts_.UpdateMatrix(scale);
	worldTransformR_parts_.UpdateMatrix(scale);

	for (int i = 0; i < 3; i++) {
		obb.orientations[i].x = EnemyRotateMatrix.m[i][0];
		obb.orientations[i].y = EnemyRotateMatrix.m[i][1];
		obb.orientations[i].z = EnemyRotateMatrix.m[i][2];
	}

	obb.center = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y + 7.0f,
	    worldTransform_.translation_.z};
	

	for (EnemyBullet* bullet: bullets_) {
		bullet->Update();
	}

	for (EnemyEffect* effect : effect_) {
		effect->Update();
	}

	enemyLifePer = 1280.0f * (EnemyLife / kMaxEnemyLife);
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	if (isDead == false)
	{
		models_[0]->Draw(worldTransform_, viewProjection);
		models_[1]->Draw(worldTransformL_parts_, viewProjection);
		models_[2]->Draw(worldTransformR_parts_, viewProjection);
	}

	for (EnemyBullet* bullet:bullets_) {
		bullet->Draw(viewProjection);
	}

	for (EnemyEffect* effect : effect_) {
		effect->Draw(viewProjection);
	}
}

void Enemy::HitEffect() {
	EnemyEffect* newEffect;

	newEffect = new EnemyEffect;

	newEffect->Initialize(model_, GetMyWorldPosition());
	//
	effect_.push_back(newEffect);

	newEffect->HitEffect();
}


void Enemy::OnCollision() {
	for (int i = 0; i < 7; i++) {
		HitEffect();
	}
	EnemyLife -= 1.0f;
	
	
}

void Enemy::Fire(float bulletSpeed) {

	// 弾の速度
	float kBulletSpeed = bulletSpeed;
	
		Vector3 velocity = {0, 0, 0};
		Vector3 enemyPos = GetMyWorldPosition();
		Vector3 vector = {
		    target_->translation_.x - enemyPos.x, target_->translation_.y - enemyPos.y + 3.5f,
		    target_->translation_.z - enemyPos.z};
		float bulletNorm =
		    sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

		if (bulletNorm != 0.0f) {

			velocity = {
			    ((vector.x / bulletNorm) * kBulletSpeed),
			    (vector.y / bulletNorm) * kBulletSpeed,
			    (vector.z / bulletNorm) * kBulletSpeed};
		}

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, enemyPos, worldTransform_.rotation_, velocity);
		// 発射した弾をリストに登録する
		bullets_.push_back(newBullet);
}

void Enemy::TripleFire(float bulletSpeed) {

	// 弾の速度
	float kBulletSpeed = bulletSpeed;
	for (int i = 0; i < 3; i++) {

		Vector3 velocity = {0, 0, 0};
		Vector3 enemyPos = GetMyWorldPosition();
		Vector3 vector = {
		    target_->translation_.x - enemyPos.x, target_->translation_.y - enemyPos.y + 3.5f,
		    target_->translation_.z - enemyPos.z};
		float bulletNorm =
		    sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

		if (bulletNorm != 0.0f) {

		velocity = {
			    (((vector.x + ((i - 1) * 20.0f)) / bulletNorm) * kBulletSpeed),
			    (vector.y / bulletNorm) * kBulletSpeed,
			(vector.z / bulletNorm) * kBulletSpeed};
		}

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, enemyPos, worldTransform_.rotation_, velocity);
		// 発射した弾をリストに登録する
		bullets_.push_back(newBullet);
	}
}

void Enemy::randFire(float bulletSpeed) {
	// 弾の速度
	float kBulletSpeed = bulletSpeed;
	Vector3 enemyPos = GetMyWorldPosition();

	unsigned int currenTime = static_cast<unsigned int>(time(nullptr));
	srand(currenTime);

	for (int i = 0; i < 10; i++) {

		Vector3 velocity = {0, 0, 0};
		Vector3 vector = {
		    target_->translation_.x - enemyPos.x + rand() % 101 - 50,
		    target_->translation_.y - enemyPos.y + 3.5f + rand() % 101 - 50,
		    target_->translation_.z - enemyPos.z + rand() % 101 - 50};
		float bulletNorm =
		    sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

		if (bulletNorm != 0.0f) {

			velocity = {
				((vector.x / bulletNorm) * kBulletSpeed), 
				((vector.y / bulletNorm) * kBulletSpeed),
				((vector.z / bulletNorm) * kBulletSpeed)};
		}

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, enemyPos, worldTransform_.rotation_, velocity);
		// 発射した弾をリストに登録する
		bullets_.push_back(newBullet);
	}
	Fire(bulletSpeed);

}

void Enemy::NormalAttack() {
	 if (isDead == false) {
		// 発射タイマーカウントダウン
		fireTimer -= 1;
	 }
	 if (fireTimer==30&&fireCount%3==2) {
		TripleFire(2.0f);
	 }
	 if (fireTimer == 30 && fireCount % 10 == 9) {
		randFire(0.8f);
	 }

	 // 指定時間に達した
	 if (fireTimer == 0) {
		// 弾を発射

		Fire(2.0f);
		//カウント追加
		fireCount++;

		// タイマー初期化
		fireTimer = kFireInterval;
	 }

}

void Enemy::SecondAttack() {
	 if (isDead == false) {
		// 発射タイマーカウントダウン
		fireTimer -= 1;
	 }
	 if (fireTimer == 30 && fireCount % 3 == 2) {
		TripleFire(2.0f);
	 }

	 // 指定時間に達した
	 if (fireTimer == 0) {
		// 弾を発射

		Fire(2.0f);
		// カウント追加
		fireCount++;

		// タイマー初期化
		fireTimer = kFireInterval;
	 }
}

void Enemy::FlyAttack(float bulletSpeed) {
	 if (isDead == false && worldTransform_.translation_.y >= 30.0f) {
		// 発射タイマーカウントダウン
		fireTimer -= 1;
	 }

	 // 指定時間に達した
	 if (fireTimer%12==1) {
		// 弾を発射

		TripleFire(bulletSpeed);
		// カウント追加
		fireCount++;

		// タイマー初期化
		fireTimer = kFireInterval;
	 }

}

void Enemy::BehaviorFirstInitialize() { 
	move = {0.5f, 0.0f, 0.5f};
	fireCount = 0;
	enemyMoveCount = 600;
	enemyMoveInterval = 180;
}
void Enemy::BehaviorFirstUpdate() {
	 //// キャラクターの移動ベクトル

	
	 if (enemyMoveCount >= enemyMoveCountMax) {
		if (enemyMoveInterval>0) {
			enemyMoveInterval--;
		kFireInterval = 60;
		} else {
			if (move.x > 0 && move.z < 0) {
				move.x *= -1;
			} else if (move.x < 0 && move.z < 0) {
				move.z *= -1;
			} else if (move.x < 0 && move.z > 0) {
				move.x *= -1;
			} else if (move.x > 0 && move.z > 0) {
				move.z *= -1;
			}
			enemyMoveCount = 0;
			if (enemyMoveInterval <= 0) {
				kFireInterval = 45;
				enemyMoveInterval = 240;
			} 
		}

	 } else {
		enemyMoveCount++;
		worldTransform_.AddTransform(move);
	 }
	
	 NormalAttack();

	 if (EnemyLife <= 170.0f) {
		behaviorRequest_ = Behavior::kSecond;
	 }
}
void Enemy::BehaviorSecondInitialize() {
	 move = {0.5f, 0.0f, 0.5f};
	 fireCount = 0;
	 enemyMoveCount = 0;
	 enemyMoveInterval = 800;

	 worldTransformAir.translation_ = worldTransform_.translation_ + AirOffset;

}
void Enemy::BehaviorSecondUpdate() { 
	if (worldTransform_.translation_.y < 35.0f) {
		worldTransform_.translation_ =
		    vector_.Lerp(worldTransform_.translation_, worldTransformAir.translation_, 0.02f);
	 }
	
	

	 if (enemyMoveCount >= enemyMoveCountMax) {
		if (enemyMoveInterval > 0) {
			enemyMoveInterval--;
			kFireInterval = 60;			
			
			worldTransform_.translation_ =
				vector_.Lerp(worldTransform_.translation_, movePos[moveCount], 0.04f);
			
			if (vector_.Length(worldTransform_.translation_ - movePos[moveCount]) <= 5.0f &&moveCount < 6) {
				randFire(2.0f);
				moveCount++;
				
			}
			
		} else {
			moveCount = 0;
			enemyMoveCount = 0;
			if (enemyMoveInterval <= 0) {
				kFireInterval = 45;
				enemyMoveInterval = 800;
			}
		}

	} else {
		enemyMoveCount++;
		FlyAttack(4.0f);
		for (int i = 0; i < 4; i++) {
			movePos[i].x = {(rand() % 461 - 230) / 1.0f};
			movePos[i].y = {36.0f};
			movePos[i].z = {(rand() % 461 - 230) / 1.0f};
		}
	}

	 if (EnemyLife <= 80.0f) {
		behaviorRequest_ = Behavior::kThird;
	}
}
void Enemy::BehaviorThirdInitialize() {
	fireCount = 0;
	enemyMoveCount = 0;

	worldTransformAir.translation_ = worldTransform_.translation_ + AirOffset;

}
void Enemy::BehaviorThirdUpdate() { 

	if (worldTransform_.translation_.y < 35.0f) {
		worldTransform_.translation_ =
		    vector_.Lerp(worldTransform_.translation_, worldTransformAir.translation_, 0.02f);
	}

	if (enemyMoveCount >= enemyMoveCountMax) {
		if (enemyMoveInterval > 0) {
			enemyMoveInterval--;
			kFireInterval = 60;

			worldTransform_.translation_ =
			    vector_.Lerp(worldTransform_.translation_, movePos[moveCount], 0.06f);

			if (vector_.Length(worldTransform_.translation_ - movePos[moveCount]) <= 4.0f &&
			    moveCount < 6) {
				randFire(4.5f);
				moveCount++;
			}

		} else {
			moveCount = 0;
			enemyMoveCount = 0;
			if (enemyMoveInterval <= 0) {
				kFireInterval = 45;
				enemyMoveInterval = 600;
			}
		}

	} else {
		enemyMoveCount++;
		FlyAttack(6.0f);
		for (int i = 0; i < 5; i++) {
			movePos[i].x = {(rand() % 461 - 230) / 1.0f};
			movePos[i].y = {36.0f};
			movePos[i].z = {(rand() % 461 - 230) / 1.0f};
		}
	}

}
void Enemy::BehaviorLastInitialize() {


}
void Enemy::BehaviorLastUpdate() {


}
