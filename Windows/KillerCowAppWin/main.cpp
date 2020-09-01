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
#include "LightningSceneNode.h"
int QUAD_SEGMENT_INCREMENT = -10.0f;

#include "EnemyOrb.h"
#include "BigEnemy.h"
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
#define STATE_POWERUP 5

IrrlichtDevice* device;
MyEventReceiver er;
ICameraSceneNode* cam;
IMeshSceneNode* cutsceneGroundSceneNode[2];
IMeshSceneNode* groundSceneNode;
IMeshSceneNode* ufoSceneNode;
IMeshSceneNode* ufoBladesSceneNode;
IMeshSceneNode* cowHeadGameOver;
ILightSceneNode* dirLight;
ILightSceneNode* ufoSpotlight;
//custom scenenode
LightningSceneNode* cutsceneLightning;
EnemyOrb enemyOrb;
vector3df bigEnemyOldPos;
vector3df bigEnemyNewPos;
//how far the enemy moves out of the capsule
float bigEnemyMoveCounter = 0.0f;
float bigEnemyCapsuleTakeoff = 1.0f;
float bigEnemyMoveMax = 2.1f;
float bigEnemyCapVelocity = -0.0f;
bool bigEnemyOnMove = false;
bool bigEnemyStopShooting = false;
bool bigEnemyFirstMove = false;
bool bigEnemyWalkOutCap = false;
bool bigEnemyCapOutOfRange = false;
bool bossScene = false;
bool bossDead = false;

vector3df OldCameraPosition;

//game specifics
Player p;
EnemyFactory ef;
BigEnemy be;

bool playerNukeGoneOff = false;
bool globalPlayerMunchFlag = false;
int totalCowsKilled = 0;
int cowsKilled = 0;
float cowsXp = 0.0f;
int cowsXpLvl = 0;
float xpMod = 4.8f;
float lightningUpgradeTimer = 0.0f;
float lightningUpgradeWait = 3.0f;
ITexture* menuBkg;
IGUIFont* font;

//cutscene specifics
int currentCutscene = 0;
float gameOverTimer = 0.0f;
#define GAME_OVER_FADE_OUT_TIME 3.0f
#define GAME_OVER_FINISH_TIME 10.0f

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
		/*for (int i = 0; i < groundSceneNode->getMaterialCount(); i++)
		{
			SMaterial& s = groundSceneNode->getMaterial(i);
			printf("");
		}*/
		groundSceneNode->getMaterial(GROUNDSCENENODE_BASE_ID).getTextureMatrix(0).setScale(18.0f);
		if (groundSceneNode)
		{
			//groundSceneNode->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			groundSceneNode->setMaterialFlag(EMF_LIGHTING, true);
			groundSceneNode->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
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
			if (groundSceneNode)
			{
				cutsceneGroundSceneNode[i]->setMaterialFlag(EMF_LIGHTING, true);
				cutsceneGroundSceneNode[i]->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
				cutsceneGroundSceneNode[i]->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
				cutsceneGroundSceneNode[i]->setPosition(vector3df(-40.0f, 0.0f, 0.0f));
				//ground texture id (0,0)
				//cutsceneGroundSceneNode[i]->getMaterial(0).setTexture(0, driver->getTexture("media/base_plane/grass_dirt.jpg"));
				cutsceneGroundSceneNode[i]->getMaterial(2).getTextureMatrix(0).setScale(vector3df(12.0f, 24.0f, 0.0f));
			}

			/*for (int c = 0; c < cutsceneGroundSceneNode[i]->getMaterialCount(); c++)
			{
				SMaterial& s = cutsceneGroundSceneNode[i]->getMaterial(c);
				printf("");
			}*/
		}

		cutsceneGroundSceneNode[0]->setVisible(true);
		cutsceneGroundSceneNode[1]->setVisible(false);	
	}

	//loading in the ufo
	mesh = smgr->getMesh("media/ufo/ufo_crashed.obj");
	if (mesh)
	{
		ufoSceneNode = smgr->addMeshSceneNode(mesh);
		ufoSceneNode->setScale(vector3df(1.25f));

		if (ufoSceneNode){
			ufoSceneNode->setMaterialFlag(EMF_LIGHTING, true);
			ufoSceneNode->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			//add the light to the bottom of the craft
			//smgr->addLightSceneNode(ufoSceneNode, vector3df(0.0f, -5.0f, 0.0f), SColorf(0.0f, 1.0f, 1.0f, 1.0f), 20.0f);
			ufoSpotlight = smgr->addLightSceneNode(ufoSceneNode, vector3df(0.0f, 1.0f, 0.0f), SColorf(0.0f, 1.0f, 1.0f, 1.0f), 30000.0f);
			ufoSpotlight->getLightData().Type = video::ELT_SPOT;
			ufoSpotlight->getLightData().InnerCone = 30.0f;
			ufoSpotlight->getLightData().OuterCone = 70.0f;
			ufoSpotlight->getLightData().Falloff = 20.0f;
			ufoSpotlight->setRotation(vector3df(90.0f, 0.0f, 0.0f)); //default is (1,1,0) for directional lights
		}
	}

	//loading in the ufo blades
	mesh = smgr->getMesh("media/ufo/ufo_blades.obj");
	if (mesh)
	{
		ufoBladesSceneNode = smgr->addMeshSceneNode(mesh);
		ufoBladesSceneNode->setScale(vector3df(1.25f));

		if (ufoBladesSceneNode)
		{
			ufoBladesSceneNode->setMaterialFlag(EMF_LIGHTING, true);
			ufoBladesSceneNode->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
		}
			
		ufoBladesSceneNode->setParent(ufoSceneNode);
		ufoBladesSceneNode->setPosition(vector3df(0.0f, -1.0f, 0.0f));
	}

	mesh = smgr->getMesh("media/cow/cow_head.obj");
	if (mesh)
	{
		cowHeadGameOver = smgr->addMeshSceneNode(mesh);
		cowHeadGameOver->setScale(vector3df(1.25f));

		if (cowHeadGameOver)
		{
			cowHeadGameOver->setMaterialFlag(EMF_LIGHTING, false);
			cowHeadGameOver->setPosition(vector3df(999.0f));
			cowHeadGameOver->getMaterial(0).setTexture(0, driver->getTexture("media/cow/eye.png"));
			cowHeadGameOver->getMaterial(1).setTexture(0, driver->getTexture("media/cow/eye.png"));
			cowHeadGameOver->getMaterial(2).setTexture(0, driver->getTexture("media/cow/cow.png"));
		}
	}

	QUAD_SEGMENT_INCREMENT = -10.0f;
	cutsceneLightning = new LightningSceneNode(smgr->getRootSceneNode(), smgr, 666);
	cutsceneLightning->setMaterialTexture(0, driver->getTexture(lightning_types[currentLightningType].texture));
	cutsceneLightning->setVisible(false);
}

