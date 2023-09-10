#pragma once
#include"BaseField.h"

class Ground : public BaseField{
public:

	void Initialize(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

private:

	float size = 27.0f;

	Vector3 scale = {size, 1.0f, size};
};
