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
		void Fire(IrrlichtDevice* device);
		void WeaponFiringLightToggle(bool t) { weaponFiringLight->setVisible(t); }
		void Idle();
		int GetHealth() { return health; }
		void RemoveHealth(int n) { health -= n; }
		void SetHealth(int n) { health = n; }
		void AddHealth(int n) { health += n; }
		void SetAnimationName(const char* name) { node->setMD2Animation(name); }
		int HealthGUIValueUpdate() {return (int)((float)HEALTH_GUI_SIZE_X * ((float)health / 100.0f));}
		ISceneNode* GetNode() { return node; }
		~Player();
	private:
		int health{ 100 };
		ILightSceneNode* weaponFiringLight;
		IAnimatedMesh* mesh;
		IAnimatedMeshSceneNode* node;
};