#pragma once
#include "BaseEffect.h"
#include "Sprite.h"

class SceneTransition : public BaseEffect {
public:
	// 初期化
	void Initialize(uint32_t textureHandle) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(const ViewProjection& viewProjection) override;

private:
	std::list<std::unique_ptr<Sprite>> sprites_;
};
