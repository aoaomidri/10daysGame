#include "Enemy.h"
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

	move = {1.0f, 0.0f, -1.0f};

	enemyMoveCount = 0;
	enemyMoveInterval = 90;

	worldTransformHitBox_.Initialize();
	
	worldTransformRoll_.Initialize();
	worldTransformRoll_.parent_ = &worldTransform_;
	
	worldTransformL_parts_.Initialize();
	worldTransformL_parts_.translation_ = Fin_offset_Base;
	worldTransformL_parts_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransformL_parts_.parent_ = &worldTransformRoll_;
	//worldTransformR_parts_.Initialize();
	//worldTransformR_parts_.rotation_ = {1.57f, 0.0f, 0.0f};

	worldTransformAir.Initialize();

	AirOffset = {0, 70.0f, 0};

	fireTimer = kFireInterval;

	EnemyLife = kMaxEnemyLife;

	isDead = false;

	isRotate = false;

	input_ = Input::GetInstance();

	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Enemy";
	// グループを追加
	adjustment_item->CreateGroup(groupName);

	model_ = Model::Create();

	//adjustment_item->AddItem(groupName, "EnemyLife", EnemyLife);
	adjustment_item->AddItem(groupName, "TackleSpeed", tackleSpeedBase);

	isOpen = false;
}

void Enemy::ApplyGlobalVariables() {
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Enemy";

	//EnemyLife = adjustment_item->GetfloatValue(groupName, "EnemyLife");
	tackleSpeedBase = adjustment_item->GetfloatValue(groupName, "tackleSpeed");
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
	ImGui::DragFloat3("PartsRotate", &worldTransformL_parts_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Move", &move.x, 0.01f);
	ImGui::DragFloat3("Fin", &Fin_offset_Base.x, 0.01f);
	ImGui::Text(
	    "targetTranslation = %.1f, %.1f, %.1f", target_->translation_.x, target_->translation_.y,
	    target_->translation_.z);

	ImGui::Text("DeathTimer = %d", deathTimer);
	ImGui::Text("EnemyLife = %d", EnemyLife);
	ImGui::Text("EnemyMoveCount = %d", enemyMoveCount);
	ImGui::Text("EnemyMoveInterval = %d", enemyMoveInterval);
	ImGui::Text("isTackle = %d", isTackle);
	ImGui::Text("EnemyLifePer = %.1f", enemyLifePer);
	ImGui::Text("ActionCount = %d", EnemyActionsCount);
	ImGui::End();

	if (input_->TriggerKey(DIK_G)) {
		EnemyLife = 10.0f;
	}
	#endif
	
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
		case Behavior::kDead:
			BehaviorDeadInitialize();
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
	case Behavior::kDead:
		BehaviorDeadUpdate();
		break;
	}

	

	/*ImGui::Begin("EnemyParts");
	ImGui::DragFloat3("EnemyPosition", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("Enemy_Rotate", &worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Move", &move.x, 0.01f);*/
	// ImGui::DragFloat("EnemyPartsL_Rotate", &worldTransformL_parts_.rotation_.x, 0.01f);
	// ImGui::DragFloat("EnemyPartsR_Rotate", &worldTransformR_parts_.rotation_.x ,0.01f);
	// ImGui::End();

	

	//worldTransformL_parts_.translation_ = worldTransform_.translation_ + L_parts_offset;
	//worldTransformR_parts_.translation_ = worldTransform_.translation_ + R_parts_offset;


	 Vector3 vector = {
	    target_->translation_.x - worldTransform_.translation_.x,
	    target_->translation_.y - worldTransform_.translation_.y,
	    target_->translation_.z - worldTransform_.translation_.z};
	if (isRotate) {
		worldTransform_.rotation_.y = std::atan2(vector.x, vector.z);
		Vector3 velocityXZ{vector.x, 0.0f, vector.z};
		float besage = vector_.Length(velocityXZ);
		worldTransform_.rotation_.x = std::atan2(-vector.y, besage);

	} else {
	}

	Matrix4x4 EnemyRotateMatrix = matrix.MakeRotateMatrix(worldTransform_.rotation_);
	Vector3 OBB_offset = {0.0f, 0.0f, 5.0f};
	Vector3 shot_offsetBase = {0.0f, -10.0f, 15.0f};

	OBB_offset = vector_.TransformNormal(OBB_offset, EnemyRotateMatrix);
	shot_offset = vector_.TransformNormal(shot_offsetBase, EnemyRotateMatrix);

	//worldTransformL_parts_.rotation_.y=worldTransform_.rotation_.y; 
	//worldTransformR_parts_.rotation_.y=worldTransform_.rotation_.y;

	worldTransformL_parts_.translation_ = Fin_offset_Base;
	

	FinAnimationUpdate();
	worldTransform_.UpdateMatrix(scale);
	worldTransformHitBox_.UpdateMatrix(obb.size);
	worldTransformRoll_.UpdateMatrix(scaleChild);
	worldTransformL_parts_.UpdateMatrix(scaleChild);
	//worldTransformR_parts_.UpdateMatrix(scale);

	for (int i = 0; i < 3; i++) {
		obb.orientations[i].x = EnemyRotateMatrix.m[i][0];
		obb.orientations[i].y = EnemyRotateMatrix.m[i][1];
		obb.orientations[i].z = EnemyRotateMatrix.m[i][2];
	}

	obb.center = {
	    worldTransform_.translation_.x + OBB_offset.x,
	    worldTransform_.translation_.y + OBB_offset.y,
	    worldTransform_.translation_.z + OBB_offset.z
	};

	worldTransformHitBox_.translation_ = obb.center;
	worldTransformHitBox_.rotation_ = worldTransform_.rotation_;
	

	for (EnemyBullet* bullet: bullets_) {
		bullet->Update();
	}

	for (EnemyEffect* effect : effect_) {
		effect->Update();
	}

	enemyLifePer = 1280.0f * (EnemyLife / kMaxEnemyLife);
}

