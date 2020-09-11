#pragma once

#include <engine.h>
using namespace engine;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#include <vector>
#include "Helper.h"
#include "Player.h"
#include "LightningSceneNode.h"

//#include <fmod.hpp>

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

/*static FMOD::Sound* cowMooEffect;
static FMOD::Sound* cowMooEffect1;
static FMOD::Sound* cowMooEffect2;
static FMOD::Sound* cowMooEffect3;
static FMOD::ChannelGroup* channelGroupMoo;*/

class Enemy
{
public:
	Enemy() 
	{
		animationID = ENEMY_ANIMATION_IDLE;
		deathAnimationTimer= 0.0f;
		deathAnimationTrigger= false;
		healthDepleteTimer= 0.0f;
		healthDepleteRate= 0.1f;
		health= BASE_COW_HEALTH;
		attackStrikeDone= false;
		attackDamage=23;
		attackOnce=false;
		isAttacking= false;
		attackDistance= 0.0f;
		attackLength= 0.0f;
		currAttackLength= 0.0f;
		speed = 2.1f;
	}

	Enemy(engineDevice* d, const float distAway);
	const vector3df& GetPosition() { return node->getPosition(); }
	void LookAt(vector3df p, const float offset);
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
	bool DeathAnimation(void* FMODSystem, const float dt);
	enum ENEMY_STATE MoveTowards(const vector3df p, const float dt);
	void SetSpeed(float s) { speed = s; }
	float GetSpeed() { return speed; }
	float GetCurrentAttackLength() { return currAttackLength; }
	~Enemy();
private:
	//FMOD::Channel* channel;
	int animationID;
	float deathAnimationTimer;
	bool deathAnimationTrigger;
	float healthDepleteTimer;
	float healthDepleteRate;
	int health;
	bool attackStrikeDone;
	int attackDamage;
	bool attackOnce;
	bool isAttacking;
	float attackDistance;
	float attackLength;
	float currAttackLength;
	float speed;
	int soundEffectID;
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* node;
};

class EnemyFactory
{
	friend class Enemy;
	public:
		EnemyFactory() { playerGettingMunched = false; }
		EnemyFactory(engineDevice* d, void* FMODSystem, const int size, const int usable);
		void Update(Player& p, void* FMODSystem, bool& shieldActive, int& cowsKilled, const float dt);
		Enemy* FindEnemy(ISceneNode* s);
		void ResetEmission() { for (int i = 0; i < enemies.size(); i++) { enemies[i].GetNode()->getMaterial(0).EmissiveColor = SColor(255, 0, 0, 0); } }
		void AddSpeed(float s) { for (int i = 0; i < enemies.size(); i++) { enemies[i].SetSpeed(enemies[i].GetSpeed() + s); } }
		void ForceReset();
		void SetHealthAll(int health) { for (int i = 0; i < enemies.size(); i++) { enemies[i].SetHealth(health); } }
		void ForceDeath(float& xpMod, float& cowsXp, int& cowsKilled);
		void SetVisible(bool s) { for (int i = 0; i < enemies.size(); i++) { enemies[i].GetNode()->setVisible(s); }}
		Enemy* GetNearestEnemy(Player& p);
		bool isPlayerGettingMunched() { return playerGettingMunched; }
		void SetPlayerGettingMunched(bool p) { playerGettingMunched = p; }
		void SetEnemyCount(int c);
		int GetEnemyCount() { return usable; }
		~EnemyFactory();
	private:
		//how many cows are munching us
		bool playerGettingMunched;
		int usable;
		std::vector<Enemy> enemies;
};