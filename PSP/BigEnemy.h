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
#include "Enemy.h"

#define BASE_BOSS_HEALTH 55
#define BASE_BOSS_HEIGHT -2.0f

#define BIG_BOSS_ANIM_IDLE 0
#define BIG_BOSS_ANIM_ATTACK 1
#define BIG_BOSS_ANIM_DEATH 2
#define BIG_BOSS_ANIM_DEATH_END 3
#define BIG_BOSS_ANIM_DEATH_END2 4
#define BIG_BOSS_ANIM_DEATH_END3 5
#define BIG_BOSS_ANIM_WALK 6

//static FMOD::Sound* bigMooEffect;
//static FMOD::ChannelGroup* channelGroupBigMoo;

static bool firstDeath = true;

class BigEnemy
{
public:
	BigEnemy()
	{
		animationTimer = 0.0f;
		animationRate = 4.0f;
		newPositionTimer = 0.0f;
		newPositionRate = 4.0f;
		healthDepleteTimer = 0.0f;
		healthDepleteRate = 0.1f;
		health = BASE_BOSS_HEALTH;
		attackStrikeDone = false;
		animationID = BIG_BOSS_ANIM_IDLE;
		attackDamage = 6;
		attackOnce = false;
		isAttacking = false;
		attackDistance = 0.0f;
		attackLength = 0.0f;
		currAttackLength = 0.0f;
		speed = 3.7f;
	}
	BigEnemy(engineDevice* d, void* FMODSystem, const float distAway);
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
	vector3df RandomPosition(const float distAway, bool setPos);
	int GetAttackDamage() { return attackDamage; }
	void SetAttackDamage(int a) { attackDamage = a; }
	bool GetAttackStrikeDone() { return attackStrikeDone; }
	ISceneNode* GetNode() { return node; }
	void SetAttackStrikeDone(int a) { attackStrikeDone = a; }
	bool MoveTowards(const vector3df p, const float dt, bool includeCap);
	vector3df GetCachedSpawnPosition() { return cachedSpawnPosition; }
	bool PollNewPosition(const float dt);
	void SetAnimationID(int i) { animationID = i; }
	int GetAnimationID() { return animationID; }
	void SetAnimationName(const char* n) { node->setMD2Animation(n); }
	void DeathAnimation(const float dt, void* FMODSystem);
	IAnimatedMeshSceneNode* GetNodeDirt() { return node_dirt; }
	IAnimatedMeshSceneNode* GetNodeDrill() { return node_drill; }
	IAnimatedMeshSceneNode* GetNodeCap() { return node_cap; }
	~BigEnemy();
private:
	//FMOD::Channel* channel;
	float animationTimer;
	float animationRate;
	float newPositionTimer;
	float newPositionRate;
	float healthDepleteTimer;
	float healthDepleteRate;
	int health;
	bool attackStrikeDone;
	int animationID;
	int attackDamage;
	bool attackOnce;
	bool isAttacking;
	float attackDistance;
	float attackLength;
	float currAttackLength;
	float speed;

	vector3df cachedSpawnPosition;
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* node;
	IAnimatedMeshSceneNode* node_cap;
	IAnimatedMeshSceneNode* node_drill;
	IAnimatedMeshSceneNode* node_dirt;
};