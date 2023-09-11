#include "SceneTransition.h"
#include "GameScene.h"

// 初期化
void SceneTransition::Initialize(uint32_t textureHandle) {
	BaseEffect::Initialize(textureHandle);

	emitter_.size = {50, 720};
	emitter_.worldTransform.translation_ = {1350, 360, 0};
	isDraw_ = true;
	transitionTime_ = 0;
	startTransition_ = false;
	completeTransition_ = false;
	IsPop_ = true;
}

// 更新
void SceneTransition::Update() {
	
	if (startTransition_) {
		if (transitionTime_ >= kTransitionTime_) {
			IsPop_ = false;
			startTransition_ = false;
			completeTransition_ = true;
		} else {
			transitionTime_++;
		}

		if (particlePopTime_ <= 0) {
			for (int i = 0; i < 10; i++) {
				Vector2 popPos;
				popPos.x = (rand() % (int)emitter_.size.x) - (emitter_.size.x / 2) +
				           emitter_.worldTransform.translation_.x;
				popPos.y = (rand() % (int)emitter_.size.y) - (emitter_.size.y / 2) +
				           emitter_.worldTransform.translation_.y;

				std::unique_ptr<SpriteParticle> spriteParticle = std::make_unique<SpriteParticle>();
				spriteParticle->sprite =
				    Sprite::Create(textureHandle_, {0, 0}, {1, 1, 1, 1}, {0.5f, 0.5f});
				spriteParticle->sprite->SetPosition(popPos);
				spriteParticle->speed.x = static_cast<float>(-1 * (rand() % 10 + 10));
				spriteParticle->speed.y = 0;
				spriteParticle->activeTime = 0;
				spriteParticle->isActive = true;
				spriteParticles_.push_back(std::move(spriteParticle));
			}
			particlePopTime_ = particlePopCoolTime;
		} else {
			particlePopTime_--;
		}
	}

	for (auto spriteParticlesIt = spriteParticles_.begin(); spriteParticlesIt != spriteParticles_.end();) {
		SpriteParticle* particle = spriteParticlesIt->get();
		particle->activeTime++;
		particle->sprite->SetPosition({particle->sprite->GetPosition().x + particle->speed.x, particle->sprite->GetPosition().y + particle->speed.y});
		


		if (particle->sprite->GetPosition().x <= -1500) {
			particle->isActive = false;
		}
		if (!particle->isActive) {
			spriteParticlesIt = spriteParticles_.erase(spriteParticlesIt);
		} else {
			++spriteParticlesIt;
		}
	}

	BaseEffect::Update();
}

// 描画
void SceneTransition::Draw() {
	BaseEffect::Draw();
}