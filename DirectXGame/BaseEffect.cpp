#include "BaseEffect.h"

// 初期化
void BaseEffect::Initialize(const Model* model) {
#ifdef _DEBUG
	assert(model);
#endif
	model = model;
}

// 更新
void BaseEffect::Update() {
	for (auto particleIt = particles_.begin(); particleIt != particles_.end(); particleIt++) {
		Particle* particle = particleIt->get();
		//粒子の移動
		particle->worldTransform.translation_ += particle->speed;
		//マトリックスの更新
		particle->worldTransform.UpdateMatrix(particle->worldTransform.scale_);
	}
}

// 描画
void BaseEffect::Draw(const ViewProjection& viewProjection) {
	for (auto particleIt = particles_.begin(); particleIt != particles_.end(); particleIt++) {
		Particle* particle = particleIt->get();
		//粒子の描画
		particleModel_->Draw(particle->worldTransform, viewProjection);
	}
}