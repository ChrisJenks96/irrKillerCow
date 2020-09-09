/** Example 027 Helloworld_Android
	This example shows a simple application for Android.
*/

#include <irrlicht.h>
#include "Player.h"
#include "Enemy.h"
#include <time.h>
#include "LightningSceneNode.h"

#include "Cutscene.h"

#ifdef _IRR_ANDROID_PLATFORM_

#include <android_native_app_glue.h>
#include "android_tools.h"
#include "android/window.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

SEvent fakeMouseEvent;
bool HoldingLeftDown = false;
IGUIButton* shieldBtnToggle;
IGUIButton* nukeBtnToggle;
bool GUINukeToggle{ false };
bool GUIShieldToggle{ false };

JNIEnv* jni = 0;

irr::android::SDisplayMetrics displayMetrics;

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(android_app* app )
	: Device(0), AndroidApp(app), SpriteToMove(0), TouchID(-1)
	{
        fakeMouseEvent.MouseInput.Event = EMIE_COUNT;
	}

	void Init(IrrlichtDevice *device)
	{
		Device = device;
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_TOUCH_INPUT_EVENT)
		{
			/*
				For now we fake mouse-events. Touch-events will be handled inside Irrlicht in the future, but until
				that is implemented you can use this workaround to get a GUI which works at least for simple elements like
				buttons. That workaround does ignore multi-touch events - if you need several buttons pressed at the same
				time you have to handle that yourself.
			*/
			fakeMouseEvent.EventType = EET_MOUSE_INPUT_EVENT;
			fakeMouseEvent.MouseInput.X = event.TouchInput.X;
			fakeMouseEvent.MouseInput.Y = event.TouchInput.Y;
			fakeMouseEvent.MouseInput.Shift = false;
			fakeMouseEvent.MouseInput.Control = false;
			fakeMouseEvent.MouseInput.ButtonStates = 0;
			fakeMouseEvent.MouseInput.Event = EMIE_COUNT;

			switch (event.TouchInput.Event)
			{
				case ETIE_PRESSED_DOWN:
				{
					// We only work with the first for now.force opengl error
					if ( TouchID == -1 )
					{
						fakeMouseEvent.MouseInput.Event = EMIE_LMOUSE_PRESSED_DOWN;
                        if (shieldBtnToggle->isVisible() && fakeMouseEvent.MouseInput.X > 10 && fakeMouseEvent.MouseInput.X < 10 + 48 &&
                                fakeMouseEvent.MouseInput.Y > 108 && fakeMouseEvent.MouseInput.Y < 108 + 48) {
                            shieldBtnToggle->setVisible(false);
                            GUIShieldToggle = true;
                        }
                        else if (nukeBtnToggle->isVisible() && fakeMouseEvent.MouseInput.X > 64 && fakeMouseEvent.MouseInput.X < 64 + 48 &&
                            fakeMouseEvent.MouseInput.Y > 108 && fakeMouseEvent.MouseInput.Y < 108 + 48){
                            nukeBtnToggle->setVisible(false);
                            GUINukeToggle = true;
                        }
					}
					break;
				}
				case ETIE_MOVED:
                    HoldingLeftDown = true;
					if ( TouchID == event.TouchInput.ID )
					{
						fakeMouseEvent.MouseInput.Event = EMIE_MOUSE_MOVED;
						fakeMouseEvent.MouseInput.ButtonStates = EMBSM_LEFT;
					}
					break;
				case ETIE_LEFT_UP:
                    HoldingLeftDown = false;
					if ( TouchID == event.TouchInput.ID )
                    {
						fakeMouseEvent.MouseInput.Event = EMIE_LMOUSE_LEFT_UP;
						if ( SpriteToMove )
						{
							TouchID = -1;
							position2d<s32> touchPoint(event.TouchInput.X, event.TouchInput.Y);
							MoveSprite(touchPoint);
							SpriteToMove = 0;
						}
					}
					break;
			    default:
					break;
			}

			if ( fakeMouseEvent.MouseInput.Event != EMIE_COUNT && Device )
			{
				Device->postEventFromUser(fakeMouseEvent);
			}
		}
		else if ( event.EventType == EET_GUI_EVENT )
		{
			/*
				Show and hide the soft input keyboard when an edit-box get's the focus.
			*/
			switch(event.GUIEvent.EventType)
			{
				default:
					break;
			}
		}

		return false;
	}

	void MoveSprite(const irr::core::position2d<irr::s32> &touchPos)
	{
		irr::core::position2d<irr::s32> move(touchPos-TouchStartPos);
		SpriteToMove->setRelativePosition(SpriteStartRect.UpperLeftCorner + move);
	}

private:
	IrrlichtDevice * Device;
    android_app* AndroidApp;
	gui::IGUIElement * SpriteToMove;
	core::rect<s32> SpriteStartRect;
	core::position2d<irr::s32> TouchStartPos;
	s32 TouchID;
};

#include "Player.h"
#include "Enemy.h"
#include <time.h>
#include "LightningSceneNode.h"
#include <fmod.hpp>

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

#define MAX_COWS 15
#define STARTING_ENEMIES 4

//MUSIC RELATED STUFF
FMOD::System* FMODSystem;
FMOD::Channel* channel = 0;
FMOD::Channel* channel_bkg = 0;

FMOD_MODE currMode;
FMOD::Sound* mainMenuMusic;
FMOD::Sound* backgroundMusic;
FMOD::Sound* lightningEffectStart;
FMOD::Sound* lightningEffectMid;
FMOD::Sound* lightningEffectEnd;
FMOD::Sound* lightningCutsceneOnce;

bool lightningEffectStartTrigger = false;
bool lightningEffectMidTrigger = false;
bool lightningEffectEndTrigger = false;

float lightningWait = 0.0f;
#define LIGHTNING_WAIT_MAX 0.7f

//system stuff
ICameraSceneNode* cam;
IMeshSceneNode* cutsceneGroundSceneNode[2];
IAnimatedMeshSceneNode* groundSceneNode;
IMeshSceneNode* ufoSceneNode;
IMeshSceneNode* ufoBladesSceneNode;
IMeshSceneNode* cowHeadGameOver;
ILightSceneNode* dirLight;
ILightSceneNode* ufoSpotlight;
IMeshSceneNode* earthSceneNode;
ILightSceneNode* introCutsceneLight;
vector3df intersectPoint = vector3df(0.0f);
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
int beKilled = 0;

#define ENEMY_ORB_DEFAULT_SPEED 10
float enemyOrbSpeed = ENEMY_ORB_DEFAULT_SPEED;

float gameOverResetTimer = 0.0f;
float gameOverResetRate = 4.0f;

int perkCount = 0;
#define NUKE_CHANCE 3
#define SHIELD_CHANCE 0

float shieldTimer = 0.0f;
float shieldRate = 3.5f;

vector3df OldCameraPosition;

//game specifics
Player p;
EnemyFactory* ef;
BigEnemy* be;

