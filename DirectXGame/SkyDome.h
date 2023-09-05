#pragma once
#include"BaseField.h"

class SkyDome :public BaseField{
public:

	void Initialize(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

private:

	float size = 500.0f;

	Vector3 scale = {size, size, size};
};
