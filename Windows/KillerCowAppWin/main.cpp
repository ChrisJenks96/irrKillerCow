#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#include "Player.h"
#include "Enemy.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

int main()
{
	IrrlichtDevice *device =
		createDevice( video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
			false, false, false, 0);

	if (!device)
		return 1;

	device->setWindowCaption(L"Killer Cows");
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();

	Player p(device);
	Enemy e(device);
	smgr->addCameraSceneNode(0, vector3df(0, 10.0f, -5.0f), p.GetPosition());
	
	u32 then = device->getTimer()->getTime();

	while(device->run())
	{
		//time
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;

		e.LookAt(p.GetPosition(), -90.0f);
		e.MoveTowards(p.GetPosition(), 2.0f * frameDeltaTime);
		driver->beginScene(true, true, SColor(255,100,101,140));
		smgr->drawAll();
		driver->endScene();
	}

	device->drop();
	return 0;
}