bool gameOverToReset = false;
bool playerNukeGoneOff = false;
bool globalPlayerMunchFlag = false;
int totalCowsKilled = 0;
int savedCowsKilled = 0;
int cowsKilled = 0;
float cowsXp = 0.0f;
int cowsXpLvl = 1;
float xpMod = 4.8f;
float lightningUpgradeTimer = 0.0f;
float lightningUpgradeWait = 3.0f;
IGUIFont* font;
IGUISkin* skin;

//cutscene specifics
int currentCutscene = 0;
float gameOverTimer = 0.0f;
#define GAME_OVER_FADE_OUT_TIME 3.0f
#define GAME_OVER_FINISH_TIME 1.0f

static void SaveLoadGame(bool s)
{
    FILE* f = NULL;
    if (s)
    {
        if (savedCowsKilled < cowsKilled) {
            f = fopen("/data/data/com.albongames.killercowapp3/files/HS.DAT", "wb");
            if (f){
                fwrite(&cowsKilled, 4, 1, f);
                savedCowsKilled = cowsKilled;
                fclose(f);
            }
        }
    }

    else
    {
        f = fopen("/data/data/com.albongames.killercowapp3/files/HS.DAT", "rb");
        if (f) {
            fread(&savedCowsKilled, 4, 1, f);
            fclose(f);
        }

        else
        {
            f = fopen("/data/data/com.albongames.killercowapp3/files/HS.DAT", "wb");
            if (f){
                cowsKilled = 0;
                fwrite(&cowsKilled, 4, 1, f);
                savedCowsKilled = cowsKilled;
                fclose(f);
            }
        }
    }
}

