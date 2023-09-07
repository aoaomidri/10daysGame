#pragma once
#include <list>
#include <iostream>
#include <assert.h>
#include "WorldTransform.h"
#include "Model.h"
#include "Vector2.h"
#include "Vector3.h"

struct Emitter {
	Vector3 pos;
	Vector3 size;
};

struct Particle {
	WorldTransform worldTransform;
	Vector3 speed;
	int activeTime;
	bool isActive;
};

class BaseEffect {
public:

	// 初期化
	virtual void Initialize(const Model* model);

	// 更新
	virtual void Update();

	// 描画
	virtual void Draw(const ViewProjection& viewProjection);

public: //ゲッターセッター
	inline void SetEmitterPos(Vector3 pos) { emmiter_.pos = pos; }
	inline void SetEmitterSize(Vector3 size) { emmiter_.size = size; }

protected:
	
	//モデル
	Model* particleModel_;
	//エミッター
	Emitter emmiter_;
	//パーティクル
	std::list<std::unique_ptr<Particle>> particles_;

};
