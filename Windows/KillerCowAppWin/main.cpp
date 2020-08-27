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

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

IMeshSceneNode* ufoSceneNode;
IMeshSceneNode* ufoBladesSceneNode;

static void StaticMeshesLoad(IrrlichtDevice* device)
{
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	
	//loading in the ground
	IMesh* mesh = smgr->getMesh("media/base_plane/base_plane.obj");
	IMeshSceneNode* node;
	if (mesh)
	{
		node = smgr->addMeshSceneNode(mesh);
		node->setPosition(vector3df(0.0f, -1.0f, 0.0f));
		node->setScale(vector3df(2.0f));
		//ground texture id (0,0)
		node->getMaterial(0).getTextureMatrix(0).setScale(6.0f);
		if (node)
		{
			node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			node->setMaterialFlag(EMF_LIGHTING, true);
		}
	}

	//loading in the ufo
	mesh = smgr->getMesh("media/ufo/ufo_crashed.obj");
	if (mesh)
	{
		ufoSceneNode = smgr->addMeshSceneNode(mesh);
		ufoSceneNode->setPosition(vector3df(-2.0f, -4.0f, 5.0f));
		//add the light to the bottom of the craft
		smgr->addLightSceneNode(ufoSceneNode, vector3df(0.5f, -0.2f, -1.5f), SColorf(0.0f, 1.0f, 1.0f, 1.0f), 50.0f);
		ufoSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
		ufoSceneNode->setScale(vector3df(1.0f));

		if (ufoSceneNode)
		{
			ufoSceneNode->setMaterialFlag(EMF_LIGHTING, true);
			ufoSceneNode->setMaterialType(EMT_SOLID);
			//the body
			ufoSceneNode->getMaterial(1).getTextureMatrix(0).setScale(2.0f);
			ufoSceneNode->getMaterial(1).Shininess = 20.0f;
			ufoSceneNode->getMaterial(1).SpecularColor.set(255, 80, 80, 80);
		}
	}

	//loading in the ufo blades
	mesh = smgr->getMesh("media/ufo/ufo_blades.obj");
	if (mesh)
	{
		ufoBladesSceneNode = smgr->addMeshSceneNode(mesh);
		ufoBladesSceneNode->setPosition(vector3df(-2.0f, -4.0f, 5.0f));
		ufoBladesSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
		ufoBladesSceneNode->setScale(vector3df(1.0f));

		if (ufoBladesSceneNode)
		{
			ufoBladesSceneNode->setMaterialFlag(EMF_LIGHTING, true);
			ufoBladesSceneNode->setMaterialType(EMT_SOLID);
		}
	}
}

int main()
{
	/* initialize random seed: */
	srand(time(NULL));

	IrrlichtDevice *device =
		createDevice( video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
			false, false, false, 0);

	if (!device)
		return 1;

	device->setWindowCaption(L"Killer Cows");
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();

	//load the non important static meshes for the scene with no behaviour
	StaticMeshesLoad(device);

	smgr->addLightSceneNode(0, vector3df(0.0f, 10.0f, 0.0f), SColorf(0.8f, 0.8f, 0.8f), 15.0f);

	Player p(device);
	EnemyFactory ef(device, 6);
	smgr->addCameraSceneNode(0, vector3df(3.0f, 10.0f, -9.0f), p.GetPosition());
	
	//debug ufo up close
	//smgr->addCameraSceneNode(0, vector3df(0.0f, 3.0f, 7.0f), vector3df(-2.0f, -4.0f, 5.0f));

	u32 then = device->getTimer()->getTime();

	while(device->run())
	{
		//time
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;
		
		//rotate the blades around the craft
		ufoBladesSceneNode->setRotation(ufoBladesSceneNode->getRotation() + vector3df(0.0f, 25.0f * frameDeltaTime, 0.0f));

		ef.Update(p.GetPosition(), frameDeltaTime);
		driver->beginScene(true, true, SColor(255,100,101,140));
		smgr->drawAll();
		driver->endScene();
	}

	device->drop();
	return 0;
}