#include "SceneTransition.h"

// 初期化
void SceneTransition::Initialize(uint32_t textureHandle) {
	BaseEffect::Initialize(textureHandle);

	emitter_.size = {50, 720};
	isDraw_ = true;
}

// 更新
void SceneTransition::Update() {
	

}

// 描画
void SceneTransition::Draw(const ViewProjection& viewProjection) {

}