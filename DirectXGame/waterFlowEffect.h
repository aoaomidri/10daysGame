#pragma once
#include "BaseEffect.h"

class WaterFlowEffect : public BaseEffect {

	// ������
	void Initialize(const Model* model) override;

	// �X�V
	void Update() override;

	// �`��
	void Draw(const ViewProjection& viewProjection) override;
};
