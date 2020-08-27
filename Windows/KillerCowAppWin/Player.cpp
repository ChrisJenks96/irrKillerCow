#include "Player.h"

Player::Player(IrrlichtDevice* d)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/sydney.md2");

	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);
		node->setPosition(vector3df(3.0f, 0.0f, -3.0f));
		node->setScale(vector3df(0.05f, 0.05f, 0.05f));
		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, true);
			node->setMD2Animation(scene::EMAT_ATTACK);
			node->setMaterialTexture(0, driver->getTexture("media/sydney.bmp"));
		}
	}

	//weapon firing lighting effect
	weaponFiringLight = smgr->addLightSceneNode(node, vector3df(0.0f, 10.0f, -100.0f), SColorf(1.0f, 1.0f, 0.0f, 1.0f));
	weaponFiringLight->getLightData().Type = ELT_SPOT;
	weaponFiringLight->getLightData().InnerCone = 10.0f;
	weaponFiringLight->getLightData().OuterCone = 20.0f;
	weaponFiringLight->getLightData().Falloff = 0.0f;
}

void Player::Fire()
{
	WeaponFiringLightToggle(true);
}

void Player::Idle()
{
	WeaponFiringLightToggle(false);
}

void Player::LookAt(const vector3df p, const float offset)
{
	const vector3df toTarget = (p - node->getPosition()).normalize();
	node->setRotation(toTarget.getHorizontalAngle() + vector3df(0.0f, offset, 0.0f));
}

Player::~Player()
{

}