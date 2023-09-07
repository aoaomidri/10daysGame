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

	// ������
	virtual void Initialize(const Model* model);

	// �X�V
	virtual void Update();

	// �`��
	virtual void Draw(const ViewProjection& viewProjection);

public: //�Q�b�^�[�Z�b�^�[
	inline void SetEmitterPos(Vector3 pos) { emmiter_.pos = pos; }
	inline void SetEmitterSize(Vector3 size) { emmiter_.size = size; }

protected:
	
	//���f��
	Model* particleModel_;
	//�G�~�b�^�[
	Emitter emmiter_;
	//�p�[�e�B�N��
	std::list<std::unique_ptr<Particle>> particles_;

};
