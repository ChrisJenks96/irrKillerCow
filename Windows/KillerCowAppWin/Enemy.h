#pragma once

#include <irrlicht.h>
#include <vector>
#include "Helper.h"
#include "Player.h"
#include "LightningSceneNode.h"

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

#define ENEMY_ANIMATION_IDLE 0
#define ENEMY_ANIMATION_WALK 1
#define ENEMY_ANIMATION_DEATH 2
#define ENEMY_ANIMATION_DEATH_IDLE 3
#define ENEMY_ANIMATION_ATTACK 4

//enemy id ranges from 300+
static int enemyID = 300;

class Enemy
{
public:
	Enemy() {}
	Enemy(IrrlichtDevice* d, const float distAway);
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
	void SetDeathAnimationTrigger(bool x) { deathAnimationTrigger = x; }
	bool isDeathAnimationTrigger() { return deathAnimationTrigger; }
	void RandomPosition(const float distAway);
	int GetAttackDamage() { return attackDamage; }
	void SetAttackDamage(int a) { attackDamage = a; }
	bool GetAttackStrikeDone() { return attackStrikeDone; }
	ISceneNode* GetNode() { return node; }
	void SetAttackStrikeDone(int a) { attackStrikeDone = a; }
	void SetAnimationName(const char* name) { node->setMD2Animation(name); }
	void SetAnimationID(int i) { animationID = i; }
	int GetAnimationID() { return animationID; }
	bool DeathAnimation(const float dt);
	ENEMY_STATE MoveTowards(const vector3df p, const float dt);
	void SetSpeed(float s) { speed = s; }
	float GetSpeed() { return speed; }
	float GetCurrentAttackLength() { return currAttackLength; }
	~Enemy();
private:
	int animationID{ ENEMY_ANIMATION_IDLE };
	float deathAnimationTimer{ 0.0f };
	bool deathAnimationTrigger{ false };
	float healthDepleteTimer{ 0.0f };
	float healthDepleteRate{ 0.1f };
	int health{ BASE_COW_HEALTH };
	bool attackStrikeDone{ false };
	int attackDamage{ 12 };
	bool attackOnce{ false };
	bool isAttacking{ false };
	float attackDistance{ 0.0f };
	float attackLength{ 0.0f };
	float currAttackLength{ 0.0f };
	float speed{ 2.1f };
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* node;
};

class EnemyFactory
{
	friend class Enemy;
	public:
		EnemyFactory() {}
		EnemyFactory(IrrlichtDevice* d, const int size, const int usable);
		void Update(Player& p, bool& shieldActive, int& cowsKilled, const float dt);
		Enemy* FindEnemy(ISceneNode* s);
		void ResetEmission() { for (auto& x : enemies) { x.GetNode()->getMaterial(0).EmissiveColor = SColor(255, 0, 0, 0); } }
		void AddSpeed(float s) { for (auto& x : enemies) { x.SetSpeed(x.GetSpeed() + s); } }
		void ForceReset();
		void SetHealthAll(int health) { for (auto& x : enemies) { x.SetHealth(health); } }
		void ForceDeath(float& xpMod, float& cowsXp, int& cowsKilled);
		void SetVisible(bool s) {for (auto& x : enemies) { x.GetNode()->setVisible(s); }}
		Enemy* GetNearestEnemy(Player& p);
		bool isPlayerGettingMunched() { return playerGettingMunched; }
		void SetPlayerGettingMunched(bool p) { playerGettingMunched = p; }
		void SetEnemyCount(int c);
		int GetEnemyCount() { return usable; }
		~EnemyFactory();
	private:
		//how many cows are munching us
		bool playerGettingMunched{ false };
		int usable;
		std::vector<Enemy> enemies;
};