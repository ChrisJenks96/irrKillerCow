#pragma once

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define PI 3.142
#define PI2 (PI * 2)

static vector3df SceneNodeDir(ISceneNode* node)
{
	matrix4 mat = node->getAbsoluteTransformation();
	vector3df in(mat[8], mat[9], mat[10]);
	in.normalize();
	return in;
}