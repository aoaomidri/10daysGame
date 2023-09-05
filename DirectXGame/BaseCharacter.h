#pragma once
#include<Model.h>
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"MyMatrix.h"
#include"MyVector.h"

// OBB
struct OBB {
	Vector3 center;          // 中心点
	Vector3 orientations[3]; // 座標軸。正規化・直行必須
	Vector3 size;            // 座標軸の長さの半分。中心から面までの距離
};
class BaseCharacter {
protected:
	//モデルデータ配列
	std::vector<Model*> models_;
	//ワールド変換データ
	WorldTransform worldTransform_;

public:
	//初期化
	virtual void Initialize(const std::vector<Model*>& models);

	//更新
	virtual void Update();

	//描画
	virtual void Draw(const ViewProjection& viewProjection);

	//ワールド変換データを取得
	const WorldTransform& GetWorldTransform() { return worldTransform_;}

	
};
