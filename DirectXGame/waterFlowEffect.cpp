#include "WaterFlowEffect.h"
#include "PlayerBullet.h"

// 初期化
void WaterFlowEffect::Initialize(Model* model) {
    BaseEffect::Initialize(model);
	particlePopTime_ = 0;
}
    // 更新
void WaterFlowEffect::Update() {
	static MyVector vector;
	static MyMatrix matrix;

	emitter_.worldTransform.translation_ = bulletPos_; //後で治す
	if (IsPop_) {
		// パーティクルの発生
		if (particlePopTime_ <= 0) {
			for (int i = 0; i < 8; i++) {
				float pi = 3.14f;
				float radius = (i / 4.0f) * pi;
				Vector3 speed = vector.TransformNormal(
				    {0.0f, 1.0f, 0.0f}, matrix.MakeRotateMatrixZ({0, 0, radius}));

				std::unique_ptr<Particle> particle = std::make_unique<Particle>();
				particle->activeTime = 0;
				particle->isActive = true;
				particle->speed = vector.TransformNormal(speed, matrix.MakeRotateMatrix(*bulletVelocity_)) * 0.1f;
				particle->worldTransform.Initialize();
				particle->worldTransform.translation_ = emitter_.worldTransform.translation_;
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
		particle->worldTransform.scale_.x -= 0.02f;
		particle->worldTransform.scale_.y -= 0.02f;
		particle->worldTransform.scale_.z -= 0.02f;

		if (particle->worldTransform.scale_.x <= 0.0f) {
			particleIt = particles_.erase(particleIt);
		} 
		else {
			++particleIt;
		}
	}

    BaseEffect::Update();
}

// 描画
void WaterFlowEffect::Draw(const ViewProjection& viewProjection) {
	if (isDraw_) {
		BaseEffect::Draw(viewProjection);
	}
}