//MUST ALWAYS LOAD CUTSCENEINIT FIRST... THIS BOOTS ALL OUR ASSETS FOR THE GAME
void CutsceneInit(IrrlichtDevice* device)
{
	//main ligth for the game
	dirLight = device->getSceneManager()->addLightSceneNode(0, vector3df(0.0f, 10.0f, 0.0f), SColorf(0.0f, 0.15f, 0.15f, 1.0f), 0.0f);
	dirLight->getLightData().Type = ELT_DIRECTIONAL;
	dirLight->setRotation(vector3df(60.0f, 0.0f, 0.0f)); //default is (1,1,0) for directional lights
	
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
				cutsceneGroundSceneNode[1]->setPosition(cutsceneGroundSceneNode[0]->getPosition() - vector3df(0.0f, 0.0f, (cutscene1GroundDistance*2)));
			}

			if (cutscene1currentGround == 0) {
				cutsceneGroundSceneNode[1]->setVisible(false);
				cutsceneGroundSceneNode[0]->setVisible(true);
				cutsceneGroundSceneNode[0]->setPosition(cutsceneGroundSceneNode[1]->getPosition() - vector3df(0.0f, 0.0f, cutscene1GroundDistance));
			}
			
			cutscenespeedAccum = 0;

			if (cutscene1EndPass >= NUM_CUTSCENE1_PASSES)
			{
				cutsceneGroundSceneNode[0]->setVisible(false);
				cutsceneGroundSceneNode[1]->setVisible(true);
				cutsceneGroundSceneNode[0]->setPosition(cutsceneGroundSceneNode[1]->getPosition() - vector3df(0.0f, 0.0f, cutscene1GroundDistance));
				cutsceneGroundSceneNode[1]->setPosition(cutsceneGroundSceneNode[0]->getPosition() + vector3df(0.0f, 0.0f, cutscene1GroundDistance));
				cam->setPosition(cutscene2CamPosition);
				//move the ufo back ready for the next scene (it will shoot past the screen)
				ufoSceneNode->setPosition(ufoSceneNode->getPosition() - vector3df(0.0f, 0.0f, 200.0f));
				ufoSceneNode->setRotation(vector3df(0.0f, 90.0f, 0.0f));
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
		ufoSpotlight->setPosition(vector3df(0.0f, 4.0f, 0.0f));
		cam->setTarget(ufoSceneNode->getPosition());
		ufoSceneNode->setPosition(ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, cutsceneUFOSpeed * dt));
		//ufoBladesSceneNode->setPosition(ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, cutsceneUFOSpeed * dt));

		cutscenespeedAccum += cutsceneUFOSpeed * dt;
		//lightning strike point
		if (cutscenespeedAccum > CUTSCENE2_LIGHTNING_PASS && cutscenespeedAccum <= CUTSCENE2_LIGHTNING_PASS+3.0f){
			//LIGHTNING EFFECTS HAPPEN HERE....
			cutsceneLightning->setVisible(true);
			//origin of lightning starts at destination of object
			QUAD_SEGMENT_INCREMENT = -10.0f;
			cutsceneLightning->setPosition(ufoSceneNode->getPosition() - vector3df(0.0f, QUAD_SEGMENT_INCREMENT * (QUAD_SEGMENTS-2), 0.0f));
			//TESTING... BLOCK UP THE GAME CHAIN
			//currentCutscene = 999;
		}

		if (cutscenespeedAccum > CUTSCENE2_END)
		{
			cutscene3CrashPosition = ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, CUTSCENE3_CRASH_AHEAD_DISTANCE);
			cutscene3CrashPosition.Y = -10.0f;
			cutsceneLightning->setVisible(false);
			currentCutscene = 2;
		}
	}

	else if (currentCutscene == 2)
	{
		ufoSpotlight->setPosition(vector3df(0.0f, 1.0f, 0.0f));
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
	ef = EnemyFactory(device, 5);
	be = BigEnemy(device, 12.0f);
	enemyOrb = EnemyOrb(device);
	be.GetNode()->setVisible(false);

	ufoSceneNode->setPosition(vector3df(-2.0f, -4.0f, 5.0f));
	ufoSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
	ufoBladesSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
	groundSceneNode->setPosition(vector3df(0.0f, -1.0f, 0.0f));

	//smgr->addLightSceneNode(ufoSceneNode, vector3df(0.0f, 8.0f, 0.0f), SColorf(0.0f, 0.3f, 0.3f, 0.3f), 20.0f);
	QUAD_SEGMENT_INCREMENT = -10.0f;
	cutsceneLightning->setParent(p.GetNode());
	cutsceneLightning->setPosition(vector3df(0.0f, 1.0f, 0.0f));
	cutsceneLightning->setScale(vector3df(LIGHTNING_SCALE));
	cutsceneLightning->setRotation(vector3df(-90.0f, 0.0f, 90.0f));

	p.SetEnergyDepleteRate(lightning_types[currentLightningType].energyDepleteRate);
	p.SetEnergyRestoreRate(lightning_types[currentLightningType].energyRestoreRate);

	cam->setPosition(vector3df(3.0f, 10.0f, -9.0f));
	cam->setTarget(p.GetPosition());

	dirLight->remove();
	dirLight = smgr->addLightSceneNode(0, vector3df(0.0f, 15.0f, 0.0f), SColorf(0.0f, 0.2f, 0.2f, 1.0f), 80.0f);
	dirLight->getLightData().Type = video::ELT_SPOT;
	dirLight->getLightData().InnerCone = 30.0f;
	dirLight->getLightData().OuterCone = 100.0f;
	dirLight->getLightData().Falloff = 20.0f;
	dirLight->setRotation(vector3df(90.0f, 0.0f, 0.0f));
}



