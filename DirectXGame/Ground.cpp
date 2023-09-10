#include "Ground.h"

void Ground::Initialize(const std::vector<Model*>& models) { 
	BaseField::Initialize(models); 
	worldTransform_.translation_.y = -50.0f;
}

void Ground::Update() { 
	worldTransform_.UpdateMatrix(scale);
}

void Ground::Draw(const ViewProjection& viewProjection) { 
	BaseField::Draw(viewProjection); 
}