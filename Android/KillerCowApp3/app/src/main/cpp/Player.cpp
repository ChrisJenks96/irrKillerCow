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
			node->setMD2Animation(scene::EMAT_STAND);
			node->setMaterialTexture(0, driver->getTexture("media/sydney.bmp"));
		}
	}
}

void Player::LookAt(const vector3df p, const float offset)
{
	const vector3df toTarget = p - node->getPosition();
	node->setRotation(toTarget.getHorizontalAngle() + vector3df(0.0f, offset, 0.0f));
}

Player::~Player()
{

}