void Enemy::FinAnimationUpdate() {
	finRotate += 0.05f;
	worldTransformL_parts_.rotation_.y = (std::sin(finRotate)) * float(M_PI) / 4.0f;
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	//if (isDead == false)
	{
		if (isOpen)
		{
			models_[2]->Draw(worldTransformRoll_, viewProjection);
		}
		else
		{
			models_[0]->Draw(worldTransformRoll_, viewProjection);
		}
		//models_[0]->Draw(worldTransformRoll_, viewProjection);
		models_[1]->Draw(worldTransformL_parts_, viewProjection);
		//models_[2]->Draw(worldTransformRoll_, viewProjection);

		//model_->Draw(worldTransformHitBox_, viewProjection);
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
	Vector3 enemyPos = GetMyWorldPosition() + shot_offset;
		Vector3 vector = {
		    target_->translation_.x - enemyPos.x, target_->translation_.y - enemyPos.y + 1.0f,
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
	    newBullet->Initialize(models_[3], enemyPos, worldTransform_.rotation_, velocity);
		// 発射した弾をリストに登録する
		bullets_.push_back(newBullet);
}

void Enemy::TripleFire(float bulletSpeed) {

	// 弾の速度
	float kBulletSpeed = bulletSpeed;
	for (int i = 0; i < 3; i++) {

		Vector3 velocity = {0, 0, 0};
		    Vector3 enemyPos = GetMyWorldPosition() + shot_offset;
		Vector3 vector = {
		    target_->translation_.x - enemyPos.x, target_->translation_.y - enemyPos.y + 1.0f,
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
		newBullet->Initialize(models_[3], enemyPos, worldTransform_.rotation_, velocity);
		// 発射した弾をリストに登録する
		bullets_.push_back(newBullet);
	}
}

void Enemy::randFire(float bulletSpeed) {
	// 弾の速度
	float kBulletSpeed = bulletSpeed;
	Vector3 enemyPos = GetMyWorldPosition() + shot_offset;

	unsigned int currenTime = static_cast<unsigned int>(time(nullptr));
	srand(currenTime);

	for (int i = 0; i < 10; i++) {

		Vector3 velocity = {0, 0, 0};
		Vector3 vector = {
		    target_->translation_.x - enemyPos.x + rand() % 101 - 50,
		    target_->translation_.y - enemyPos.y + 1.0f + rand() % 101 - 50,
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
		newBullet->Initialize(models_[3], enemyPos, worldTransform_.rotation_, velocity);
		// 発射した弾をリストに登録する
		bullets_.push_back(newBullet);
	}
	Fire(bulletSpeed);

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

void Enemy::TackleInitialize() {
	 rotate = 0;

	 isTackle = false;

	 tackleTimer = 0;
	 tackleTimerMax = 120;
	 tackleMoveCount = 0;
	 tackleMoveTime = 0;
	 tackleMoveInterval = 120;
	 tackleMoveCountMax = 3;
	 
}

void Enemy::Tackle(float tackleSpeed) { 
	/*tackleMoveCount = 0;
	 rotate = 0;*/
	 // 弾の速度
	 float kTackleSpeed = tackleSpeed;
	 worldTransform_.rotation_.x = 0;
	 worldTransformRoll_.rotation_.z += rotate;
	 if (tackleMoveCount < tackleMoveCountMax) {
		if (rotate < 2.0f) {
			rotate += 0.01f;
		} else {			

			if (isTackle == false) {

				Vector3 enemyPos = GetMyWorldPosition();
				Vector3 vector = {
					target_->translation_.x - enemyPos.x,
					target_->translation_.y - enemyPos.y + 5.0f,
					target_->translation_.z - enemyPos.z};
				float bulletNorm =
					sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

				if (bulletNorm != 0.0f) {

					move = {
						((vector.x / bulletNorm) * kTackleSpeed),
						0 ,
						(vector.z / bulletNorm) * kTackleSpeed
					};
				}
				tackleMoveTime++;
				if (tackleMoveTime == tackleMoveInterval) {
					tackleMoveTime = 0;

					isTackle = true;
				}
			}
			if (isTackle == true) {

				tackleTimer++;
				if (tackleTimer == tackleTimerMax) {
					tackleTimer = 0;
					tackleMoveCount++;
					isTackle = false;
				}
			}
			
		}
	 } else {
		if (rotate > 0.01f) {
			rotate -= 0.01f;
		} else {
			move = {1.0f, 0, 1.0f};
			EnemyActionsCount++;
			attack_ = Attack::Normal;
		}
	 }
	 worldTransform_.rotation_.y = std::atan2(move.x, move.z);
	 Vector3 velocityXZ{move.x, 0.0f, move.z};
	 float besage = vector_.Length(velocityXZ);
	 worldTransform_.rotation_.x = std::atan2(-move.y, besage);

	 if (isTackle) {
		if ((worldTransform_.translation_ + move).x > MoveMax) {
			move.x = {0};
			worldTransform_.translation_.x = MoveMax;
		} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
			move.x = {0};
			worldTransform_.translation_.x = -MoveMax;
		}

		if ((worldTransform_.translation_ + move).z > MoveMax) {
			move.z = {0};
			worldTransform_.translation_.z = MoveMax;
		} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
			move.z = {0};
			worldTransform_.translation_.z = -MoveMax;
		}
		worldTransform_.AddTransform(move);
	 }
	

}

void Enemy::Tackle2(float tackleSpeed) {
	 /*tackleMoveCount = 0;
	  rotate = 0;*/
	 // 弾の速度
	 float kTackleSpeed = tackleSpeed;

	 worldTransform_.rotation_.x = 0;
	 worldTransformRoll_.rotation_.z += rotate;
	 if (tackleMoveCount < tackleMoveCountMax) {
		if (rotate < 2.0f) {
			rotate += 0.01f;
		} else {

			if (isTackle == false) {

				Vector3 enemyPos = GetMyWorldPosition();
				Vector3 vector = {
				    target_->translation_.x - enemyPos.x,
				    target_->translation_.y - enemyPos.y + 5.0f,
				    target_->translation_.z - enemyPos.z};
				float bulletNorm =
				    sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

				if (bulletNorm != 0.0f) {

					move = {
					    ((vector.x / bulletNorm) * kTackleSpeed), 0,
					    (vector.z / bulletNorm) * kTackleSpeed};
				}
				
				if (tackleMoveTime % 30 == 29){
					randFire(3.0f);

				}
				tackleMoveTime++;
				if (tackleMoveTime == tackleMoveInterval) {
					tackleMoveTime = 0;

					isTackle = true;
				}
			}
			if (isTackle == true) {

				tackleTimer++;
				if (tackleTimer == tackleTimerMax) {
					tackleTimer = 0;
					tackleMoveCount++;
					isTackle = false;
				}
			}
		}
	 } else {
		if (rotate > 0.01f) {
			rotate -= 0.01f;
		} else {
			move = {1.0f, 0, 1.0f};
			EnemyActionsCount++;
			attack_ = Attack::Normal;
		}
	 }
	 worldTransform_.rotation_.y = std::atan2(move.x, move.z);
	 Vector3 velocityXZ{move.x, 0.0f, move.z};
	 float besage = vector_.Length(velocityXZ);
	 worldTransform_.rotation_.x = std::atan2(-move.y, besage);

	 if (isTackle) {
		if ((worldTransform_.translation_ + move).x > MoveMax) {
			move.x = {0};
			worldTransform_.translation_.x = MoveMax;
		} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
			move.x = {0};
			worldTransform_.translation_.x = -MoveMax;
		}

		if ((worldTransform_.translation_ + move).z > MoveMax) {
			move.z = {0};
			worldTransform_.translation_.z = MoveMax;
		} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
			move.z = {0};
			worldTransform_.translation_.z = -MoveMax;
		}
		worldTransform_.AddTransform(move);
	 }
}

void Enemy::ExTackleInitialize() {
	 rotate = 0;

	 isTackle = false;

	 tackleTimer = 0;
	 tackleTimerMax = 90;
	 tackleMoveCount = 0;
	 tackleMoveTime = 0;
	 tackleMoveInterval = 120;
	 tackleMoveCountMax = 3;
}

void Enemy::ExTackle(float tackleSpeed) {
	 /*tackleMoveCount = 0;
	  rotate = 0;*/
	 // 弾の速度
	 float kTackleSpeed = tackleSpeed;

	 worldTransform_.translation_ = vector_.Lerp(
	     worldTransform_.translation_,
	     {worldTransform_.translation_.x, 5, worldTransform_.translation_.z}, 0.12f);

	  worldTransform_.rotation_.x = 0;
	 worldTransform_.rotation_.y += rotate;
	 if (tackleTimer < 900) {

		if (rotate < 2.0f) {
			rotate += 0.01f;
		} else {

			if (isTackle == false) {

				Vector3 enemyPos = GetMyWorldPosition();
				Vector3 vector = {
				    80,
				    target_->translation_.y - enemyPos.y + 5.0f,
				    target_->translation_.z - enemyPos.z};
				float bulletNorm =
				    sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

				if (bulletNorm != 0.0f) {

					move = {
					    ((vector.x / bulletNorm) * kTackleSpeed), 0,
					    (vector.z / bulletNorm) * kTackleSpeed};
				}
				tackleMoveTime++;
				if (tackleMoveTime == tackleMoveInterval) {
					tackleMoveTime = 0;

					isTackle = true;
				}
			}
			if (isTackle == true) {

				tackleTimer++;
				// if (tackleTimer == tackleTimerMax) {
				//	tackleTimer = 0;
				//	tackleMoveCount++;
				//	isTackle = false;
				// }
			}
		}
	 } else{
		isTackle = false;
		if (rotate > 0.01f) {
			rotate -= 0.01f;
		} else {
			move = {1.0f, 0, 1.0f};
			EnemyActionsCount++;
			attack_ = Attack::Normal;
		}
	  }
	    
		
	 /*worldTransform_.rotation_.y = std::atan2(move.x, move.z);
	 Vector3 velocityXZ{move.x, 0.0f, move.z};
	 float besage = vector_.Length(velocityXZ);
	 worldTransform_.rotation_.x = std::atan2(-move.y, besage);*/

	 if (isTackle) {
		if ((worldTransform_.translation_ + move).x > MoveMax) {
			move.x *= -1;
			worldTransform_.translation_.x = MoveMax;
		} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
			move.x *= -1;
			worldTransform_.translation_.x = -MoveMax;
		}

		if ((worldTransform_.translation_ + move).z > MoveMax) {
			move.z *= -1;
			worldTransform_.translation_.z = MoveMax;
		} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
			move.z *= -1;
			worldTransform_.translation_.z = -MoveMax;
		}

		worldTransform_.AddTransform(move);
	 }
}