void GameUpdate(IrrlichtDevice* device, s32& MouseX, s32& MouseXPrev, const float& frameDeltaTime)
{
	//rotate the player around
	MouseX = er.GetMouseState().Position.X;
	s32 MouseXDiff = MouseX - MouseXPrev;
	p.GetNode()->setRotation(p.GetNode()->getRotation() + vector3df(0.0f, (MouseXDiff * (100.0f * frameDeltaTime)), 0.0f));
	MouseXPrev = MouseX;

	enemyOrb.Update(frameDeltaTime);
	if (bigEnemyCapOutOfRange)
	{
		if (!bigEnemyStopShooting){
			vector3df newPos = ((p.GetPosition() - enemyOrb.GetNode()->getPosition()).normalize()) * 10.0f * frameDeltaTime;
			float dist = (p.GetPosition() - enemyOrb.GetNode()->getPosition()).getLengthSQ();
			enemyOrb.GetNode()->setPosition(enemyOrb.GetNode()->getPosition() + newPos);
			if (dist < 0.2f) {
				enemyOrb.GetNode()->setPosition(be.GetPosition());
				p.RemoveHealth(be.GetAttackDamage());
				if (bigEnemyOnMove)
					bigEnemyStopShooting = true;
			}	
		}

		else			
			enemyOrb.GetNode()->setPosition(be.GetNode()->getPosition() + vector3df(0.0f, 4.0f, 0.4));

	}

	//the enemy entrance etc...
	else
		enemyOrb.GetNode()->setPosition(be.GetNode()->getPosition() + vector3df(0.0f, 4.0f, 0.4));

	if (er.GUINukeToggle)
	{
		if (!playerNukeGoneOff) {
			p.SetEnergy(p.GetEnergy() - 50);
			p.GetOrb().GetNode()->setVisible(true);
			playerNukeGoneOff = true;
			er.GUINukeToggle = false;
		}
	}

	//firing state for the player
	if (er.GetMouseState().LeftButtonDown){
		p.RemoveEnergy(frameDeltaTime);
		if (p.GetEnergy() > 0)
		{	
			p.FiringAnimation(frameDeltaTime);
			ISceneNode* e = p.Fire(device);
			if (e != NULL){
				if (e->getID() == 667)
				{
					be.RemoveHealth(lightning_types[currentLightningType].damage, frameDeltaTime);
					if (be.GetHealth() <= 0) {
						bossDead = true;
						be.GetNode()->setVisible(false);
						cam->setTarget(p.GetPosition());
						//ef.ForceReset();
						cowsKilled += 1;
						bossScene = false;
					}
				}
				else
				{
					Enemy* enemy = ef.FindEnemy(e);
					enemy->RemoveHealth(lightning_types[currentLightningType].damage, frameDeltaTime);
					enemy->GetNode()->getMaterial(0).EmissiveColor = SColor(255, 255, 0, 0);
					if (enemy->GetHealth() <= 0){
						if (!enemy->isDeathAnimationTrigger()){
							cowsXp += ((float)enemy->GetAttackDamage() / 10) * xpMod;
							cowsKilled += 1;
						}
							
						enemy->SetDeathAnimationTrigger(true);
					}
				}
			}

			QUAD_SEGMENT_INCREMENT = -10.0f;
			cutsceneLightning->ArkUpdate(frameDeltaTime);
			float lightningLength = LIGHTNING_SCALE * ((float)p.GetEnergy() / 100.0f);
			cutsceneLightning->setScale(vector3df(lightningLength));
			cutsceneLightning->setVisible(true);
		}
	}
		
	else
	{
		p.NotFiringAnimation(frameDeltaTime);
		p.Idle();
		cutsceneLightning->setVisible(false);
		if (p.GetEnergy() <= 100)
			p.AddEnergy(frameDeltaTime);
		ef.ResetEmission();
	}

	if (playerNukeGoneOff)
	{
		p.GetOrb().GetNode()->setScale(p.GetOrb().GetNode()->getScale() + vector3df(10.0f * frameDeltaTime));
		p.GetOrb().GetNode()->setPosition(p.GetPosition());
		p.GetOrb().Update(frameDeltaTime);

		//DISABLE NUKE BUTTON EHRE TO PREVENT INF NUKES
		if (p.GetOrb().GetNode()->getScale().Y > 50.0f) {
			playerNukeGoneOff = false;
			ef.SetHealthAll(0);
			ef.ForceDeath(xpMod, cowsXp, cowsKilled);
			p.GetOrb().GetNode()->setScale(vector3df(1.6f));
			p.GetOrb().GetNode()->setVisible(false);
		}
			
	}

	//if we have no energy, shut it off
	if (p.GetEnergy() > 0)
		p.ShieldUVScroll(frameDeltaTime);
	else if (p.GetEnergy() <= 0) {
		p.SetEnergy(0);
		er.GUIShieldToggle = false;
	}

	//rotate the blades around the craft
	ufoBladesSceneNode->setRotation(ufoBladesSceneNode->getRotation() + vector3df(0.0f, 25.0f * frameDeltaTime, 0.0f));
	ef.Update(p, frameDeltaTime);

	if (ef.isPlayerGettingMunched() && !globalPlayerMunchFlag){
		globalPlayerMunchFlag = true;
		p.GetNode()->getMaterial(0).EmissiveColor = SColor(255, 255, 0, 0);
	}

	else if (globalPlayerMunchFlag){
		globalPlayerMunchFlag = false;
		ef.SetPlayerGettingMunched(false);
		p.ResetEmission();
	}
}

