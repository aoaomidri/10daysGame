#pragma once
#include "BaseEffect.h"
#include "Matrix4x4.h"

class ConcentrationLine : public BaseEffect {
public:
	// 初期化
	void Initialize(Model* model) override;

	// 更新
	void Update() override;

	// 描画
	void Draw(const ViewProjection& viewProjection) override;

public: // ゲッターセッター
	inline void SetEmitterParent(const WorldTransform* parent) {
		emitter_.worldTransform.parent_ = parent;
	}
	inline void const SetPlayerVelocity(const Vector3* velocity) { playerVelocity_ = velocity; }
	inline void const SetIsPop(bool isPop) { IsPop_ = isPop; }
	inline void const SetPlayerWorldTransform(WorldTransform* playerWorldTransform) { playerWorldTransform_ = playerWorldTransform; }

private:
	int particlePopTime_;
	bool IsPop_;
	const int particlePopCoolTime = 10;
	const Vector3* playerVelocity_;
	const WorldTransform* playerWorldTransform_;
};
