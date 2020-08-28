#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#include "Player.h"
#include "Enemy.h"
#include <time.h>
#include "ER.h"

#include "Cutscene.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#endif

#define STATE_GAME 0
#define STATE_INTRO_CUTSCENE 1
#define STATE_MENU 2
#define STATE_OPTIONS 3
#define STATE_GAME_OVER 4

IrrlichtDevice* device;
MyEventReceiver er;
ICameraSceneNode* cam;
IMeshSceneNode* cutsceneGroundSceneNode[2];
IMeshSceneNode* groundSceneNode;
IMeshSceneNode* ufoSceneNode;
IMeshSceneNode* ufoBladesSceneNode;
ILightSceneNode* mainDirLight;

//game specifics
Player p;
EnemyFactory ef;

//cutscene specifics
int currentCutscene = 0;
float gameOverTimer = 0.0f;
#define GAME_OVER_FADE_OUT_TIME 3.0f
#define GAME_OVER_FINISH_TIME 5.5f

static void StaticMeshesLoad(IrrlichtDevice* device)
{
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	
	//loading in the ground
	IMesh* mesh = smgr->getMesh("media/base_plane/base_plane.obj");
	if (mesh)
	{
		groundSceneNode = smgr->addMeshSceneNode(mesh);
		
		groundSceneNode->setScale(vector3df(2.0f));
		//ground texture id (0,0)
		groundSceneNode->getMaterial(1).getTextureMatrix(0).setScale(6.0f);
		if (groundSceneNode)
		{
			groundSceneNode->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			groundSceneNode->setMaterialFlag(EMF_LIGHTING, true);
			groundSceneNode->setVisible(false);
		}
	}

	//only for cutscene... unload when you're done!!
	mesh = smgr->getMesh("media/base_plane/cutscene_base_plane.obj");
	if (mesh)
	{
		for (int i = 0; i < 2; i++)
		{
			cutsceneGroundSceneNode[i] = smgr->addMeshSceneNode(mesh);
			cutsceneGroundSceneNode[i]->setScale(vector3df(20.0f));
			//ground texture id (0,0)
			cutsceneGroundSceneNode[i]->getMaterial(0).setTexture(0, driver->getTexture("media/base_plane/grass_dirt.jpg"));
			cutsceneGroundSceneNode[i]->getMaterial(0).getTextureMatrix(0).setScale(vector3df(60.0f, 120.0f, 0.0f));
			if (groundSceneNode)
			{
				cutsceneGroundSceneNode[i]->setMaterialFlag(EMF_LIGHTING, false);
				cutsceneGroundSceneNode[i]->setPosition(vector3df(-40.0f, 0.0f, 0.0f));
			}
		}

		cutsceneGroundSceneNode[0]->setVisible(true);
		cutsceneGroundSceneNode[1]->setVisible(false);
	}

	//loading in the ufo
	mesh = smgr->getMesh("media/ufo/ufo_crashed.obj");
	if (mesh)
	{
		ufoSceneNode = smgr->addMeshSceneNode(mesh);
		
		//add the light to the bottom of the craft
		smgr->addLightSceneNode(ufoSceneNode, vector3df(0.0f, 5.0f, 0.0f), SColorf(0.0f, 1.0f, 1.0f, 1.0f), 120.0f);
		ufoSceneNode->setScale(vector3df(1.25f));

		if (ufoSceneNode)
			ufoSceneNode->setMaterialFlag(EMF_LIGHTING, true);
	}

	//loading in the ufo blades
	mesh = smgr->getMesh("media/ufo/ufo_blades.obj");
	if (mesh)
	{
		ufoBladesSceneNode = smgr->addMeshSceneNode(mesh);
		ufoBladesSceneNode->setScale(vector3df(1.25f));

		if (ufoBladesSceneNode)
			ufoBladesSceneNode->setMaterialFlag(EMF_LIGHTING, true);
		ufoBladesSceneNode->setParent(ufoSceneNode);
		ufoBladesSceneNode->setPosition(vector3df(0.0f, -1.0f, 0.0f));
	}
}

