#pragma once

#include <irrlicht.h>
#include <vector>
#include "Helper.h"
#include "Player.h"
#include "Enemy.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define BASE_BOSS_HEALTH 15
#define BASE_BOSS_HEIGHT -2.0f

class BigEnemy
{
public:
	BigEnemy(){}
	BigEnemy(IrrlichtDevice* d, const float distAway);
	const vector3df& GetPosition() { return node->getPosition(); }
	void LookAt(const vector3df p, const float offset);
	void Reset();
	void SetAttackAttrib(const float dist, const float length) { attackDistance = dist; attackLength = length; }
	void Attack(const float dt);
	int GetHealth() { return health; }
	void RemoveHealth(int damage, const float dt);
	void SetHealth(int n) { health = n; }
	void AddHealth(int n) { health += n; }
	bool isAttackingFlag() { return isAttacking; }
	vector3df RandomPosition(const float distAway, bool setPos);
	int GetAttackDamage() { return attackDamage; }
	void SetAttackDamage(int a) { attackDamage = a; }
	bool GetAttackStrikeDone() { return attackStrikeDone; }
	ISceneNode* GetNode() { return node; }
	void SetAttackStrikeDone(int a) { attackStrikeDone = a; }
	bool MoveTowards(const vector3df p, const float dt, bool includeCap);
	vector3df GetCachedSpawnPosition() { return cachedSpawnPosition; }
	bool PollNewPosition(const float dt);
	IAnimatedMeshSceneNode* GetNodeDirt() { return node_dirt; }
	IAnimatedMeshSceneNode* GetNodeDrill() { return node_drill; }
	IAnimatedMeshSceneNode* GetNodeCap() { return node_cap; }
	~BigEnemy();
private:
	float newPositionTimer{ 0.0f };
	float newPositionRate{ 4.0f };
	vector3df cachedSpawnPosition;
	float healthDepleteTimer{ 0.0f };
	float healthDepleteRate{ 0.1f };
	int health{ BASE_BOSS_HEALTH };
	bool attackStrikeDone{ false };
	int attackDamage{ 13 };
	bool attackOnce{ false };
	bool isAttacking{ false };
	float attackDistance{ 0.0f };
	float attackLength{ 0.0f };
	float currAttackLength{ 0.0f };
	float speed{ 3.0f };
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* node;
	IAnimatedMeshSceneNode* node_cap;
	IAnimatedMeshSceneNode* node_drill;
	IAnimatedMeshSceneNode* node_dirt;
};