static void StaticMeshesLoad(IrrlichtDevice* device)
{
    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();

    //loading in the ground

    //IReadFile* file = createMemoryReadFile(mmemory, msize, mfilename, deleteMemoryWhenDropped);

    IAnimatedMesh* mesh = smgr->getMesh("media/base_plane/base_plane.obj");
    if (mesh)
    {
        groundSceneNode = smgr->addAnimatedMeshSceneNode(mesh);
        groundSceneNode->setScale(vector3df(2.0f));

        if (groundSceneNode)
        {
            //groundSceneNode->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
            groundSceneNode->setMaterialFlag(EMF_LIGHTING, true);
            groundSceneNode->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
            groundSceneNode->setVisible(false);

            groundSceneNode->getMaterial(1).setTexture(0, driver->getTexture("media/base_plane/dirt.png"));
            groundSceneNode->getMaterial(0).setTexture(0, driver->getTexture("media/base_plane/grass_dirt.png"));
            groundSceneNode->getMaterial(0).getTextureMatrix(0).setScale(18.0f);
            groundSceneNode->getMaterial(2).setTexture(0, driver->getTexture("media/base_plane/hay.png"));
            groundSceneNode->getMaterial(3).setTexture(0, driver->getTexture("media/base_plane/grass1.png"));

            scene::ITriangleSelector* selector = 0;
            selector = smgr->createTriangleSelector(groundSceneNode);
            groundSceneNode->setTriangleSelector(selector);
            selector->drop();
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
                cutsceneGroundSceneNode[i]->getMaterial(0).setTexture(0, driver->getTexture("media/base_plane/grass1.png"));
                cutsceneGroundSceneNode[i]->getMaterial(1).setTexture(0, driver->getTexture("media/base_plane/hay.png"));
                cutsceneGroundSceneNode[i]->getMaterial(2).setTexture(0, driver->getTexture("media/base_plane/grass_dirt.png"));
                cutsceneGroundSceneNode[i]->getMaterial(2).getTextureMatrix(0).setScale(vector3df(12.0f, 24.0f, 0.0f));
            }
        }

        cutsceneGroundSceneNode[0]->setVisible(false);
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
            ufoSceneNode->setVisible(false);
            ufoSceneNode->getMaterial(0).setTexture(0, driver->getTexture("media/ufo/exhaust.png"));
            ufoSceneNode->getMaterial(0).Shininess = 60;
            ufoSceneNode->getMaterial(1).setTexture(0, driver->getTexture("media/ufo/exhaust.png"));
            ufoSceneNode->getMaterial(1).Shininess = 60;
            ufoSceneNode->getMaterial(2).setTexture(0, driver->getTexture("media/ufo/seat.png"));
            ufoSceneNode->getMaterial(3).setTexture(0, driver->getTexture("media/ufo/pod.png"));
            ufoSceneNode->getMaterial(4).setTexture(0, driver->getTexture("media/ufo/body.png"));
            ufoSceneNode->getMaterial(4).Shininess = 45;
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
        ufoBladesSceneNode->setVisible(false);

        if (ufoBladesSceneNode)
        {
            ufoBladesSceneNode->setMaterialFlag(EMF_LIGHTING, true);
            ufoBladesSceneNode->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
            ufoBladesSceneNode->getMaterial(0).setTexture(0, driver->getTexture("media/ufo/blades.png"));
        }

        ufoBladesSceneNode->setParent(ufoSceneNode);
        ufoBladesSceneNode->setPosition(vector3df(0.0f, -1.0f, 0.0f));
    }

    mesh = smgr->getMesh("media/cow/cow_head.obj");
    if (mesh)
    {
        cowHeadGameOver = smgr->addMeshSceneNode(mesh);
        cowHeadGameOver->setScale(vector3df(1.25f));
        cowHeadGameOver->setVisible(true);

        if (cowHeadGameOver)
        {
            cowHeadGameOver->setMaterialFlag(EMF_LIGHTING, false);
            cowHeadGameOver->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
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
    earthSceneNode->setVisible(false);
    //main ligth for the game
    dirLight = device->getSceneManager()->addLightSceneNode(0, vector3df(0.0f, 10.0f, 0.0f), SColorf(0.0f, 0.85f, 0.85f, 1.0f), 0.0f);
    dirLight->getLightData().Type = ELT_SPOT;
    dirLight->setRotation(vector3df(60.0f, 0.0f, 0.0f)); //default is (1,1,0) for directional lights

    introCutsceneLight->getLightData().DiffuseColor = SColor(255, 140, 140, 140);

    //load the non important static meshes for the scene with no behaviour

    //ENABLE ALL VISIBILITY HERE....
    ufoSceneNode->setVisible(true);
    ufoBladesSceneNode->setVisible(true);
    cutsceneGroundSceneNode[0]->setVisible(true);

    ufoSceneNode->setPosition(vector3df(0.0f, CUTSCENE_UFO_HEIGHT, 0.0f));
    ufoSceneNode->setRotation(vector3df(0.0f, -90.0f, 0.0f));
    //ufoBladesSceneNode->setPosition(vector3df(0.0f, 40.0f, 0.0f));
    ufoBladesSceneNode->setRotation(vector3df(0.0f, -90.0f, 0.0f));
    cam->setPosition(cutscene1CamPosition);
    cam->setTarget(ufoSceneNode->getPosition());

    FMODSystem->createSound("file:///android_asset/media/music/Lightning_Once.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, 0, &lightningCutsceneOnce);
}

void CutsceneUnload(IrrlichtDevice* device)
{
    cutsceneGroundSceneNode[0]->setVisible(false);
    cutsceneGroundSceneNode[1]->setVisible(false);
    groundSceneNode->setVisible(true);
    lightningCutsceneOnce->release();
    lightningCutsceneOnce = 0;
}

void CutsceneUpdate(IrrlichtDevice* device, const float dt)
{
    dirLight->setPosition(ufoSceneNode->getPosition() + vector3df(0.0f, 20.0f, 0.0f));
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
        ufoBladesSceneNode->setRotation(ufoBladesSceneNode->getRotation() + vector3df(0.0f, 2250.0f * dt, 0.0f));

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
            //play lightning strike sound effect
            bool lightningOnceFlag;
            channel->isPlaying(&lightningOnceFlag);
            if (!lightningOnceFlag) {
                channel->setMode(FMOD_LOOP_OFF);
                FMODSystem->playSound(lightningCutsceneOnce, 0, false, &channel);
                channel->setVolume(0.6f);
            }
        }

        if (cutscenespeedAccum > CUTSCENE2_END)
        {
            cutscene3CrashPosition = ufoSceneNode->getPosition() + vector3df(0.0f, 0.0f, CUTSCENE3_CRASH_AHEAD_DISTANCE);
            cutscene3CrashPosition.Y = -10.0f;
            cutsceneLightning->setVisible(false);
            currentCutscene = 2;

            bool bkgMusicPlaying;
            channel_bkg->isPlaying(&bkgMusicPlaying);
            if (!bkgMusicPlaying) {
                channel_bkg->setMode(FMOD_LOOP_NORMAL);
                FMODSystem->playSound(backgroundMusic, 0, false, &channel_bkg);
                channel_bkg->setVolume(0.6f);
            }
        }
    }

    else if (currentCutscene == 2)
    {
        introCutsceneLight->getLightData().DiffuseColor = SColor(255, 18, 18, 18);
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

void LightningUpgrade(IrrlichtDevice* device)
{
    cutsceneLightning->getMaterial(0).setTexture(0, device->getVideoDriver()->getTexture(lightning_types[currentLightningType].texture));
    p.ShieldTexture(lightning_types[currentLightningType].shield_texture, device->getVideoDriver());
    p.SetEnergyDepleteRate(lightning_types[currentLightningType].energyDepleteRate);
    p.SetEnergyRestoreRate(lightning_types[currentLightningType].energyRestoreRate);
    shieldRate = 3.5f * (currentLightningType + 1);
    p.LightningChangeCol(lightning_types[currentLightningType].col);
}

void GameReset()
{
    globalPlayerMunchFlag = false;
    cowsXp = 0;
    cowsXpLvl = 0;
    cowsKilled = 0;
    perkCount = 0;
    p.SetHealth(100);
    p.SetEnergy(100);
    p.SetAnimationName("idle");
    p.SetAnimationID(PLAYER_ANIMATION_IDLE);
    p.ResetAnimationTimer();
    ef->ForceReset();
    cam->setPosition(vector3df(defaultCamPos));
    cam->setTarget(p.GetPosition());
    ef->SetEnemyCount(STARTING_ENEMIES);
}

void GameInit(IrrlichtDevice* device)
{
    SaveLoadGame(false);
    ISceneManager* smgr = device->getSceneManager();

    p = Player(device);
    ef = new EnemyFactory(device, FMODSystem, MAX_COWS, STARTING_ENEMIES);
    ef->SetEnemyCount(STARTING_ENEMIES);
    be = new BigEnemy(device, FMODSystem, 12.0f);
    enemyOrb = EnemyOrb(device);
    enemyOrb.GetNode()->setPosition(vector3df(-999.0f));
    be->GetNode()->setVisible(false);

    ufoSceneNode->setPosition(vector3df(-2.0f, -4.0f, 5.0f));
    ufoSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
    ufoBladesSceneNode->setRotation(vector3df(0.0f, 180.0f, 15.0f));
    groundSceneNode->setPosition(vector3df(0.0f, -1.0f, 0.0f));

    QUAD_SEGMENT_INCREMENT = -10.0f;
    cutsceneLightning->setParent(p.GetNode());
    cutsceneLightning->setPosition(vector3df(-0.65f, 0.8f, 1.3f));
    cutsceneLightning->setScale(vector3df(LIGHTNING_SCALE));
    cutsceneLightning->setRotation(vector3df(-90.0f, 0.0f, 90.0f));

    LightningUpgrade(device);

    cam->setPosition(defaultCamPos);
    cam->setTarget(p.GetPosition());

    dirLight->remove();
    dirLight = smgr->addLightSceneNode(0, vector3df(0.0f, 80.0f, 0.0f), SColorf(0.0f, 0.4f, 0.4f, 1.0f), 160.0f);
    dirLight->getLightData().Type = video::ELT_SPOT;
    dirLight->getLightData().InnerCone = 30.0f;
    dirLight->getLightData().OuterCone = 100.0f;
    dirLight->getLightData().Falloff = 20.0f;
    dirLight->setRotation(vector3df(90.0f, 0.0f, 0.0f));
}

void GameUpdate(IrrlichtDevice* device, s32& MouseX, s32& MouseXPrev, const float& frameDeltaTime)
{
    //rotate the player around
    if (p.GetHealth() > 0)
    {
        intersectPoint = getSceneNodeFromScreenCoordinatesBB(device->getSceneManager(), device->getVideoDriver(), groundSceneNode->getTriangleSelector(), vector2d<s32>(fakeMouseEvent.MouseInput.X, fakeMouseEvent.MouseInput.Y), 0);
        const vector3df toTarget = intersectPoint - p.GetNode()->getPosition();
        p.GetNode()->setRotation((toTarget.getHorizontalAngle() - 4.0f) * vector3df(0.0f, 1.0f, 0.0f));

        //THIS CAN BE JOYSTICK CODE... DO NOT DELETE!!!!!!!!!

        //MouseX = er.GetMouseState().Position.X;
        //s32 MouseXDiff = MouseX - MouseXPrev;
        //p.GetNode()->setRotation(p.GetNode()->getRotation() + vector3df(0.0f, (MouseXDiff * (100.0f * frameDeltaTime)), 0.0f));
        //MouseXPrev = MouseX;
    }

    enemyOrb.Update(frameDeltaTime, true);
    if (bigEnemyCapOutOfRange)
    {
        if (!bigEnemyStopShooting) {
            vector3df newPos = ((p.GetPosition() - enemyOrb.GetNode()->getPosition()).normalize()) * enemyOrbSpeed * frameDeltaTime;
            enemyOrbSpeed += 2.0f * frameDeltaTime;
            float dist = (p.GetPosition() - enemyOrb.GetNode()->getPosition()).getLengthSQ();
            enemyOrb.GetNode()->setPosition(enemyOrb.GetNode()->getPosition() + newPos);
            if (dist < 0.2f && !GUIShieldToggle) {
                enemyOrb.GetNode()->setPosition(be->GetPosition());
                p.RemoveHealth(be->GetAttackDamage());
                if (bigEnemyOnMove)
                    bigEnemyStopShooting = true;
            }

            else if (dist < 2.8f && GUIShieldToggle) {
                enemyOrb.GetNode()->setPosition(be->GetPosition());
                if (bigEnemyOnMove)
                    bigEnemyStopShooting = true;
            }
        }

        else
            enemyOrb.GetNode()->setPosition(be->GetNode()->getPosition() + vector3df(0.0f, 4.0f, 0.4));
    }

        //the enemy entrance etc...
    else
        enemyOrb.GetNode()->setPosition(be->GetNode()->getPosition() + vector3df(0.0f, 4.0f, 0.4));

    if (GUINukeToggle)
    {
        if (!playerNukeGoneOff) {
            p.SetEnergy(p.GetEnergy() - 50);
            p.GetOrb()->setVisible(true);
            playerNukeGoneOff = true;
            GUINukeToggle = false;
        }
    }

    //firing state for the player
    bool leftPressedMidEffect = false;
    //if (er.GetMouseState().LeftButtonDown) {
      if (HoldingLeftDown){
        if (p.GetEnergy() > 0)
        {
            if (lightningEffectMidTrigger) {
                leftPressedMidEffect = true;
                channel->getMode(&currMode);
                if (currMode != FMOD_LOOP_NORMAL) {
                    channel->setMode(FMOD_LOOP_NORMAL);
                    channel->setLoopPoints(20, FMOD_TIMEUNIT_MS, 750, FMOD_TIMEUNIT_MS);
                }

            }

            else if (!lightningEffectStartTrigger) {
                bool playing;
                channel->isPlaying(&playing);
                if (!playing){
                    channel->setMode(FMOD_LOOP_OFF);
                    FMODSystem->playSound(lightningEffectStart, 0, false, &channel);
                    channel->setVolume(0.3f);
                    channel->setPaused(false);
                    lightningEffectStartTrigger = true;
                }

            }

            p.FiringAnimation(frameDeltaTime);
            ISceneNode* e = p.Fire(device, 25.0f * (p.GetEnergy() / 100.0f));
            lightningWait += 1.0f * frameDeltaTime;
            if (lightningWait > 0.7f) {
                p.RemoveEnergy(frameDeltaTime);
                QUAD_SEGMENT_INCREMENT = -10.0f;
                cutsceneLightning->ArkUpdate(frameDeltaTime);
                float lightningLength = LIGHTNING_SCALE * ((float)p.GetEnergy() / 100.0f);
                cutsceneLightning->setScale(vector3df(lightningLength));
                cutsceneLightning->setVisible(true);
                if (e != NULL) {
                    if (e->getID() == 667)
                    {
                        be->RemoveHealth(lightning_types[currentLightningType].damage, frameDeltaTime);
                        if (be->GetHealth() <= 0) {
                            if (!bossDead) {
                                cam->setTarget(p.GetPosition());
                                be->SetAnimationID(BIG_BOSS_ANIM_DEATH);
                            }

                            bossDead = true;
                            be->Reset();
                        }
                    }
                    else
                    {
                        Enemy* enemy = ef->FindEnemy(e);
                        if (enemy != NULL) {
                            enemy->RemoveHealth(lightning_types[currentLightningType].damage, frameDeltaTime);
                            enemy->GetNode()->getMaterial(0).EmissiveColor = SColor(255, 255, 0, 0);
                            if (enemy->GetHealth() <= 0) {
                                if (!enemy->isDeathAnimationTrigger()) {
                                    cowsXp += ((float)enemy->GetAttackDamage() / 10) * xpMod;
                                    cowsKilled += 1;
                                }

                                enemy->SetDeathAnimationTrigger(true);
                            }
                        }
                    }
                }
            }
        }
    }

    else
    {
        lightningWait = 0.0f;
        p.NotFiringAnimation(frameDeltaTime);
        p.Idle();
        cutsceneLightning->setVisible(false);
        if (p.GetEnergy() <= 100)
            p.AddEnergy(frameDeltaTime);
        ef->ResetEmission();
    }

    if (lightningEffectStartTrigger) {
        channel->isPlaying(&lightningEffectStartTrigger);
        if (!lightningEffectStartTrigger) {
            channel->stop();
            lightningEffectMidTrigger = true;
            channel->setMode(FMOD_LOOP_NORMAL);
            FMODSystem->playSound(lightningEffectMid, 0, false, &channel);
            channel->setVolume(0.3f);
        }
    }

    else if (!leftPressedMidEffect  && lightningEffectMidTrigger) {
        channel->stop();
        channel->setMode(FMOD_LOOP_OFF);
        FMODSystem->playSound(lightningEffectEnd, 0, false, &channel);
        channel->setVolume(0.3f);
        lightningEffectMidTrigger = false;
        lightningEffectEndTrigger = true;
    }

    else if (lightningEffectEndTrigger) {
        channel->isPlaying(&lightningEffectEndTrigger);
        if (!lightningEffectEndTrigger) {
            channel->stop();
        }
    }

    //if we have no energy, shut it off
    if (p.GetEnergy() > 0)
        p.ShieldUVScroll(frameDeltaTime);
    else if (p.GetEnergy() <= 0) {
        p.SetEnergy(0);
        shieldBtnToggle->setVisible(false);
        GUIShieldToggle = false;
    }

    //rotate the blades around the craft
    ufoBladesSceneNode->setRotation(ufoBladesSceneNode->getRotation() + vector3df(0.0f, 25.0f * frameDeltaTime, 0.0f));
    ef->Update(p, FMODSystem, GUIShieldToggle, cowsKilled, frameDeltaTime);

    if (ef->isPlayerGettingMunched() && !globalPlayerMunchFlag){
        globalPlayerMunchFlag = true;
        p.GetNode()->getMaterial(0).EmissiveColor = SColor(255, 255, 0, 0);
    }

    else if (globalPlayerMunchFlag){
        globalPlayerMunchFlag = false;
        ef->SetPlayerGettingMunched(false);
        p.ResetEmission();
    }
}

void MenuInit(IrrlichtDevice* device)
{
    /*skin = device->getGUIEnvironment()->getSkin();
    font = device->getGUIEnvironment()->getFont("media/gui/myfont0.bmp");
    if (font)
        skin->setFont(font);*/
    IGUIEnvironment* guienv = device->getGUIEnvironment();
    skin = guienv->getSkin();
    font = 0;
    //if ( displayMetrics.xdpi < 100 )	// just guessing some value where fontsize might start to get too small
        font = guienv->getFont("fonthaettenschweiler.bmp");
    //else
       // font = guienv->getFont("bigfont.png");
    if (font) {
        skin->setFont(font);
    }
}

void MenuFontDraw(IrrlichtDevice* device)
{
    stringw str = L"Click to Start Game";
    dimension2du s = device->getVideoDriver()->getScreenSize();
    font->draw(str.c_str(), core::rect<s32>(s.Width, s.Height - 40, 0, 0), video::SColor(255, 255, 255, 255), true);
}

void HighScoreFontDraw(IrrlichtDevice* device, const int cowsMuggedOff)
{
    stringw str = L"X ";
    str += cowsMuggedOff;
    dimension2du s = device->getVideoDriver()->getScreenSize();
    font->draw(str.c_str(), core::rect<s32>((s.Width * 2) - 160, 32, 0, 0), video::SColor(255, 255, 255, 255), true);
}

/* Main application code. */
void android_main(android_app* app)
{
    // Make sure glue isn't stripped.
    app_dummy();

    /*
        The receiver can already receive system events while createDeviceEx is called.
        So we create it first.
    */

    MyEventReceiver receiver(app);

    /*
        Create the device.
        You have currently the choice between 2 drivers:
        EDT_OGLES1 is basically a opengl fixed function pipeline.
        EDT_OGLES2 is a shader pipeline. Irrlicht comes with shaders to simulate
                   typical fixed function materials. For this to work the
                   corresponding shaders from the Irrlicht media/Shaders folder are
                   copied to the application assets folder (done in the Makefile).
    */
    SIrrlichtCreationParameters param;
    param.DriverType = EDT_OGLES1;                // android:glEsVersion in AndroidManifest.xml should be "0x00010000" (requesting 0x00020000 will also guarantee that ES1 works)
	//param.DriverType = EDT_OGLES2;				// android:glEsVersion in AndroidManifest.xml should be "0x00020000"
    param.WindowSize = dimension2d<u32>(0,
                                        0);    // using 0,0 it will automatically set it to the maximal size
    param.PrivateData = app;
    param.Bits = 24;
    param.ZBufferBits = 16;
    param.AntiAlias = 0;
    param.EventReceiver = &receiver;

    /* Logging is written to a file. So your application should disable all logging when you distribute your
       application or it can fill up that file over time.
    */
#ifndef _DEBUG
    param.LoggingLevel = ELL_NONE;
#endif

    /* initialize random seed: */
    srand(time(NULL));

    IrrlichtDevice *device = createDeviceEx(param);
    if (device == 0)
        return;

    receiver.Init(device);

//	ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, 0);

    IVideoDriver *driver = device->getVideoDriver();
    ISceneManager *smgr = device->getSceneManager();
    IGUIEnvironment *gui = device->getGUIEnvironment();
    ILogger *logger = device->getLogger();
    IFileSystem *fs = device->getFileSystem();

    /* Access to the Android native window. You often need this when accessing NDK functions like we are doing here.
       Note that windowWidth/windowHeight have already subtracted things like the taskbar which your device might have,
       so you get the real size of your render-window.
    */
    ANativeWindow *nativeWindow = static_cast<ANativeWindow *>(driver->getExposedVideoData().OGLESAndroid.Window);
    int32_t windowWidth = ANativeWindow_getWidth(app->window);
    int32_t windowHeight = ANativeWindow_getHeight(app->window);

    /* Get display metrics. We are accessing the Java functions of the JVM directly in this case as there is no NDK function for that yet.
       Checkout android_tools.cpp if you want to know how that is done. */
    memset(&displayMetrics, 0, sizeof displayMetrics);
    irr::android::getDisplayMetrics(app, displayMetrics);

    char strDisplay[1000];
    sprintf(strDisplay, "Window size:(%d/%d)\nDisplay size:(%d/%d)", windowWidth, windowHeight, displayMetrics.widthPixels, displayMetrics.heightPixels);
    logger->log(strDisplay);

    core::dimension2d<s32> dim(driver->getScreenSize());
    sprintf(strDisplay, "getScreenSize:(%d/%d)", dim.Width, dim.Height);
    logger->log(strDisplay);

    /* Your media must be somewhere inside the assets folder. The assets folder is the root for the file system.
       This example copies the media in the Android.mk makefile. */
    stringc mediaPath = "media/";

    // The Android assets file-system does not know which sub-directories it has (blame google).
    // So we have to add all sub-directories in assets manually. Otherwise we could still open the files,
    // but existFile checks will fail (which are for example needed by getFont).
    for (u32 i = 0; i < fs->getFileArchiveCount(); ++i) {
        IFileArchive *archive = fs->getFileArchive(i);
        if (archive->getType() == EFAT_ANDROID_ASSET) {
            archive->addDirectoryToFileList(mediaPath);
            break;
        }
    }

    //for fmod to work...
    app->activity->vm->AttachCurrentThread(&jni, NULL);

    FMOD_RESULT r;
    r = FMOD::System_Create(&FMODSystem);
    if (r == FMOD_OK)
    {
        r = FMODSystem->init(4 + MAX_COWS, FMOD_INIT_NORMAL, 0);
        r = FMODSystem->createSound("file:///android_asset/media/music/KillerCowOST.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0,
                                &mainMenuMusic);
        r = FMODSystem->createSound("file:///android_asset/media/music/Lightning_Effect_Start.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF,
                                0, &lightningEffectStart);
        r = FMODSystem->createSound("file:///android_asset/media/music/Lightning_Effect_Mid.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL,
                                0, &lightningEffectMid);
        r = FMODSystem->createSound("file:///android_asset/media/music/Lightning_Effect_End.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, 0,
                                &lightningEffectEnd);
        r = FMODSystem->createSound("file:///android_asset/media/music/Moron.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0,
                                &backgroundMusic);
    }
    //FMODSystem->createChannelGroup("Lightning", &channelGroupLightning);
    //FMODSystem->createChannelGroup("BKGMusic", &channelGroupBKGMusic);
    //channel->setChannelGroup(channelGroupLightning);
    //channel_bkg->setChannelGroup(channelGroupBKGMusic);

    cam = smgr->addCameraSceneNode();
    StaticMeshesLoad(device);

    IMesh *mesh = smgr->getMesh("media/gui/earth.obj");
    if (mesh) {
        earthSceneNode = smgr->addMeshSceneNode(mesh);
        if (earthSceneNode) {
            earthSceneNode->getMaterial(0).setTexture(0, driver->getTexture("media/gui/earth.png"));
            earthSceneNode->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
            earthSceneNode->setMaterialFlag(EMF_LIGHTING, true);
            earthSceneNode->setScale(vector3df(5.0f));
            earthSceneNode->setPosition(cam->getPosition() + vector3df(0.0f, 0.0f, 65.0f));
        }
    }

    introCutsceneLight = smgr->addLightSceneNode(0, vector3df(0.0f), SColor(255, 160, 160, 160),2000.0f);
    introCutsceneLight->getLightData().Type = ELT_DIRECTIONAL;
    introCutsceneLight->setRotation(vector3df(60.0f, 0.0f, 0.0f));

    int state = STATE_MENU;
    MenuInit(device);

    u32 then = device->getTimer()->getTime();
    s32 MouseX = fakeMouseEvent.MouseInput.X;
    s32 MouseXPrev = MouseX;

    //LEGALLY HAVE TO DO THIS!!! DONT REMOVE...!!!!!
    ITexture *fmod_logo = driver->getTexture("media/gui/fmod.png");
    ITexture *title_logo = driver->getTexture("media/gui/title.png");
    ITexture *ag_logo = driver->getTexture("media/gui/albon_games_logo_small.png");
    ITexture *go_logo = driver->getTexture("media/gui/go.png");

    IGUIImage *unlock_inside = gui->addImage(driver->getTexture("media/gui/unlock_inside.png"),
                                             vector2di(85, 53));
    unlock_inside->setMaxSize(dimension2du(p.UnlockGUIValueUpdate(cowsXp), 10));
    IGUIImage *health_inside = gui->addImage(driver->getTexture("media/gui/healthbar_inside.png"),
                                             vector2di(85, 13));
    health_inside->setMaxSize(dimension2du(HEALTH_GUI_SIZE_X, 10));
    IGUIImage *heat_inside = gui->addImage(driver->getTexture("media/gui/heat_inside.png"),
                                           vector2di(85, 33));
    health_inside->setMaxSize(dimension2du(HEALTH_GUI_SIZE_X, 10));
    IGUIImage *cow_icon = gui->addImage(driver->getTexture("media/gui/cow_icon.png"),
                                        vector2di(driver->getViewPort().getWidth() - 170, 10));
    cow_icon->setMaxSize(dimension2du(64, 64));
    IGUIImage *alien_icon = gui->addImage(driver->getTexture("media/gui/alien_icon.png"),
                                          vector2di(14, 10));
    cow_icon->setMaxSize(dimension2du(64, 64));
    IGUIImage *unlock_outside = gui->addImage(driver->getTexture("media/gui/healthbar_outside.png"),
                                              vector2di(80, 50));
    unlock_outside->setMaxSize(dimension2du(170, 15));
    IGUIImage *health_outside = gui->addImage(driver->getTexture("media/gui/healthbar_outside.png"),
                                              vector2di(80, 10));
    health_outside->setMaxSize(dimension2du(170, 15));
    IGUIImage *heat_outside = gui->addImage(driver->getTexture("media/gui/healthbar_outside.png"),
                                            vector2di(80, 30));
    health_outside->setMaxSize(dimension2du(170, 15));

    shieldBtnToggle = gui->addButton(recti(10, 108, 10 + 48, 108 + 48));
    shieldBtnToggle->setImage(driver->getTexture("media/gui/shield_icon.png"));
    shieldBtnToggle->setScaleImage(true);
    shieldBtnToggle->setID(234);
    shieldBtnToggle->setVisible(false);

    nukeBtnToggle = gui->addButton(recti(64, 108, 64 + 48, 108 + 48));
    nukeBtnToggle->setImage(driver->getTexture("media/gui/nuke_icon.png"));
    nukeBtnToggle->setScaleImage(true);
    nukeBtnToggle->setID(235);
    nukeBtnToggle->setVisible(false);

    while (device->run()) {
        //time
        const u32 now = device->getTimer()->getTime();
        const f32 frameDeltaTime = (f32) (now - then) / 1000.f; // Time in seconds
        then = now;

        if (state == STATE_GAME) {
            //fade back into the game
            if (cutscene3FadeOut && transition_alpha != 0) {
                cutscene3FadeOut = false;
                updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
            } else {
                p.ShieldToggle(GUIShieldToggle);
                if (GUIShieldToggle) {
                    shieldTimer += 1.0f * frameDeltaTime;
                    if (shieldTimer > shieldRate) {
                        shieldTimer = 0.0f;
                        GUIShieldToggle = false;
                        p.ShieldToggle(GUIShieldToggle);
                        shieldBtnToggle->setVisible(false);
                    }
                }

                cutscene3FadeOut = false;
                if (!cutscene4AlienOutOfShip) {
                    cutscene4AlienMovingTowards = true;
                    cutscene4PlayerPosBegin.Y = p.GetPosition().Y;
                    cutscene4PlayerPosEnd.Y = p.GetPosition().Y;
                    p.GetNode()->setPosition(cutscene4PlayerPosBegin);
                    p.SetAnimationName("crawl_s");
                    p.SetAnimationID(PLAYER_ANIMATION_CRAWL_FROM_SHIP);
                    cutscene4AlienOutOfShip = true;
                } else if (cutscene4AlienMovingTowards) {
                    if (p.MoveTowards(cutscene4PlayerPosEnd, frameDeltaTime)) {
                        p.LookAt(cutscene4PlayerPosEnd, 0.0f);
                        cam->setTarget(p.GetPosition());
                    } else {
                        p.SetAnimationID(PLAYER_ANIMATION_CRAWL_WALK);
                        p.SetAnimationName("crawl_walk");
                        cutscene4AlienMovingTowards = false;
                    }
                } else {
                    GameUpdate(device, MouseX, MouseXPrev, frameDeltaTime);
                    be->GetNodeDrill()->setRotation(be->GetNodeDrill()->getRotation() +
                                                    vector3df(0.0f, 750.0f * frameDeltaTime, 0.0f));

                    if (p.GetHealth() <= 0) {
                        //play death animation
                        p.DeathAnimation(frameDeltaTime);
                        gameOverResetTimer += 1.0f * frameDeltaTime;
                        if (gameOverResetTimer > gameOverResetRate) {
                            SaveLoadGame(true);
                            cam->setPosition(vector3df(999.0f));
                            cowHeadGameOver->setPosition(cam->getPosition() + vector3df(0.0f, 1.8f, 0.0f));
                            cowHeadGameOver->setRotation(vector3df(0.0f, 0.0f, -90.0f));
                            cam->setTarget(cowHeadGameOver->getPosition());
                            totalCowsKilled += cowsKilled;
                            //reset boss shite
                            bossScene = false;
                            bossDead = false;
                            gameOverResetTimer = 0.0f;
                            be->Reset();
                            bigEnemyStopShooting = true;
                            enemyOrb.GetNode()->setVisible(false);
                            be->GetNodeDirt()->setPosition(vector3df(-9.99f));
                            be->GetNode()->setVisible(false);
                            firstDeath = true;
                            enemyOrbSpeed = ENEMY_ORB_DEFAULT_SPEED;
                            enemyOrb.GetNode()->setPosition(vector3df(-999.0f));
                            state = STATE_GAME_OVER;
                        }
                    }

                        //lightning upgrade states
                    else if ((currentLightningType == 0 && cowsXpLvl == 2) ||
                             (currentLightningType == 1 && cowsXpLvl == 5)
                             || (currentLightningType == 2 && cowsXpLvl == 9) ||
                             (currentLightningType == 3 && cowsXpLvl == 14)
                             || (currentLightningType == 4 && cowsXpLvl == 20)) {
                        currentLightningType++;
                        if (currentLightningType == LIGHTNING_TYPES)
                            currentLightningType = LIGHTNING_TYPES - 1;
                        else {
                            //re top up all player stats
                            p.SetHealth(100);
                            p.SetEnergy(100);

                            OldCameraPosition = cam->getPosition();
                            cam->setPosition(vector3df(-5.0f, 0.0f, 3.0f));
                            cam->setTarget(p.GetPosition());
                            ef->SetVisible(false);
                            p.GetNode()->setRotation(vector3df(0.0f, -45.0f, 0.0f));
                            groundSceneNode->setVisible(false);
                            ufoBladesSceneNode->setVisible(false);
                            ufoSceneNode->setVisible(false);
                            ef->SetHealthAll(0);
                            ef->ForceDeath(xpMod, cowsXp, cowsKilled);
                            LightningUpgrade(device);
                            state = STATE_POWERUP;
                        }
                    }

                        //boss scene (he will always be around and never trully killed but you must keep fighting him
                    else if ((cowsKilled != 0 && (cowsKilled % 25) == 0) && !bossScene)
                        //else if ((cowsKilled == 0 || cowsKilled == 3) && !bossScene)
                    {
                        ef->SetVisible(false);
                        enemyOrb.GetNode()->setVisible(true);
                        be->SetAnimationID(BIG_BOSS_ANIM_IDLE);
                        be->SetAnimationName("walk");
                        be->GetNode()->setVisible(true);
                        be->RandomPosition(12.0f, true);
                        bigEnemyNewPos = be->GetPosition();
                        be->GetNode()->setPosition(vector3df(be->GetNode()->getPosition().X, -10.0f,
                                                             be->GetNode()->getPosition().Z));
                        be->GetNodeCap()->setPosition(
                                be->GetNode()->getPosition() + vector3df(-0.2f, 3.5f, -0.2f));
                        be->GetNodeDirt()->setPosition(
                                vector3df(be->GetNode()->getPosition().X, 0.1f,
                                          be->GetNode()->getPosition().Z));
                        cam->setTarget(be->GetNode()->getPosition() + vector3df(0.0f, 15.0f, 0.0f));
                        be->LookAt(p.GetPosition(), 180.0f);
                        be->SetHealth(BASE_BOSS_HEALTH + (10 * beKilled));
                        beKilled += 1;
                        bossScene = true;
                        bigEnemyFirstMove = false;
                        bigEnemyWalkOutCap = false;
                        bigEnemyOnMove = false;
                        bigEnemyCapOutOfRange = false;
                        bigEnemyStopShooting = false;
                        bigEnemyCapVelocity = -1.0f;
                    } else if (bossScene) {
                        if (!bossDead) {
                            //if its the intro sequence of the big enemy, make it fade and show him climbinmg out the ground
                            if (!bigEnemyFirstMove &&
                                be->MoveTowards(be->GetCachedSpawnPosition(), frameDeltaTime,
                                                true)) {
                                vector3df p1 = (p.GetPosition() - cam->getPosition()).normalize() *
                                               (ZOOM_INTO_BOSS_SPEED * frameDeltaTime);
                                cam->setPosition(cam->getPosition() + p1);
                                //log the current pos (which will be the final on the next step)
                                bigEnemyOldPos = be->GetPosition();
                            } else {
                                bigEnemyFirstMove = true;

                                //heads towards the centre (the ufo is the middle)
                                //BECAREFUL OF THE Y AXIS!!!!!!!!!
                                if (!bigEnemyWalkOutCap) {
                                    bigEnemyWalkOutCap = !be->MoveTowards(
                                            (ufoSceneNode->getPosition() -
                                             bigEnemyOldPos).normalize(), frameDeltaTime, false);
                                    be->GetNodeCap()->setRotation(((be->GetNode()->getPosition() -
                                                                    be->GetNodeCap()->getPosition()).getHorizontalAngle() +
                                                                   vector3df(0.0f, -90.0f, 0.0f)) *
                                                                  vector3df(0.0f, 1.0f, 0.0f));
                                    bigEnemyMoveCounter += 1.0f * frameDeltaTime;
                                    be->GetNodeCap()->setMD2Animation("open_idle");

                                    if (bigEnemyMoveCounter > bigEnemyMoveMax) {
                                        be->SetAnimationName("idle");
                                        bigEnemyMoveCounter = 0.0f;
                                        bigEnemyWalkOutCap = true;
                                    }
                                } else if (bigEnemyWalkOutCap && !bigEnemyOnMove &&
                                           be->PollNewPosition(frameDeltaTime)) {
                                    bigEnemyNewPos = be->RandomPosition(12.0f, false);
                                    be->LookAt(bigEnemyNewPos, 180.0f);
                                    bigEnemyOnMove = true;
                                } else if (bigEnemyOnMove) {
                                    if (be->GetAnimationID() != BIG_BOSS_ANIM_WALK) {
                                        be->SetAnimationName("walk");
                                        be->SetAnimationID(BIG_BOSS_ANIM_WALK);
                                    }

                                    bigEnemyOnMove = be->MoveTowards(bigEnemyNewPos, frameDeltaTime,
                                                                     false);
                                    if (!bigEnemyOnMove) {
                                        bigEnemyStopShooting = false;
                                        be->LookAt(p.GetPosition(), 180.0f);
                                        if (be->GetAnimationID() != BIG_BOSS_ANIM_ATTACK) {
                                            be->SetAnimationName("attack_main");
                                            be->SetAnimationID(BIG_BOSS_ANIM_ATTACK);
                                        }
                                    }
                                }

                                if (bigEnemyWalkOutCap && !bigEnemyCapOutOfRange) {
                                    be->GetNodeCap()->setMD2Animation("close_idle");

                                    //shoot the capsule off out of the scene
                                    be->GetNodeCap()->setPosition(be->GetNodeCap()->getPosition() +
                                                                  vector3df(0.0f,
                                                                            (bigEnemyCapVelocity *
                                                                             (bigEnemyMoveCounter -
                                                                              bigEnemyCapsuleTakeoff)) *
                                                                            frameDeltaTime, 0.0f));
                                    bigEnemyCapVelocity -= 50.0f * frameDeltaTime;
                                    if (be->GetNodeCap()->getPosition().Y > 100.0f)
                                        bigEnemyCapOutOfRange = true;
                                }

                                cam->setPosition(bossFightCamPos);
                                cam->setTarget(p.GetPosition());
                            }
                        } else if (bossDead){
                            be->DeathAnimation(frameDeltaTime, FMODSystem);
                            vector3df p1 = (defaultCamPos - cam->getPosition()).normalize() * (ZOOM_INTO_BOSS_DEAD_SPEED * frameDeltaTime);
                            cam->setPosition(cam->getPosition() + p1);
                            cam->setTarget(p.GetPosition());
                            if (be->GetAnimationID() == BIG_BOSS_ANIM_DEATH_END3) {
                                be->Reset();
                                bigEnemyStopShooting = true;
                                enemyOrb.GetNode()->setVisible(false);
                                be->GetNodeDirt()->setPosition(vector3df(-9.99f));
                                be->GetNode()->setVisible(false);
                                float dist = (defaultCamPos - cam->getPosition()).getLengthSQ();
                                if (dist < 0.2f)
                                {
                                    firstDeath = true;
                                    bossDead = false;
                                    //add 2 extra cows after the boss battle
                                    ef->SetEnemyCount(ef->GetEnemyCount() + 2);
                                    ef->AddSpeed(0.3f);
                                    cowsXp += ((float)be->GetAttackDamage() / 10) * xpMod;
                                    cowsKilled += 1;
                                    xpMod += 2.4f;
                                    bossScene = false;
                                    enemyOrbSpeed = ENEMY_ORB_DEFAULT_SPEED;
                                }
                            }
                        }
                    }
                }
            }

            if (playerNukeGoneOff) {
                p.GetOrb()->setPosition(p.GetPosition());
                p.GetOrb()->setScale(p.GetOrb()->getScale() + vector3df(10.0f * frameDeltaTime));

                //DISABLE NUKE BUTTON EHRE TO PREVENT INF NUKES
                if (p.GetOrb()->getScale().Y > 40.0f) {
                    playerNukeGoneOff = false;
                    ef->SetHealthAll(0);
                    ef->ForceDeath(xpMod, cowsXp, cowsKilled);
                    p.GetOrb()->setScale(vector3df(1.6f));
                    p.GetOrb()->setVisible(false);
                }
            }
        } else if (state == STATE_INTRO_CUTSCENE) {
            if (ufoSceneNode->getPosition().Y < 0.0f) {
                CutsceneUnload(device);
                state = STATE_GAME;
                GameInit(device);
            } else
                CutsceneUpdate(device, frameDeltaTime);
        } else if (state == STATE_MENU) {
            earthSceneNode->setRotation(
                    earthSceneNode->getRotation() + vector3df(0.0f, -2.0f * frameDeltaTime, 0.0f));
            //Common_Update();
            FMODSystem->playSound(mainMenuMusic, 0, false, &channel);
            channel->setVolume(0.8f);
            //FMODSystem->update();

            //if (er.GetMouseState().LeftButtonDown) {
            if (fakeMouseEvent.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN){
                CutsceneInit(device);
                mainMenuMusic->release();
                state = STATE_INTRO_CUTSCENE;
            }
        } else if (state == STATE_GAME_OVER) {
            //cam->setTarget(ef->GetNearestEnemy(p)->GetPosition());
            //if (!gameOverToReset && er.GetMouseState().LeftButtonDown)
            if (!gameOverToReset && fakeMouseEvent.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                gameOverToReset = true;

            if (gameOverToReset) {
                gameOverTimer += 1.0f * frameDeltaTime;
                if (gameOverTimer > GAME_OVER_FINISH_TIME) {
                    gameOverTimer = 0.0f;
                    GameReset();
                    gameOverToReset = false;
                    state = STATE_GAME;
                }
            }
        } else if (state == STATE_POWERUP) {
            lightningUpgradeTimer += 1.0f * frameDeltaTime;
            p.NotFiringAnimation(frameDeltaTime);
            if (lightningUpgradeTimer > lightningUpgradeWait) {
                lightningUpgradeTimer = 0.0f;
                cam->setPosition(OldCameraPosition);
                cam->setTarget(p.GetPosition());
                ef->SetVisible(true);
                groundSceneNode->setVisible(true);
                ufoBladesSceneNode->setVisible(true);
                ufoSceneNode->setVisible(true);
                state = STATE_GAME;
            }
        }

        driver->beginScene(true, true, SColor(255, 0, 0, 0));
        smgr->drawAll();
        if (state == STATE_MENU) {
            MenuFontDraw(device);
            driver->draw2DImage(fmod_logo, vector2di(20, driver->getScreenSize().Height -
                                                         fmod_logo->getSize().Height - 20));
            driver->draw2DImage(title_logo, vector2di(
                    (driver->getScreenSize().Width / 2) - (title_logo->getSize().Width / 2), 40));

            driver->draw2DImage(ag_logo, vector2di(
                    (driver->getScreenSize().Width) - (ag_logo->getSize().Width) - 20,
                    (driver->getScreenSize().Height) - (ag_logo->getSize().Height) - 20));
        } else if (state == STATE_INTRO_CUTSCENE) {
            if (!cutscene3FadeOut && transition_alpha != 0)
                updateFadeIn(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
            else if (cutscene3FadeOut)
                updateFadeOut(device, 2.0f * frameDeltaTime, device->getTimer()->getTime());
        } else if (state == STATE_GAME_OVER) {
            cowHeadGameOver->setRotation(
                    cowHeadGameOver->getRotation() + vector3df(8.0f * frameDeltaTime, 0.0f, 0.0f));
        }

        if (state == STATE_GAME || state == STATE_GAME_OVER) {
            if (state == STATE_GAME_OVER) {
                dimension2du s = device->getVideoDriver()->getScreenSize();
                stringw str = L"Cows Destroyed: ";
                str += cowsKilled;
                font->draw(str.c_str(), core::rect<s32>(s.Width, s.Height - 110, 0, 0), video::SColor(255, 255, 255, 255), true);
                str = L"Total Cows Record: ";
                str += savedCowsKilled;
                font->draw(str.c_str(), core::rect<s32>(s.Width, s.Height - 80, 0, 0), video::SColor(255, 255, 255, 255), true);
                str = L"Total Cows Destroyed: ";
                str += totalCowsKilled;
                font->draw(str.c_str(), core::rect<s32>(s.Width, s.Height - 50, 0, 0), video::SColor(255, 255, 255, 255), true);

                driver->draw2DImage(go_logo, vector2di((s.Width / 2) - (go_logo->getSize().Width / 2), 20));
            } else {
                 health_inside->setMaxSize(dimension2du(p.HealthGUIValueUpdate(), 10));
                 heat_inside->setMaxSize(dimension2du(p.EnergyGUIValueUpdate(), 10));
                 health_outside->draw();
                 if (p.GetHealth() > 0)
                     health_inside->draw();
                 heat_outside->draw();
                 if (p.GetEnergy() > 0)
                     heat_inside->draw();
                if (cowsXp > 100) {
                    //change to x amount of buttons
                    int whichPerk = rand() % 2;
                    if (whichPerk == 1 && (perkCount % NUKE_CHANCE) != 0)
                        whichPerk = 0;
                    perkCount++;
                    //nuke chance is the highest, if it goes above, reset it
                    if (perkCount == NUKE_CHANCE)
                        perkCount = 0;
                    //enable an op power
                    switch (whichPerk) {
                        case 0:
                            shieldBtnToggle->setVisible(true);
                            shieldTimer = 0.0f;
                            break;
                        case 1:
                            nukeBtnToggle->setVisible(true);
                            break;
                    }

                    cowsXp = 0;
                    cowsXpLvl += 1;
                }

                unlock_inside->setMaxSize(dimension2du(p.UnlockGUIValueUpdate(cowsXp), 10));
                unlock_outside->draw();
                unlock_inside->draw();
                if (shieldBtnToggle->isVisible())
                    shieldBtnToggle->draw();
                if (nukeBtnToggle->isVisible())
                    nukeBtnToggle->draw();
                cow_icon->draw();
                alien_icon->draw();
                dimension2du s = device->getVideoDriver()->getScreenSize();
                stringw str;
                str += (int) cowsXp;
                str += L"/100 | LVL ";
                str += cowsXpLvl;
                font->draw(str.c_str(), core::rect<s32>(320, 70, 0, 0), video::SColor(255, 255, 255, 255), true);

                HighScoreFontDraw(device, cowsKilled);
            }
        }

        driver->endScene();
    }

    /* Cleanup */
    delete ef;
    delete be;
    device->setEventReceiver(0);
    device->closeDevice();
    FMODSystem->close();
    FMODSystem->release();
    device->drop();
    app->activity->vm->DetachCurrentThread();
}
    //mainloop(device, text);

#endif	// defined(_IRR_ANDROID_PLATFORM_)

/*
**/
