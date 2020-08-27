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
		Player(IrrlichtDevice* d);
		const vector3df& GetPosition() { return node->getPosition(); }
		void LookAt(const vector3df p, const float offset);
		void Fire(IrrlichtDevice* device);
		void WeaponFiringLightToggle(bool t) { weaponFiringLight->setVisible(t); }
		void Idle();
		ISceneNode* GetNode() { return node; }
		~Player();
	private:
		ILightSceneNode* weaponFiringLight;
		IAnimatedMesh* mesh;
		IAnimatedMeshSceneNode* node;
};