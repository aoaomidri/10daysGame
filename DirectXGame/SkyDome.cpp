#include "SkyDome.h"

void SkyDome::Initialize(const std::vector<Model*>& models) { 
	BaseField::Initialize(models);
}

void SkyDome::Update() { worldTransform_.UpdateMatrix(scale); }

void SkyDome::Draw(const ViewProjection& viewProjection) { 
	BaseField::Draw(viewProjection);
}