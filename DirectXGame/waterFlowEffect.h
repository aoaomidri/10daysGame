#pragma once
#include "BaseEffect.h"
#include "MyVector.h"
#include "MyMatrix.h"

class WaterFlowEffect : public BaseEffect {
public:
	// 初期化
	void Initialize(Model* model) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(const ViewProjection& viewProjection) override;

public: //ゲッターセッター

	inline void SetEmitterParent(const WorldTransform* parent) {emitter_.worldTransform.parent_ = parent;}
	inline void const SetBulletVelocity(const Vector3* velocity) { bulletVelocity_ = velocity; }

private:
	int particlePopTime_;
	const int particlePopCoolTime = 15;
	const Vector3* bulletVelocity_;
};