void Enemy::ExTackle2(float tackleSpeed) {
	 /*tackleMoveCount = 0;
	  rotate = 0;*/
	 // 弾の速度
	 float kTackleSpeed = tackleSpeed;

	 worldTransform_.translation_ = vector_.Lerp(
	     worldTransform_.translation_,
	     {worldTransform_.translation_.x, 5, worldTransform_.translation_.z}, 0.12f);
	 worldTransform_.rotation_.x = 0;
	 worldTransform_.rotation_.y += rotate;
	 if (tackleTimer < 900) {

		if (rotate < 2.0f) {
			rotate += 0.01f;
		} else {

			if (isTackle == false) {

				Vector3 enemyPos = GetMyWorldPosition();
				Vector3 vector = {
				    80, target_->translation_.y - enemyPos.y + 5.0f,
				    target_->translation_.z - enemyPos.z};
				float bulletNorm =
				    sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

				if (bulletNorm != 0.0f) {

					move = {
					    ((vector.x / bulletNorm) * kTackleSpeed), 0,
					    (vector.z / bulletNorm) * kTackleSpeed};
				}
				tackleMoveTime++;
				if (tackleMoveTime == tackleMoveInterval) {
					tackleMoveTime = 0;

					isTackle = true;
				}
			}
			if (isTackle == true) {
				if (tackleTimer%shotInterval==0) {
					randFire(1.5f);
				}
				tackleTimer++;
				// if (tackleTimer == tackleTimerMax) {
				//	tackleTimer = 0;
				//	tackleMoveCount++;
				//	isTackle = false;
				// }
			}
		}
	 } else {
		isTackle = false;
		if (rotate > 0.01f) {
			rotate -= 0.01f;
		} else {
			move = {1.0f, 0, 1.0f};
			EnemyActionsCount++;
			attack_ = Attack::Normal;
		}
	 }

	 if (isTackle) {
		if ((worldTransform_.translation_ + move).x > MoveMax) {
			move.x *= -1;
			worldTransform_.translation_.x = MoveMax;
		} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
			move.x *= -1;
			worldTransform_.translation_.x = -MoveMax;
		}

		if ((worldTransform_.translation_ + move).z > MoveMax) {
			move.z *= -1;
			worldTransform_.translation_.z = MoveMax;
		} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
			move.z *= -1;
			worldTransform_.translation_.z = -MoveMax;
		}

		worldTransform_.AddTransform(move);
	 }
}