void MenuInit(IrrlichtDevice* device)
{
	menuBkg = device->getVideoDriver()->getTexture("media/gui/menu.png");
	font = device->getGUIEnvironment()->getFont("media/gui/myfont.xml");
}

void MenuFontDraw(IrrlichtDevice* device)
{
	stringw str = L"Click to Start Game";
	dimension2du s = device->getVideoDriver()->getScreenSize();
	font->draw(str.c_str(), core::rect<s32>(s.Width / 2 - 100, s.Height - 60, 0, 0), video::SColor(255, 255, 255, 255));
}

void HighScoreFontDraw(IrrlichtDevice* device, const int cowsMuggedOff)
{
	stringw str = L"X ";
	str += cowsMuggedOff;
	dimension2du s = device->getVideoDriver()->getScreenSize();
	font->draw(str.c_str(), core::rect<s32>(s.Width - 60, 32, 0, 0), video::SColor(255, 255, 255, 255));
}

bool Sys_Init()
{
	/* initialize random seed: */
	srand(time(NULL));

	device = createDevice(video::EDT_OPENGL, dimension2d<u32>(1280, 720), 16,
		false, false, true, &er);

	if (!device)
		return -1;

	device->setWindowCaption(L"Killer Cows");
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	//cam = smgr->addCameraSceneNodeFPS(0, 100.0f, 0.03f);
	cam = smgr->addCameraSceneNode();
	return 0;
}