//MUST ALWAYS LOAD CUTSCENEINIT FIRST... THIS BOOTS ALL OUR ASSETS FOR THE GAME
void CutsceneInit(IrrlichtDevice* device)
{
	//load the non important static meshes for the scene with no behaviour
	StaticMeshesLoad(device);
	ufoSceneNode->setPosition(vector3df(0.0f, CUTSCENE_UFO_HEIGHT, 0.0f));
	ufoSceneNode->setRotation(vector3df(0.0f, -90.0f, 0.0f));
	//ufoBladesSceneNode->setPosition(vector3df(0.0f, 40.0f, 0.0f));
	ufoBladesSceneNode->setRotation(vector3df(0.0f, -90.0f, 0.0f));
	cam->setPosition(cutscene1CamPosition);
	cam->setTarget(ufoSceneNode->getPosition());
}

void CutsceneUnload(IrrlichtDevice* device)
{
	cutsceneGroundSceneNode[0]->setVisible(false);
	cutsceneGroundSceneNode[1]->setVisible(false);
	groundSceneNode->setVisible(true);
}

void CutsceneUpdate(IrrlichtDevice* device, const float dt)
{
	if (currentCutscene == 0)
	{
		//scroll the cutscene forward to give the illusion of the craft moving
		cutscenespeedAccum += cutsceneUFOSpeed * dt;
		if (cutscenespeedAccum > cutscene1GroundDistance){
			cutscene1currentGround++;
			cutscene1EndPass++;
			if (cutscene1currentGround > 1)
				cutscene1currentGround = 0;

			if (cutscene1currentGround == 1) {
				cutsceneGroundSceneNode[0]->setVisible(false);
				cutsceneGroundSceneNode[1]->setVisible(true);
				cutsceneGroundSceneNode[1]->setPosition(cutsceneGroundSceneNode[0]->getPosition() - vector3df(0.0f, 0.0f, cutscene1GroundDistance));
			}

			if (cutscene1currentGround == 0) {
				cutsceneGroundSceneNode[1]->setVisible(false);
				cutsceneGroundSceneNode[0]->setVisible(true);
				cutsceneGroundSceneNode[0]->setPosition(cutsceneGroundSceneNode[1]->getPosition() - vector3df(0.0f, 0.0f, cutscene1GroundDistance));
			}
			
			cutscenespeedAccum = 0;

			if (cutscene1EndPass >= NUM_CUTSCENE1_PASSES)
			{
				cam->setPosition(cutscene2CamPosition);
				//move the ufo back ready for the next scene (it will shoot past the screen)
				ufoSceneNode->setPosition(ufoSceneNode->getPosition() - vector3df(0.0f, 0.0f, 200.0f));
				ufoSceneNode->setRotation(vector3df(0.0f, 90.0f, 0.0f));
				//ufoBladesSceneNode->setPosition(ufoSceneNode->getPosition() - vector3df(0.0f, 0.0f, 200.0f));
				ufoBladesSceneNode->setRotation(vector3df(0.0f, 90.0f, 0.0f));
				currentCutscene = 1;
			}
				
		}

		cutsceneGroundSceneNode[cutscene1currentGround]->setPosition(cutsceneGroundSceneNode[cutscene1currentGround]->getPosition() + vector3df(0.0f, 0.0f, cutsceneUFOSpeed * dt));
		//rotate the blades around the craft
		ufoBladesSceneNode->setRotation(ufoBladesSceneNode->getRotation() + vector3df(0.0f, 750.0f * dt, 0.0f));

	}

	else if (currentCutscene == 1)
	{
		cam->setTarget(ufoSceneNode->getPosition());
		ufoSceneNode->setPosition(ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, cutsceneUFOSpeed * dt));
		//ufoBladesSceneNode->setPosition(ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, cutsceneUFOSpeed * dt));

		cutscenespeedAccum += cutsceneUFOSpeed * dt;
		//lightning strike point
		if (cutscenespeedAccum > CUTSCENE2_LIGHTNING_PASS && cutscenespeedAccum <= CUTSCENE2_LIGHTNING_PASS+3.0f){
			//LIGHTNING EFFECTS HAPPEN HERE....
		}

		if (cutscenespeedAccum > CUTSCENE2_END)
		{
			cutscene3CrashPosition = ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, CUTSCENE3_CRASH_AHEAD_DISTANCE);
			cutscene3CrashPosition.Y = -10.0f;
			currentCutscene = 2;
		}
	}

	else if (currentCutscene == 2)
	{
		//attach and move the camera onto the ufo for crashing landing cam
		cam->setTarget(ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, 10.0f));
		cam->setPosition(ufoSceneNode->getPosition() + cutscene3CamPosition);
		
		vector3df dir = (cutscene3CrashPosition - ufoSceneNode->getPosition()).normalize();
		ufoSceneNode->setPosition(ufoSceneNode->getPosition() + (dir * cutsceneUFOSpeed * dt));
		ufoSceneNode->setRotation(ufoSceneNode->getRotation() + vector3df(CUTSCENE3_ROTATE_SPEED * dt, 0.0f, 0.0f));

		//fade to black
		if (ufoSceneNode->getPosition().Y < (CUTSCENE_UFO_HEIGHT - 10.0f))
			cutscene3FadeOut = true;
	}
}

