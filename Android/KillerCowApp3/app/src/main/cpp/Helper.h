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

#define ZOOM_INTO_BOSS_SPEED 1.3f
#define ZOOM_INTO_BOSS_DEAD_SPEED 0.8f
static vector3df defaultCamPos = { 8.0f, 15.0f, -14.0f };
static vector3df bossFightCamPos = { 10.0f, 17.0f, -16.0f };

#define GROUNDSCENENODE_BASE_ID 1

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
	transition_time_start = (f32)device->getTimer()->getTime();
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

static vector3df getSceneNodeFromScreenCoordinatesBB(ISceneManager* smgr, IVideoDriver* driver, ITriangleSelector* sel, position2d<s32> pos, s32 idBitMask)
{
   const SViewFrustum* f = smgr->getActiveCamera()->getViewFrustum();
   core::vector3df farLeftUp = f->getFarLeftUp();
   core::vector3df lefttoright = f->getFarRightUp() - farLeftUp;
   core::vector3df uptodown = f->getFarLeftDown() - farLeftUp;
   core::dimension2d<u32> screenSize = driver->getScreenSize();
   f32 dx = pos.X / (f32)screenSize.Width;
   f32 dy = pos.Y / (f32)screenSize.Height;
   core::vector3df end = farLeftUp + (lefttoright * dx) + (uptodown * dy);

   core::line3d<f32> line;
   line.start = smgr->getActiveCamera()->getPosition();
   line.end = line.start +(end - line.start).normalize() * 1000.0f;

   ISceneNode* outNode;
   core::vector3df intersection;
   core::triangle3df tri;
   if (smgr->getSceneCollisionManager()->getCollisionPoint(line, sel, intersection, tri, outNode))
      return intersection;
   return end;
};

#define LIGHTNING_TYPES 5
typedef struct LIGHTNING_TYPE
{
	stringw texture;
	stringw shield_texture;
	SColorf col;
	int damage;
	float energyDepleteRate;
	float energyRestoreRate;
} LIGHTNING_TYPE;

static int currentLightningType = 0;

static LIGHTNING_TYPE lightning_types[] =
{
	LIGHTNING_TYPE{"media/lightning/blue_bolt.png", "media/shields/shield_blue.png", SColorf(0, 0.2f, 0.2f), 2, 0.1f, 0.25f},
	LIGHTNING_TYPE{"media/lightning/yellow_bolt.png", "media/shields/shield_yellow.png", SColorf(0.2f, 0.2f, 0), 2, 0.2f, 0.2f},
	LIGHTNING_TYPE{"media/lightning/green_bolt.png", "media/shields/shield_green.png", SColorf(0, 0.2f, 0), 3, 0.2f, 0.14f},
	LIGHTNING_TYPE{"media/lightning/purple_bolt.png", "media/shields/shield_purple.png", SColorf(0.2f, 0, 0.2f), 6, 0.2f, 0.08f},
	LIGHTNING_TYPE{"media/lightning/red_bolt.png", "media/shields/shield_red.png", SColorf(0.2f, 0, 0), 9, 0.4f, 0.03f},
};