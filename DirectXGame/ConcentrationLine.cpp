#include "ConcentrationLine.h"
#include "MyVector.h"
#include "MyMatrix.h"

// 初期化
void ConcentrationLine::Initialize(Model* model) {
	BaseEffect::Initialize(model);
	particlePopTime_ = 0;
	isDraw_ = true;
}

// 更新
void ConcentrationLine::Update() {
	static MyVector vector;
	static MyMatrix matrix;
	Vector3 playerVelocity = *playerVelocity_;
	emitter_.worldTransform.translation_ = playerWorldTransform_->translation_; // 後で治す


	if (IsPop_ && (playerVelocity.x != 0.0f || playerVelocity.z != 0.0f)) {
		// パーティクルの発生
		if (particlePopTime_ <= 0) {
			Matrix4x4 playerRotate = matrix.MakeRotateMatrix(playerWorldTransform_->rotation_);
			for (int i = 0; i < 8; i++) {
				//パーティクルの沸き場所
				Vector3 particlePos;
				particlePos.x = static_cast<float>(rand() % 5 - 2);
				particlePos.y = static_cast<float>(rand() % 5 - 2);
				particlePos.z = 5;
				particlePos = vector.TransformNormal(particlePos, playerRotate);

				std::unique_ptr<Particle> particle = std::make_unique<Particle>();
				particle->activeTime = 0;
				particle->isActive = true;
				particle->speed = playerVelocity * -1 * (float)(rand() % 3 + 1);
				particle->worldTransform.Initialize();
				particle->worldTransform.translation_ = emitter_.worldTransform.translation_ + particlePos;
				particle->worldTransform.translation_.y += 4.0f;
				particle->worldTransform.scale_.x = 0.25f;
				particle->worldTransform.scale_.y = 0.25f;
				particle->worldTransform.scale_.z = 2;
				particle->worldTransform.rotation_.y = std::atan2(playerVelocity.x, playerVelocity.z);
				particles_.push_back(std::move(particle));
			}

			particlePopTime_ = particlePopCoolTime;
		} else {
			particlePopTime_--;
		}
	}

	for (auto particleIt = particles_.begin(); particleIt != particles_.end();) {
		Particle* particle = particleIt->get();
		particle->activeTime++;

		if (particle->activeTime >= 15) {
			particleIt = particles_.erase(particleIt);
		} else {
			++particleIt;
		}
	}

	BaseEffect::Update();
}

// 描画
void ConcentrationLine::Draw(const ViewProjection& viewProjection) {
	if (isDraw_) {
		BaseEffect::Draw(viewProjection);
	}
}