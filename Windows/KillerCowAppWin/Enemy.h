#pragma once

#include <irrlicht.h>
#include <vector>
#include "Helper.h"
#include "Player.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define BASE_COW_HEALTH 15

enum ENEMY_STATE
{
	NONE,
	RESET,
	ATTACK
};

class Enemy
{
public:
	Enemy(){}
	Enemy(IrrlichtDevice* d, const float distAway);
	const vector3df& GetPosition() { return node->getPosition(); }
	void LookAt(const vector3df p, const float offset);
	void Reset();
	void SetAttackAttrib(const float dist, const float length) { attackDistance = dist; attackLength = length; }
	void Attack(const float dt);
	int GetHealth() { return health; }
	void RemoveHealth(const float dt);
	void SetHealth(int n) { health = n; }
	void AddHealth(int n) { health += n; }
	bool isAttackingFlag() { return isAttacking; }
	void RandomPosition(const float distAway);
	int GetAttackDamage() { return attackDamage; }
	void SetAttackDamage(int a) { attackDamage = a; }
	bool GetAttackStrikeDone() { return attackStrikeDone; }
	ISceneNode* GetNode() { return node; }
	void SetAttackStrikeDone(int a) { attackStrikeDone = a; }
	ENEMY_STATE MoveTowards(const vector3df p, const float dt);
	~Enemy();
private:
	float healthDepleteTimer{ 0.0f };
	float healthDepleteRate{ 0.1f };
	int health{ BASE_COW_HEALTH };
	bool attackStrikeDone{ false };
	int attackDamage{ 13 };
	bool attackOnce{ false };
	bool isAttacking{ false };
	float attackDistance{ 0.0f };
	float attackLength{ 0.0f };
	float currAttackLength{ 0.0f };
	float speed{ 2.0f };
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* node;
};

class EnemyFactory
{
	friend class Enemy;
	public:
		EnemyFactory() {}
		EnemyFactory(IrrlichtDevice* d, const int size);
		void Update(Player& p, const float dt);
		Enemy* FindEnemy(ISceneNode* s);
		void ResetEmission() { for (auto& x : enemies) { x.GetNode()->getMaterial(0).EmissiveColor = SColor(255, 0, 0, 0); } }
		void ForceReset();
		Enemy* GetNearestEnemy(Player& p);
		bool isPlayerGettingMunched() { return playerGettingMunched; }
		void SetPlayerGettingMunched(bool p) { playerGettingMunched = p; }
		~EnemyFactory();
	private:
		//how many cows are munching us
		bool playerGettingMunched{ false };
		std::vector<Enemy> enemies;
};