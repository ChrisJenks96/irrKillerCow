#pragma once

#include <irrlicht.h>
#include "Helper.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

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
		int EnergyGUIValueUpdate() { return (int)((float)HEALTH_GUI_SIZE_X * ((float)energy / 100.0f)); }
		int HealthGUIValueUpdate() {return (int)((float)HEALTH_GUI_SIZE_X * ((float)health / 100.0f));}
		ISceneNode* GetNode() { return node; }
		float GetEnergyDepleteRate() { return energyDepleteRate; }
		void SetEnergyDepleteRate(float s) { energyDepleteRate = s; }
		float GetEnergyRestoreRate() { return energyRestoreRate; }
		void SetEnergyRestoreRate(float s) { energyRestoreRate = s; }
		~Player();
	private:
		float energyDepleteTimer{ 0.0f };
		float energyDepleteRate{ 0.1f };
		float energyRestoreTimer{ 0.0f };
		float energyRestoreRate{ 0.2f };
		int energy{ 100 };
;		int health{ 100 };
		ILightSceneNode* weaponFiringLight;
		IAnimatedMesh* mesh;
		IAnimatedMeshSceneNode* node;
};