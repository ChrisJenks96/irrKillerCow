#pragma once

#include <irrlicht.h>
#include "Helper.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class Enemy
{
public:
	Enemy(IrrlichtDevice* d);
	const vector3df& GetPosition() { return node->getPosition(); }
	void LookAt(const vector3df p, const float offset);
	void MoveTowards(const vector3df p, const float speed);
	~Enemy();
private:
	IAnimatedMesh* mesh;
	IAnimatedMeshSceneNode* node;
};