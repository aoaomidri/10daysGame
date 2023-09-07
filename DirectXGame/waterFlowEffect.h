#pragma once
#include "BaseEffect.h"

class WaterFlowEffect : public BaseEffect {

	// ‰Šú‰»
	void Initialize(const Model* model) override;

	// XV
	void Update() override;

	// •`‰æ
	void Draw(const ViewProjection& viewProjection) override;
};
