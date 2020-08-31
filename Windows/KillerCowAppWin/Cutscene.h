#pragma once

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define NUM_CUTSCENE_PHASES 2
//how many passes of ground terrain before the next cutscene
#define NUM_CUTSCENE1_PASSES 2
//when the lightning strikes
#define CUTSCENE2_LIGHTNING_PASS 140.0f
#define CUTSCENE2_END 280.0f

#define CUTSCENE3_CRASH_AHEAD_DISTANCE 400.0f
#define CUTSCENE3_ROTATE_SPEED 25.0f

#define CUTSCENE_UFO_HEIGHT 40.0f

//globally used cutscene vars
static float cutsceneUFOSpeed = 90.0f;
static float cutscenespeedAccum = 0.0f;
//individual cutscene vars
static int cutscene1EndPass = 0;
static int cutscene1currentGround = 0;
static int cutscene1GroundDistance = 360.0f;
static vector3df cutscene1CamPosition = { 10.0f, 60.0f, 7.0f };
static vector3df cutscene2CamPosition = { -7.55f, 0.46f, 24.6f };
static vector3df cutscene3CamPosition = { 3.0f, 5.0f, -1.0f };
static vector3df cutscene3CrashPosition;
static bool cutscene3FadeOut = false;

static bool cutscene4AlienOutOfShip = false;
static bool cutscene4AlienMovingTowards = false;
static vector3df cutscene4PlayerPosBegin = { 0.96f, 0.0f, 1.3f };
static vector3df cutscene4PlayerPosEnd = { 3.1f, 0.0f, -4.69f };