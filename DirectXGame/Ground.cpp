#include "Ground.h"
#include<imgui.h>

void Ground::Initialize(const std::vector<Model*>& models) { 
	BaseField::Initialize(models); 
	worldTransform_.translation_.y = -50.0f;
}

void Ground::Update() { 
	#ifdef _DEBUG
	ImGui::Begin("Ground");
	ImGui::DragFloat3("Translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::End();
#endif
	worldTransform_.UpdateMatrix(scale);
}

void Ground::Draw(const ViewProjection& viewProjection) { 
	BaseField::Draw(viewProjection); 
}