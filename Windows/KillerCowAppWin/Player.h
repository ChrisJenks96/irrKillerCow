#pragma once

#include <irrlicht.h>
#include "Helper.h"
#include "EnemyOrb.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

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
		Player() {}
		Player(IrrlichtDevice* d);
		const vector3df& GetPosition() { return node->getPosition(); }
		void LookAt(const vector3df p, const float offset);
		ISceneNode* Fire(IrrlichtDevice* device);
		void WeaponFiringLightToggle(bool t) { weaponFiringLight->setVisible(t); }
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
		void ShieldTexture(stringw st, IVideoDriver* driver) { nodeShield->setMaterialTexture(0, driver->getTexture(st));  }
		EnemyOrb& GetOrb() { return orb; }

		//animations
		void FiringAnimation(const float dt);
		void DeathAnimation(const float dt);
		void NotFiringAnimation(const float dt);

		~Player();
	private:
		float animationTimer{ 0.0f };
		float nodeShieldY{ 0.0f };
		int animationID{ PLAYER_ANIMATION_IDLE };
		int oldAnimationID{ 0 };
		float energyDepleteTimer{ 0.0f };
		float energyDepleteRate{ 0.1f };
		float energyRestoreTimer{ 0.0f };
		float energyRestoreRate{ 0.2f };
		int energy{ 100 };
;		int health{ 100 };
		ILightSceneNode* weaponFiringLight;
		IAnimatedMesh* mesh;
		ISceneNode* nodeShield;
		IAnimatedMeshSceneNode* node;
		EnemyOrb orb;
};