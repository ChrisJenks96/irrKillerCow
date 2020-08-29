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

#define HEALTH_GUI_SIZE_X 160

#define LIGHTNING_SCALE 0.4f

static vector3df SceneNodeDir(ISceneNode* node)
{
	matrix4 mat = node->getAbsoluteTransformation();
	vector3df in(mat[8], mat[9], mat[10]);
	in.normalize();
	return in;
}

static int transition_alpha = 255;  //the alpha value of the rectangle
static f32 transition_time_start = -1;  //when the transition start...

  //Call this function when you want to start the effect
static void startTransitionFadeOut(IrrlichtDevice* device)
{
	transition_time_start = device->getTimer()->getTime();
}

//this function do the real work and must be placed between "smgr->drawAll();" and "driver->endScene();"
  //you need to pass to it the speed of the transition and the current time
static void updateFadeIn(IrrlichtDevice* device, irr::f32 speed, irr::f32 current_time)
{
	float difference = (current_time - transition_time_start) / 1000;
	IVideoDriver* driver = device->getVideoDriver();
	driver->draw2DRectangle(irr::video::SColor(transition_alpha, 0, 0, 0),
		irr::core::rect<irr::s32>(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height));
	if (difference >= speed / 1000)
	{
		transition_alpha--;
		transition_time_start = current_time;
	}
}

static void updateFadeOut(IrrlichtDevice* device, irr::f32 speed, irr::f32 current_time)
{
	float difference = (current_time - transition_time_start) / 1000;
	IVideoDriver* driver = device->getVideoDriver();
	driver->draw2DRectangle(irr::video::SColor(transition_alpha, 0, 0, 0),
		irr::core::rect<irr::s32>(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height));
	if (difference >= speed / 1000)
	{
		transition_alpha++;
		if (transition_alpha > 254)
			transition_alpha = 255;
		transition_time_start = current_time;
	}
}

#define LIGHTNING_TYPES 5
typedef struct LIGHTNING_TYPE
{
	stringw texture;
	SColor col;
	int damage;
	float energyDepleteRate;
	float energyRestoreRate;
} LIGHTNING_TYPE;

static int currentLightningType = 4;

static LIGHTNING_TYPE lightning_types[] =
{
	LIGHTNING_TYPE{"media/lightning/blue_bolt.png", SColor(255, 0, 128, 255), 1, 0.1f, 0.2f},
	LIGHTNING_TYPE{"media/lightning/yellow_bolt.png", SColor(255, 255, 255, 0), 1, 0.2f, 0.2f},
	LIGHTNING_TYPE{"media/lightning/green_bolt.png", SColor(255, 0, 255, 0), 2, 0.2f, 0.3f},
	LIGHTNING_TYPE{"media/lightning/purple_bolt.png", SColor(255, 255, 0, 255), 3, 0.2f, 0.5f},
	LIGHTNING_TYPE{"media/lightning/red_bolt.png", SColor(255, 255, 0, 0), 5, 0.3f, 0.03f},
};