void Enemy::BehaviorFirstInitialize() { 
	move = {0.5f, 0.0f, 0.5f};
	isOpen = false;
	fireCount = 0;
	EnemyActionsCount = 0;
	enemyMoveCount = 0;
	enemyMoveInterval = 180;
}
void Enemy::BehaviorFirstUpdate() {
	 //// キャラクターの移動ベクトル
	if (attack_ == Attack::Tackle) {
		
		//Tackle(2.0f);
		Tackle2(3.5f);
	} 
	if (attack_==Attack::Normal) {
	
		worldTransformRoll_.rotation_.z = 0.0f; 

		if (move.x != 0.0f || move.z != 0.0f) {
			worldTransform_.rotation_.y =
			    vector_.LerpShortAngle(worldTransform_.rotation_.y, std::atan2(move.x, move.z),0.1f);
		}

		if (enemyMoveCount >= enemyMoveCountMax) {
			if (enemyMoveInterval > 0) {
				enemyMoveInterval--;
				kFireInterval = 60;
			} else {
				EnemyActionsCount++;
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
			if ((worldTransform_.translation_ + move).x > MoveMax) {
				move.x *= -1;
				worldTransform_.translation_.x = MoveMax;
			} else if ((worldTransform_.translation_ + move).x <= -MoveMax) {
				move.x *= -1;
				worldTransform_.translation_.x = -MoveMax;
			}

			if ((worldTransform_.translation_ + move).z > MoveMax) {
				move.z *= -1;
				worldTransform_.translation_.z = MoveMax;
			} else if ((worldTransform_.translation_ + move).z <= -MoveMax) {
				move.z *= -1;
				worldTransform_.translation_.z = -MoveMax;
			}
			worldTransform_.AddTransform(move);
		}

		if (EnemyActionsCount % 3 == 2){
			TackleInitialize();
			attack_ = Attack::Tackle;
		}

		if (EnemyLife <= 200.0f) {
			behaviorRequest_ = Behavior::kSecond;
		}
		
	}
	if (EnemyLife == 0.0f) {
		behaviorRequest_ = Behavior::kDead;
	}
}
void Enemy::BehaviorSecondInitialize() {
	EnemyActionsCount = 0;
	 move = {0.5f, 0.0f, 0.5f};
	 fireCount = 0;
	 enemyMoveCount = 0;
	 enemyMoveInterval = 800;

	 worldTransformAir.translation_ = worldTransform_.translation_ + AirOffset;

}
void Enemy::BehaviorSecondUpdate() { 
	if (attack_ == Attack::Tackle) {
		isOpen = false;
		isRotate = false;
		ExTackle(4.0f);
	} 
	if (attack_ == Attack::Normal) {
		isOpen = true;
		isRotate = true;

		if (worldTransform_.translation_.y < 75.0f) {
			worldTransform_.translation_ =
			    vector_.Lerp(worldTransform_.translation_, worldTransformAir.translation_, 0.01f);
		}

		if (enemyMoveCount >= enemyMoveCountMax&& attack_ == Attack::Normal) {
			if (enemyMoveInterval > 0) {
				enemyMoveInterval--;
				kFireInterval = 60;

				worldTransform_.translation_ =
				    vector_.Lerp(worldTransform_.translation_, movePos[moveCount], 0.03f);
				if (moveCount>4) {
					isOpen = false;
				} else {
					isOpen = true;
				}
				

				if (vector_.Length(worldTransform_.translation_ - movePos[moveCount]) <= 5.0f &&
				    moveCount < 6) {
					randFire(2.0f);
					moveCount++;
				}
				

			} else {
				if (moveCount == 4) {
					EnemyActionsCount++;
				}
				if (EnemyActionsCount % 2 == 1) {
					ExTackleInitialize();
					attack_ = Attack::Tackle;
				}
				moveCount = 0;
				enemyMoveCount = 0;
				if (enemyMoveInterval <= 0) {
					kFireInterval = 45;
					enemyMoveInterval = 800;
					
				}
			}

		} else {
			if (worldTransform_.translation_.y > 70.0f) {
				enemyMoveCount++;
				FlyAttack(4.0f);
			}

			
			for (int i = 0; i < 4; i++) {
				movePos[i].x = {(rand() % 461 - 230) / 1.0f};
				movePos[i].y = {76.0f};
				movePos[i].z = {(rand() % 461 - 230) / 1.0f};
			}
		}
		

		if (EnemyLife <= 100.0f) {
			behaviorRequest_ = Behavior::kThird;
		}
	}
	 if (EnemyLife <= 0.0f) {
		behaviorRequest_ = Behavior::kDead;
	 }
}
void Enemy::BehaviorThirdInitialize() {
	 EnemyActionsCount = 0;
	fireCount = 0;
	enemyMoveCount = 0;

	worldTransformAir.translation_ = worldTransform_.translation_ + AirOffset;

}
void Enemy::BehaviorThirdUpdate() { 
	if (attack_ == Attack::Tackle) {
		isOpen = false;
		isRotate = false;
		
		ExTackle2(5.0f);
	}
	if (attack_ == Attack::Normal) {
		isOpen = true;
		isRotate = true;
		if (worldTransform_.translation_.y < 75.0f) {
			worldTransform_.translation_ =
			    vector_.Lerp(worldTransform_.translation_, worldTransformAir.translation_, 0.02f);
		}

		if (enemyMoveCount >= enemyMoveCountMax && EnemyLife > 0.0f) {
			if (enemyMoveInterval > 0) {
				enemyMoveInterval--;
				kFireInterval = 60;
				if (moveCount>4) {
					isOpen = false;
				} else {
					isOpen = true;
				}

				worldTransform_.translation_ =
				    vector_.Lerp(worldTransform_.translation_, movePos[moveCount], 0.05f);

				if (vector_.Length(worldTransform_.translation_ - movePos[moveCount]) <= 4.0f &&
				    moveCount < 6) {
					randFire(4.5f);
					moveCount++;
				}

			} else {
				if (moveCount == 5) {
					EnemyActionsCount++;
				}
				if (EnemyActionsCount % 2 == 1) {
					TackleInitialize();
					ExTackleInitialize();
					attack_ = Attack::Tackle;
				}
				moveCount = 0;
				enemyMoveCount = 0;
				if (enemyMoveInterval <= 0) {
					kFireInterval = 45;
					enemyMoveInterval = 600;
				}
			}

		} else {
			if (worldTransform_.translation_.y > 70.0f) {
				enemyMoveCount++;
				FlyAttack(6.0f);
			}
			for (int i = 0; i < 5; i++) {
				movePos[i].x = {(rand() % 461 - 230) / 1.0f};
				movePos[i].y = {76.0f};
				movePos[i].z = {(rand() % 461 - 230) / 1.0f};
			}
		}
	}
	if (EnemyLife <= 0.0f) {
		behaviorRequest_ = Behavior::kDead;
	}
}
void Enemy::BehaviorLastInitialize() {


}
void Enemy::BehaviorLastUpdate() {


}
void Enemy::BehaviorDeadInitialize() { deathTimer = 0; }
void Enemy::BehaviorDeadUpdate() { 
	deathTimer++;
	if (deathTimer>=100&&deathTimer%10==0) {
		for (int i = 0; i < 7; i++) {
			HitEffect();
		}
	}
	if (deathTimer>=800) {
		isDead = true;
	}
}
