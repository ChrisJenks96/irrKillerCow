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

	IAnimatedMesh* mesh = smgr->getMesh("media/base_plane/base_plane.obj");
	if (mesh)
	{
		IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
		node->setPosition(vector3df(0.0f, -1.0f, 0.0f));
		node->setScale(vector3df(2.0f));
		//ground texture id (0,0)
		node->getMaterial(0).getTextureMatrix(0).setScale(6.0f);
		if (node)
		{
			//node->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
			node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			node->setMaterialFlag(EMF_LIGHTING, true);
		}
	}

	smgr->addLightSceneNode(0, vector3df(0.0f, 0.0f, 0.0f), SColorf(1.0f, 1.0f, 1.0f), 100.0f);

	Player p(device);
	EnemyFactory ef(device, 6);
	smgr->addCameraSceneNode(0, vector3df(3.0f, 8.0f, -5.0f), p.GetPosition());
	
	u32 then = device->getTimer()->getTime();

	while(device->run())
	{
		//time
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;

		ef.Update(p.GetPosition(), frameDeltaTime);
		driver->beginScene(true, true, SColor(255,100,101,140));
		smgr->drawAll();
		driver->endScene();
	}

	device->drop();
	return 0;
}