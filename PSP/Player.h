#pragma once

#include <engine.h>
using namespace engine;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#include "Helper.h"
#include "EnemyOrb.h"

#define ANIMATION_FRAME_TO_TIME(x) x / 10.0f

#define PLAYER_ANIMATION_IDLE 0
#define PLAYER_ANIMATION_ATTACK_START 1
#define PLAYER_ANIMATION_ATTACK_MAIN 2
#define PLAYER_ANIMATION_ATTACK_END 3
#define PLAYER_ANIMATION_CRAWL_FROM_SHIP 4
#define PLAYER_ANIMATION_CRAWL_WALK 5
#define PLAYER_ANIMATION_DEATH 6
#define PLAYER_ANIMATION_DEATH_END 7

class Player
{
	public:
		Player() 
		{
			animationTimer= 0.0f;
			nodeShieldY= 0.0f;
			animationID= PLAYER_ANIMATION_IDLE;
			oldAnimationID= 0;
			energyDepleteTimer= 0.0f;
			energyDepleteRate= 0.1f;
			energyRestoreTimer= 0.0f;
			energyRestoreRate= 0.2f;
			energy = 100;
			health= 100;
		}
		Player(engineDevice* d);
		const vector3df& GetPosition() { return node->getPosition(); }
		void LookAt(vector3df p, const float offset);
		ISceneNode* Fire(engineDevice* device, const float length);
		void WeaponFiringLightToggle(bool t) { /*weaponFiringLight->setVisible(t);*/ }
		void ResetEmission() { node->getMaterial(0).EmissiveColor = SColor(255, 0, 0, 0); }
		void Idle();
		int GetEnergy() { return energy; }
		void RemoveEnergy(const float dt);
		void SetEnergy(int n) { energy = n; }
		void AddEnergy(const float dt);
		int GetHealth() { return health; }
		void RemoveHealth(int n) { health -= n; }
		void SetHealth(int n) { health = n; }
		void AddHealth(int n) { health += n; }
		void SetAnimationName(const char* name) { node->setMD2Animation(name); }
		int UnlockGUIValueUpdate(float cowsXP) { return (int)((float)HEALTH_GUI_SIZE_X * ((float)cowsXP / 100.0f)); }
		int EnergyGUIValueUpdate() { return (int)((float)HEALTH_GUI_SIZE_X * ((float)energy / 100.0f)); }
		int HealthGUIValueUpdate() {return (int)((float)HEALTH_GUI_SIZE_X * ((float)health / 100.0f));}
		ISceneNode* GetNode() { return node; }
		float GetEnergyDepleteRate() { return energyDepleteRate; }
		void SetEnergyDepleteRate(float s) { energyDepleteRate = s; }
		float GetEnergyRestoreRate() { return energyRestoreRate; }
		void SetEnergyRestoreRate(float s) { energyRestoreRate = s; }
		void LightningChangeCol(SColorf c){ weaponFiringLight->getLightData().DiffuseColor = c; }
		void SetAnimationID(int i) { animationID = i; }
		bool MoveTowards(const vector3df p, const float dt);
		void ShieldUVScroll(const float dt);
		void ShieldToggle(bool t) {nodeShield->setVisible(t);}
		void ShieldTexture(stringc st, IVideoDriver* driver) { nodeShield->setMaterialTexture(0, driver->getTexture(st.c_str()));  }
		ISceneNode* GetOrb() { return orb; }

		//animations
		void FiringAnimation(const float dt);
		void DeathAnimation(const float dt);
		void NotFiringAnimation(const float dt);

		~Player();
	private:
		float animationTimer;
		float nodeShieldY;
		int animationID;
		int oldAnimationID;
		float energyDepleteTimer;
		float energyDepleteRate;
		float energyRestoreTimer;
		float energyRestoreRate;
		int energy;
;		int health;
		ILightSceneNode* weaponFiringLight;
		IAnimatedMesh* mesh;
		ISceneNode* nodeShield;
		IAnimatedMeshSceneNode* node;
		ISceneNode* orb;
};