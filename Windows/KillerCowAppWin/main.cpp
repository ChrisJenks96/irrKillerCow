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
#include <string>
#include <sstream>

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#endif

IrrlichtDevice* device;
MyEventReceiver er;
ICameraSceneNode* cam;
IMeshSceneNode* groundSceneNode;
IMeshSceneNode* ufoSceneNode;
IMeshSceneNode* ufoBladesSceneNode;
ILightSceneNode* mainDirLight;

Player p;
EnemyFactory ef;

static void StaticMeshesLoad(IrrlichtDevice* device)
{
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	
	//loading in the ground
	IMesh* mesh = smgr->getMesh("media/base_plane/base_plane.obj");
	if (mesh)
	{
		groundSceneNode = smgr->addMeshSceneNode(mesh);
		groundSceneNode->setPosition(vector3df(0.0f, -1.0f, 0.0f));
		groundSceneNode->setScale(vector3df(2.0f));
		//ground texture id (0,0)
		groundSceneNode->getMaterial(1).getTextureMatrix(0).setScale(6.0f);
		if (groundSceneNode)
		{
			groundSceneNode->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			groundSceneNode->setMaterialFlag(EMF_LIGHTING, true);
		}
	}

	//loading in the ufo
	mesh = smgr->getMesh("media/ufo/ufo_crashed.obj");
	if (mesh)
	{
		ufoSceneNode = smgr->addMeshSceneNode(mesh);
		ufoSceneNode->setPosition(vector3df(-2.0f, -4.0f, 5.0f));
		//add the light to the bottom of the craft
		smgr->addLightSceneNode(ufoSceneNode, vector3df(0.0f, 5.0f, 0.0f), SColorf(0.0f, 1.0f, 1.0f, 1.0f), 120.0f);
		ufoSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
		ufoSceneNode->setScale(vector3df(1.25f));

		if (ufoSceneNode)
			ufoSceneNode->setMaterialFlag(EMF_LIGHTING, true);
	}

	//loading in the ufo blades
	mesh = smgr->getMesh("media/ufo/ufo_blades.obj");
	if (mesh)
	{
		ufoBladesSceneNode = smgr->addMeshSceneNode(mesh);
		ufoBladesSceneNode->setPosition(vector3df(-2.0f, -4.0f, 5.0f));
		ufoBladesSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
		ufoBladesSceneNode->setScale(vector3df(1.25f));

		if (ufoBladesSceneNode)
			ufoBladesSceneNode->setMaterialFlag(EMF_LIGHTING, true);
	}
}

void GameSetup(IrrlichtDevice* device)
{
	ISceneManager* smgr = device->getSceneManager();

	//load the non important static meshes for the scene with no behaviour
	StaticMeshesLoad(device);

	p = Player(device);
	ef = EnemyFactory(device, 10);

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

	ef.Update(p.GetPosition(), frameDeltaTime);
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
	cam = smgr->addCameraSceneNode();
	return 0;
}

int main()
{
	if (Sys_Init() != -1)
	{
		//GameSetup(device);

		u32 then = device->getTimer()->getTime();
		ICursorControl* cursor = device->getCursorControl();
		s32 MouseX = cursor->getPosition().X;
		s32 MouseXPrev = MouseX;

		IVideoDriver* driver = device->getVideoDriver();
		ISceneManager* smgr = device->getSceneManager();
		while (device->run())
		{
			//time
			const u32 now = device->getTimer()->getTime();
			const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
			then = now;

			//GameUpdate(device, MouseX, MouseXPrev, frameDeltaTime);

			driver->beginScene(true, true, SColor(255, 100, 101, 140));
			smgr->drawAll();
			driver->endScene();
		}
	}

	device->drop();
	return 0;
}