void GameInit(IrrlichtDevice* device)
{
	ISceneManager* smgr = device->getSceneManager();

	p = Player(device);
	ef = EnemyFactory(device, 3);

	ufoSceneNode->setPosition(vector3df(-2.0f, -4.0f, 5.0f));
	ufoSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
	ufoBladesSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
	groundSceneNode->setPosition(vector3df(0.0f, -1.0f, 0.0f));

	cam->setPosition(vector3df(3.0f, 10.0f, -9.0f));
	cam->setTarget(p.GetPosition());
}

void GameUpdate(IrrlichtDevice* device, s32& MouseX, s32& MouseXPrev, const float& frameDeltaTime)
{
	//rotate the player around
	MouseX = er.GetMouseState().Position.X;
	s32 MouseXDiff = MouseX - MouseXPrev;
	p.GetNode()->setRotation(p.GetNode()->getRotation() + vector3df(0.0f, (MouseXDiff * (100.0f * frameDeltaTime)), 0.0f));
	MouseXPrev = MouseX;

	//firing state for the player
	if (er.GetMouseState().LeftButtonDown)
		p.Fire(device);
	else
		p.Idle();

	//rotate the blades around the craft
	ufoBladesSceneNode->setRotation(ufoBladesSceneNode->getRotation() + vector3df(0.0f, 25.0f * frameDeltaTime, 0.0f));

	ef.Update(p, frameDeltaTime);
}

bool Sys_Init()
{
	/* initialize random seed: */
	srand(time(NULL));

	device = createDevice(video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
		false, false, true, &er);

	if (!device)
		return -1;

	device->setWindowCaption(L"Killer Cows");
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	mainDirLight = smgr->addLightSceneNode();
	mainDirLight->setLightType(ELT_DIRECTIONAL);
	//cam = smgr->addCameraSceneNodeFPS(0, 100.0f, 0.01f);
	cam = smgr->addCameraSceneNode();
	return 0;
}

void GameReset()
{
	p.SetHealth(100);
	p.SetAnimationName("attack");
	ef.ForceReset();
	cam->setPosition(vector3df(3.0f, 10.0f, -9.0f));
	cam->setTarget(p.GetPosition());
}

