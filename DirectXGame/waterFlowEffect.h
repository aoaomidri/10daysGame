#pragma once
#include "BaseEffect.h"

class WaterFlowEffect : public BaseEffect {

	// 初期化
	void Initialize(const Model* model) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(const ViewProjection& viewProjection) override;
};