void GameReset()
{
	globalPlayerMunchFlag = false;
	cowsXp = 0;
	cowsXpLvl = 0;
	cowsKilled = 0;
	p.SetHealth(100);
	p.SetEnergy(100);
	p.SetAnimationName("idle");
	ef.ForceReset();
	cam->setPosition(vector3df(defaultCamPos));
	cam->setTarget(p.GetPosition());
}

void LightningUpgrade(IrrlichtDevice* device)
{
	cutsceneLightning->getMaterial(0).setTexture(0, device->getVideoDriver()->getTexture(lightning_types[currentLightningType].texture));
	p.ShieldTexture(lightning_types[currentLightningType].shield_texture, device->getVideoDriver());
	p.SetEnergyDepleteRate(lightning_types[currentLightningType].energyDepleteRate);
	p.SetEnergyRestoreRate(lightning_types[currentLightningType].energyRestoreRate);
	//p.LightningChangeCol(lightning_types[currentLightningType].col);
}

int main()
{
	if (Sys_Init() != -1)
	{
		//always boot in menu state
		int state = STATE_MENU;
		MenuInit(device);
		
		u32 then = device->getTimer()->getTime();
		ICursorControl* cursor = device->getCursorControl();
		s32 MouseX = cursor->getPosition().X;
		s32 MouseXPrev = MouseX;

		IVideoDriver* driver = device->getVideoDriver();
		IGUIEnvironment* gui = device->getGUIEnvironment();
		ISceneManager* smgr = device->getSceneManager();

		IGUIImage* unlock_inside = gui->addImage(driver->getTexture("media/gui/unlock_inside.png"), vector2di(85, 53));
		unlock_inside->setMaxSize(dimension2du(p.UnlockGUIValueUpdate(cowsXp), 10));
		IGUIImage* health_inside = gui->addImage(driver->getTexture("media/gui/healthbar_inside.png"), vector2di(85, 13));
		health_inside->setMaxSize(dimension2du(HEALTH_GUI_SIZE_X, 10));
		IGUIImage* heat_inside = gui->addImage(driver->getTexture("media/gui/heat_inside.png"), vector2di(85, 33));
		health_inside->setMaxSize(dimension2du(HEALTH_GUI_SIZE_X, 10));
		IGUIImage* cow_icon = gui->addImage(driver->getTexture("media/gui/cow_icon.png"), vector2di(driver->getViewPort().getWidth() - 130, 10));
		cow_icon->setMaxSize(dimension2du(64, 64));
		IGUIImage* alien_icon = gui->addImage(driver->getTexture("media/gui/alien_icon.png"), vector2di(14, 10));
		cow_icon->setMaxSize(dimension2du(64, 64));
		IGUIImage* unlock_outside = gui->addImage(driver->getTexture("media/gui/healthbar_outside.png"), vector2di(80, 50));
		unlock_outside->setMaxSize(dimension2du(170, 15));
		IGUIImage* health_outside = gui->addImage(driver->getTexture("media/gui/healthbar_outside.png"), vector2di(80, 10));
		health_outside->setMaxSize(dimension2du(170, 15));
		IGUIImage* heat_outside = gui->addImage(driver->getTexture("media/gui/healthbar_outside.png"), vector2di(80, 30));
		health_outside->setMaxSize(dimension2du(170, 15));

		IGUIButton* shieldBtnToggle = gui->addButton(recti(10, 90, 10 + 32, 90 + 32));
		shieldBtnToggle->setID(234);
		shieldBtnToggle->setVisible(true);

		IGUIButton* nukeBtnToggle = gui->addButton(recti(52, 90, 52 + 32, 90 + 32));
		nukeBtnToggle->setID(235);
		nukeBtnToggle->setVisible(true);

		while (device->run())
		{
			//time
			const u32 now = device->getTimer()->getTime();
			const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
			then = now;

			if (state == STATE_GAME)
			{
				//fade back into the game
				if (cutscene3FadeOut && transition_alpha != 0){
					cutscene3FadeOut = false;
					updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
				}
					
				else
				{
					p.ShieldToggle(er.GUIShieldToggle);
					if (er.GUIShieldToggle){
						p.RemoveEnergy(frameDeltaTime);
					}

					cutscene3FadeOut = false;
					if (!cutscene4AlienOutOfShip)
					{
						cutscene4AlienMovingTowards = true;
						cutscene4PlayerPosBegin.Y = p.GetPosition().Y;
						cutscene4PlayerPosEnd.Y = p.GetPosition().Y;
						p.GetNode()->setPosition(cutscene4PlayerPosBegin);
						p.SetAnimationName("crawl_s");
						p.SetAnimationID(PLAYER_ANIMATION_CRAWL_FROM_SHIP);
						cutscene4AlienOutOfShip = true;
					}

					else if (cutscene4AlienMovingTowards){
						if (p.MoveTowards(cutscene4PlayerPosEnd, frameDeltaTime)) {
							p.LookAt(cutscene4PlayerPosEnd, 0.0f);
							cam->setTarget(p.GetPosition());
						}
						else
						{
							p.SetAnimationID(PLAYER_ANIMATION_CRAWL_WALK);
							p.SetAnimationName("crawl_walk");
							cutscene4AlienMovingTowards = false;
						}
					}

					else
					{
						GameUpdate(device, MouseX, MouseXPrev, frameDeltaTime);
						be.GetNodeDrill()->setRotation(be.GetNodeDrill()->getRotation() + vector3df(0.0f, 750.0f * frameDeltaTime, 0.0f));

						if (p.GetHealth() <= 0) {
							//play death animation
							p.SetAnimationName("crdeth");
							cam->setPosition(vector3df(999.0f));
							cowHeadGameOver->setPosition(cam->getPosition() + vector3df(0.0f, 1.8f, 0.0f));
							cowHeadGameOver->setRotation(vector3df(0.0f, 0.0f, -90.0f));
							cam->setTarget(cowHeadGameOver->getPosition());
							totalCowsKilled += cowsKilled;
							//reset boss shite
							bossScene = false;
							state = STATE_GAME_OVER;
						}

						//lightning upgrade states
						else if ((currentLightningType == 0 && cowsXpLvl == 2) || (currentLightningType == 1 && cowsXpLvl == 4)
							|| (currentLightningType == 2 && cowsXpLvl == 6) || (currentLightningType == 3 && cowsXpLvl == 8)
							|| (currentLightningType == 4 && cowsXpLvl == 10))
						{
							currentLightningType++;
							if (currentLightningType == LIGHTNING_TYPES)
								currentLightningType = LIGHTNING_TYPES - 1;
							else
							{
								//re top up all player stats
								p.SetHealth(100);
								p.SetEnergy(100);

								OldCameraPosition = cam->getPosition();
								cam->setPosition(vector3df(-7.0f, 0.0f, 4.0f));
								cam->setTarget(p.GetPosition());
								ef.SetVisible(false);
								p.GetNode()->setRotation(vector3df(0.0f, -45.0f, 0.0f));
								groundSceneNode->setVisible(false);
								ufoBladesSceneNode->setVisible(false);
								ufoSceneNode->setVisible(false);
								LightningUpgrade(device);
								state = STATE_POWERUP;
							}
						}

						//boss scene (he will always be around and never trully killed but you must keep fighting him
						//else if ((cowsKilled != 0 && (cowsKilled % 20) == 0) && !bossScene)
						else if ((cowsKilled == 0 || cowsKilled == 3) && !bossScene)
						{
							ef.SetVisible(false);
							be.GetNode()->setVisible(true);
							be.RandomPosition(12.0f, true);
							bigEnemyNewPos = be.GetPosition();
							be.GetNode()->setPosition(vector3df(be.GetNode()->getPosition().X, -10.0f, be.GetNode()->getPosition().Z));
							be.GetNodeCap()->setPosition(be.GetNode()->getPosition() + vector3df(-0.2f, 3.5f, -0.2f));
							be.GetNodeDirt()->setPosition(vector3df(be.GetNode()->getPosition().X, 0.1f, be.GetNode()->getPosition().Z));
							cam->setTarget(be.GetNode()->getPosition() + vector3df(0.0f, 15.0f, 0.0f));
							be.LookAt(p.GetPosition(), 180.0f);
							be.SetHealth(BASE_BOSS_HEALTH);
							bossScene = true;
							bigEnemyFirstMove = false;
							bigEnemyWalkOutCap = false;
							bigEnemyOnMove = false;
							bigEnemyCapOutOfRange = false;
							bigEnemyStopShooting = false;
							bigEnemyCapVelocity = -1.0f;
						}

						else if (bossScene)
						{
							//if its the intro sequence of the big enemy, make it fade and show him climbinmg out the ground
							if (!bigEnemyFirstMove && be.MoveTowards(be.GetCachedSpawnPosition(), frameDeltaTime, true)) {
								vector3df p1 = (p.GetPosition() - cam->getPosition()).normalize() * (ZOOM_INTO_BOSS_SPEED * frameDeltaTime);
								cam->setPosition(cam->getPosition() + p1);
								//log the current pos (which will be the final on the next step)
								bigEnemyOldPos = be.GetPosition();
							}
							else
							{
								bigEnemyFirstMove = true;

								//heads towards the centre (the ufo is the middle)
								//BECAREFUL OF THE Y AXIS!!!!!!!!!
								if (!bigEnemyWalkOutCap){
									bigEnemyWalkOutCap = !be.MoveTowards((ufoSceneNode->getPosition() - bigEnemyOldPos).normalize(), frameDeltaTime, false);
									bigEnemyMoveCounter += 1.0f * frameDeltaTime;

									if (bigEnemyMoveCounter > bigEnemyMoveMax) {
										bigEnemyMoveCounter = 0.0f;
										bigEnemyWalkOutCap = true;
									}
								}

								else if (bigEnemyWalkOutCap && !bigEnemyOnMove && be.PollNewPosition(frameDeltaTime)) {
									bigEnemyNewPos = be.RandomPosition(12.0f, false);
									be.LookAt(bigEnemyNewPos, 180.0f);
									bigEnemyOnMove = true;
								}

								else if (bigEnemyOnMove){
									bigEnemyOnMove = be.MoveTowards(bigEnemyNewPos, frameDeltaTime, false);
									if (!bigEnemyOnMove) {
										bigEnemyStopShooting = false;
										be.LookAt(p.GetPosition(), 180.0f);
									}
								}
								
								if (bigEnemyWalkOutCap && !bigEnemyCapOutOfRange)
								{
									//shoot the capsule off out of the scene
									be.GetNodeCap()->setPosition(be.GetNodeCap()->getPosition() + vector3df(0.0f, (bigEnemyCapVelocity *
										(bigEnemyMoveCounter - bigEnemyCapsuleTakeoff)) * frameDeltaTime, 0.0f));
									bigEnemyCapVelocity -= 50.0f * frameDeltaTime;
									if (be.GetNodeCap()->getPosition().Y > 100.0f)
										bigEnemyCapOutOfRange = true;
								}
								
								cam->setPosition(bossFightCamPos);
								cam->setTarget(p.GetPosition());
							}
						}

						//when boss fight is done, go back to original cam
						else
						{
							if (bossDead){
								enemyOrb.GetNode()->setPosition(vector3df(999.0f));
								be.GetNodeDirt()->setPosition(vector3df(-9.99f));
								bossDead = false;
							}

							vector3df p1 = (defaultCamPos - cam->getPosition()).normalize() * (ZOOM_INTO_BOSS_DEAD_SPEED * frameDeltaTime);
							cam->setPosition(cam->getPosition() + p1);
							cam->setTarget(p.GetPosition());
						}
					}
				}
				
			}
				
			else if (state == STATE_INTRO_CUTSCENE)
			{
				if (ufoSceneNode->getPosition().Y < 0.0f){
					CutsceneUnload(device);
					state = STATE_GAME;
					GameInit(device);
				}

				else
					CutsceneUpdate(device, frameDeltaTime);
			}

			else if (state == STATE_MENU) {
				if (er.GetMouseState().LeftButtonDown) {
					CutsceneInit(device);
					state = STATE_INTRO_CUTSCENE;
				}
			}

			else if (state == STATE_GAME_OVER)
			{
				//cam->setTarget(ef.GetNearestEnemy(p)->GetPosition());
				gameOverTimer += 1.0f * frameDeltaTime;

				if (gameOverTimer > GAME_OVER_FINISH_TIME) {
					gameOverTimer = 0.0f;
					GameReset();
					state = STATE_GAME;
				}
			}

			else if (state == STATE_POWERUP)
			{
				lightningUpgradeTimer += 1.0f * frameDeltaTime;
				p.NotFiringAnimation(frameDeltaTime);
				if (lightningUpgradeTimer > lightningUpgradeWait)
				{
					lightningUpgradeTimer = 0.0f;
					cam->setPosition(OldCameraPosition);
					cam->setTarget(p.GetPosition());
					ef.SetVisible(true);
					groundSceneNode->setVisible(true);
					ufoBladesSceneNode->setVisible(true);
					ufoSceneNode->setVisible(true);
					state = STATE_GAME;
				}
			}
				
			//printf("%f, %f, %f, %f, %f, %f\n", cam->getPosition().X, cam->getPosition().Y, cam->getPosition().Z,
				//cam->getRotation().X, cam->getRotation().Y, cam->getRotation().Z);

			driver->beginScene(true, true, SColor(255, 0, 0, 0));
			smgr->drawAll();
			if (state == STATE_MENU){
				driver->draw2DImage(menuBkg, recti(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height),
					recti(0, 0, menuBkg->getSize().Width, menuBkg->getSize().Height));
				MenuFontDraw(device);
			}

			else if (state == STATE_INTRO_CUTSCENE) {
				if (!cutscene3FadeOut && transition_alpha != 0)
					updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
				else if (cutscene3FadeOut)
					updateFadeOut(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
			}

			else if (state == STATE_GAME_OVER)
			{
				cowHeadGameOver->setRotation(cowHeadGameOver->getRotation() + vector3df(8.0f * frameDeltaTime, 0.0f, 0.0f));
				//if (gameOverTimer > GAME_OVER_FADE_OUT_TIME)
				//	updateFadeOut(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());

				//if (gameOverTimer > GAME_OVER_FINISH_TIME)
				//	updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
			}

			if (state == STATE_GAME || state == STATE_GAME_OVER)
			{
				if (state == STATE_GAME_OVER) {
					dimension2du s = device->getVideoDriver()->getScreenSize();
					stringw str = L"Cows Destroyed: ";
					str += cowsKilled;
					font->draw(str.c_str(), core::rect<s32>(s.Width / 2 + 200, s.Height / 2, 0, 0), video::SColor(255, 255, 255, 255));
					str = L"Total Cows Destroyed: ";
					str += totalCowsKilled;
					font->draw(str.c_str(), core::rect<s32>(s.Width / 2 + 200, s.Height / 2 + 30, 0, 0), video::SColor(255, 255, 255, 255));
				}

				else
				{
					health_inside->setMaxSize(dimension2du(p.HealthGUIValueUpdate(), 10));
					heat_inside->setMaxSize(dimension2du(p.EnergyGUIValueUpdate(), 10));
					health_outside->draw();
					if (p.GetHealth() > 0)
						health_inside->draw();
					heat_outside->draw();
					if (p.GetEnergy() > 0)
						heat_inside->draw();
					if (cowsXp > 100) {
						shieldBtnToggle->setVisible(true);
						cowsXp = 0;
						cowsXpLvl += 1;
					}

					unlock_inside->setMaxSize(dimension2du(p.UnlockGUIValueUpdate(cowsXp), 10));
					unlock_outside->draw();
					unlock_inside->draw();
					shieldBtnToggle->draw();
					nukeBtnToggle->draw();
					cow_icon->draw();
					alien_icon->draw();
					HighScoreFontDraw(device, cowsKilled);
				}
			}
			
			driver->endScene();
		}
	}

	//cutsceneLightning->drop();
	//cutsceneLightning = 0;
	device->drop();
	return 0;
}