int main()
{
	if (Sys_Init() != -1)
	{
		int state = STATE_INTRO_CUTSCENE;

		if (state == STATE_GAME)
			GameInit(device);
		else if (state == STATE_INTRO_CUTSCENE) {
			startTransitionFadeOut(device);
			CutsceneInit(device);
		}

		u32 then = device->getTimer()->getTime();
		ICursorControl* cursor = device->getCursorControl();
		s32 MouseX = cursor->getPosition().X;
		s32 MouseXPrev = MouseX;

		IVideoDriver* driver = device->getVideoDriver();
		IGUIEnvironment* gui = device->getGUIEnvironment();
		ISceneManager* smgr = device->getSceneManager();

		IGUIImage* health_inside = gui->addImage(driver->getTexture("media/gui/healthbar_inside.png"), vector2di(15, 13));
		health_inside->setMaxSize(dimension2du(HEALTH_GUI_SIZE_X, 10));
		IGUIImage* cow_icon = gui->addImage(driver->getTexture("media/gui/cow_icon.png"), vector2di(driver->getViewPort().getWidth() - 74, 10));
		cow_icon->setMaxSize(dimension2du(64, 64));
		IGUIImage* health_outside = gui->addImage(driver->getTexture("media/gui/healthbar_outside.png"), vector2di(10, 10));
		health_outside->setMaxSize(dimension2du(170, 15));

		while (device->run())
		{
			//time
			const u32 now = device->getTimer()->getTime();
			const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
			then = now;

			if (state == STATE_GAME)
			{
				//fade back into the game
				if (cutscene3FadeOut && transition_alpha != 0)
					updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
				else
				{
					cutscene3FadeOut = false;
					GameUpdate(device, MouseX, MouseXPrev, frameDeltaTime);
					if (p.GetHealth() <= 0) {
						p.SetAnimationName("crdeth");
						cam->setPosition(vector3df(cam->getPosition().X, cam->getPosition().Y - 5.0f, cam->getPosition().Z));
						//play death animation
						state = STATE_GAME_OVER;
					}
				}
				
			}
				
			else if (state == STATE_INTRO_CUTSCENE)
			{
				if (cutscene3FadeOut && ufoSceneNode->getPosition().Y < 0.0f){
					CutsceneUnload(device);
					state = STATE_GAME;
					GameInit(device);
				}

				else
					CutsceneUpdate(device, frameDeltaTime);
			}

			else if (state == STATE_GAME_OVER)
			{
				cam->setTarget(ef.GetNearestEnemy(p)->GetPosition());
				gameOverTimer += 1.0f * frameDeltaTime;

				if (gameOverTimer > GAME_OVER_FINISH_TIME) {
					gameOverTimer = 0.0f;
					GameReset();
					state = STATE_GAME;
				}
			}
				
			//printf("%f, %f, %f, %f, %f, %f\n", cam->getPosition().X, cam->getPosition().Y, cam->getPosition().Z,
				//cam->getRotation().X, cam->getRotation().Y, cam->getRotation().Z);

			driver->beginScene(true, true, SColor(255, 100, 101, 140));
			smgr->drawAll();
			if (state == STATE_INTRO_CUTSCENE) {
				if (!cutscene3FadeOut && transition_alpha != 0)
					updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
				else if (cutscene3FadeOut)
					updateFadeOut(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
			}

			else if (state == STATE_GAME_OVER)
			{
				if (gameOverTimer > GAME_OVER_FADE_OUT_TIME)
					updateFadeOut(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());

				if (gameOverTimer > GAME_OVER_FINISH_TIME)
					updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
			}

			else if (state == STATE_GAME || state == STATE_GAME_OVER)
			{
				health_inside->setMaxSize(dimension2du(p.HealthGUIValueUpdate(), 10));
				health_outside->draw();
				if (p.GetHealth() > 0)
					health_inside->draw();
				cow_icon->draw();
			}
			
			driver->endScene();
		}
	}

	device->drop();
	return 0;
}