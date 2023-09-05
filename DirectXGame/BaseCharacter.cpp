#include "BaseCharacter.h"
#include<assert.h>
void BaseCharacter::Initialize(const std::vector<Model*>& models) { 
	#ifdef _DEBUG
	for (Model* model : models){
		assert(model);
	}
	#endif
	models_ = models; 
	worldTransform_.Initialize();
}

void BaseCharacter::Update() { 
	worldTransform_.TransferMatrix(); 
}

void BaseCharacter::Draw(const ViewProjection& viewProjection) {
	for (Model* model : models_) {
		model->Draw(worldTransform_, viewProjection);
	}
}
