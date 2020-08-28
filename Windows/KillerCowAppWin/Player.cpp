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
			node->setMD2Animation("attack");
			node->setMaterialTexture(0, driver->getTexture("media/sydney.bmp"));
		}
	}

	//weapon firing lighting effect
	weaponFiringLight = smgr->addLightSceneNode(node, vector3df(0.0f, 10.0f, -100.0f), SColorf(1.0f, 1.0f, 0.0f, 1.0f));
	weaponFiringLight->getLightData().Type = ELT_SPOT;
	weaponFiringLight->getLightData().InnerCone = 10.0f;
	weaponFiringLight->getLightData().OuterCone = 20.0f;
	weaponFiringLight->getLightData().Falloff = 0.0f;
	WeaponFiringLightToggle(false);
}

void Player::Fire(IrrlichtDevice* device)
{
	WeaponFiringLightToggle(true);

	core::line3d<f32> ray;
	ray.start = node->getPosition() + vector3df(0.0f, 1.0f, 0.0f);
	ray.end = ray.start + SceneNodeDir(node) * 1000.0f;
	// Tracks the current intersection point with the level or a mesh
	core::vector3df intersection;
	// Used to show with triangle has been hit
	core::triangle3df hitTriangle;
	scene::ISceneNode* selectedSceneNode = device->getSceneManager()->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(
		ray, intersection, hitTriangle, 0, 0);
	if (selectedSceneNode)
		selectedSceneNode->setVisible(false);
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