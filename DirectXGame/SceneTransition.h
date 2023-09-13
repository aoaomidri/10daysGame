#pragma once
#include "BaseEffect.h"
#include "Sprite.h"

class GameScene;

class SceneTransition : public BaseEffect {
public:
	// 初期化
	void Initialize(uint32_t textureHandle) override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;


public: //ゲッターセッター

	inline void SetStartTransition(bool startTransition) { startTransition_ = startTransition; }
	inline bool GetStartTransition() { return startTransition_; }
	inline void SetCompleteTransition(bool completeTransition) { completeTransition_ = completeTransition; }
	inline bool GetCompleteTransition() { return completeTransition_; }
	inline bool GetNonFish() { return nonFish_; }


private:
	int particlePopTime_;
	const int particlePopCoolTime = 1;
	bool IsPop_;
	int transitionTime_;
	const int kTransitionTime_ = 120;
	bool startTransition_;
	bool completeTransition_;
	bool nonFish_;
};
