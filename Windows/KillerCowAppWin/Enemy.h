#pragma once

#include <irrlicht.h>
#include <vector>
#include "Helper.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

enum ENEMY_STATE
{
	NONE,
	RESET,
	ATTACK
};

class Enemy
{
public:
	Enemy(IrrlichtDevice* d, const float distAway);
	const vector3df& GetPosition() { return node->getPosition(); }
	void LookAt(const vector3df p, const float offset);
	void Reset();
	void SetAttackAttrib(const float dist, const float length) { attackDistance = dist; attackLength = length; }
	void Attack(const float dt);
	void RandomPosition(const float distAway);
	ENEMY_STATE MoveTowards(const vector3df p, const float speed);
	~Enemy();
private:
	bool attackOnce{ false };
	bool isAttacking{ false };
	float attackDistance{ 0.0f };
	float attackLength{ 0.0f };
	float currAttackLength{ 0.0f };
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* node;
};

class EnemyFactory
{
	public:
		EnemyFactory(IrrlichtDevice* d, const int size);
		void Update(const vector3df t, const float dt);
		~EnemyFactory();
	private:
		std::vector<Enemy> enemies;
};