#pragma once
#include"BaseField.h"
class Rock : public BaseField {
public:
	void Initialize(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

private:
	float size = 10.0f;

	Vector3 scale = {size, size, size};

	WorldTransform worldTransformRock_[3];

};
