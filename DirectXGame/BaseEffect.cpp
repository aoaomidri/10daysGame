#include "BaseEffect.h"

// ������
void BaseEffect::Initialize(const Model* model) {
#ifdef _DEBUG
	assert(model);
#endif
	model = model;
}

// �X�V
void BaseEffect::Update() {
	for (auto particleIt = particles_.begin(); particleIt != particles_.end(); particleIt++) {
		Particle* particle = particleIt->get();
		//���q�̈ړ�
		particle->worldTransform.translation_ += particle->speed;
		//�}�g���b�N�X�̍X�V
		particle->worldTransform.UpdateMatrix(particle->worldTransform.scale_);
	}
}

// �`��
void BaseEffect::Draw(const ViewProjection& viewProjection) {
	for (auto particleIt = particles_.begin(); particleIt != particles_.end(); particleIt++) {
		Particle* particle = particleIt->get();
		//���q�̕`��
		particleModel_->Draw(particle->worldTransform, viewProjection);
	}
}