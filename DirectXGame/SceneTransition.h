#pragma once
#include "BaseEffect.h"

class SceneTransition : public BaseEffect {
public:
	// 初期化
	void Initialize(Model* model) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(const ViewProjection& viewProjection) override;

private:
	
};
