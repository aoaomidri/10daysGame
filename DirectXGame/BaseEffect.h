#pragma once
#include <list>
#include <iostream>
#include <assert.h>
#include "WorldTransform.h"
#include "Model.h"
#include "Sprite.h"
#include "Vector2.h"
#include "Vector3.h"

struct Emitter {
	WorldTransform worldTransform;
	Vector3 size;
};

struct Particle {
	WorldTransform worldTransform;
	Vector3 speed;
	int activeTime;
	bool isActive;
};

struct SpriteParticle {
	Sprite* sprite;
	Vector2 speed;
	int activeTime;
	bool isActive;
};

class BaseEffect {
public:

	// 初期化
	virtual void Initialize(Model* model);
	virtual void Initialize(uint32_t textureHandle);
	virtual void Initialize(Model* model, uint32_t textureHandle);

	// 更新
	virtual void Update();

	// 描画
	virtual void Draw(const ViewProjection& viewProjection);
	virtual void Draw();

public: //ゲッターセッター
	inline void SetEmitterPos(Vector3 pos) { emitter_.worldTransform.translation_ = pos; }
	inline void SetEmitterSize(Vector3 size) { emitter_.size = size; }

	inline bool GetIsDraw() { return isDraw_; }
	inline void SetIsDraw(bool isDraw) { isDraw_ = isDraw; }

	inline bool GetIsSpriteDraw() { return isDraw_; }
	inline void SetIsSpriteDraw(bool isDraw) { isDraw_ = isDraw; }

protected:
	
	//モデル
	Model* particleModel_;
	//テクスチャハンドル
	uint32_t textureHandle_;
	//エミッター
	Emitter emitter_;
	//パーティクル
	std::list<std::unique_ptr<Particle>> particles_;
	std::list<std::unique_ptr<SpriteParticle>> spriteParticles_;
	//パーティクルを描画するか
	bool isDraw_;
};
