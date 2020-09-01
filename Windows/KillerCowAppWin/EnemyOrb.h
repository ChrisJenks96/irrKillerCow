#pragma once

#include <irrlicht.h>
#include "Helper.h"
#include "LightningSceneNode.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define NUM_LIGHTNING_BOLTS 8
#define NUM_LIGHTNING_SPLIT_AXIS (NUM_LIGHTNING_BOLTS / 2)
#define LIGHTNING_ROT_SPEED 0.6f

class EnemyOrb
{
public:
	EnemyOrb() {}
	EnemyOrb(IrrlichtDevice* d);
	void Update(const float dt);
	ISceneNode* GetNode() { return node; }
	~EnemyOrb() {}
private:
	LightningSceneNode* bolts[NUM_LIGHTNING_BOLTS];
	ISceneNode* node;
};