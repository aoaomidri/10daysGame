#pragma once
#include<Model.h>
#include"WorldTransform.h"
class BaseField {
protected:
	WorldTransform worldTransform_;

	std::vector<Model*> models_;

public:

	// 初期化
	virtual void Initialize(const std::vector<Model*>& models);

	// 更新
	virtual void Update();

	// 描画
	virtual void Draw(const ViewProjection& viewProjection);
	
};
