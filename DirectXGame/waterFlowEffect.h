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
	inline void const SetIsPop(bool isPop) { IsPop_ = isPop; }
	inline void const SetBulletPos(Vector3 bulletPos) { bulletPos_ = bulletPos; }

private:
	int particlePopTime_;
	bool IsPop_;
	const int particlePopCoolTime = 15;
	const Vector3* bulletVelocity_;

	Vector3 bulletPos_;
};
