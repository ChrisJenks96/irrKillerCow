#include "Enemy.h"

Enemy::Enemy(IrrlichtDevice* d)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/sydney.md2");
	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);

		vector3df target = vector3df(-20.0f, 0.0f, 34.0f);
		vector3df p = (target - node->getPosition()).normalize() * 15.0f;
		node->setPosition(p);
		node->setScale(vector3df(0.05f, 0.05f, 0.05f));
		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMD2Animation(scene::EMAT_RUN);
			node->setMaterialTexture(0, driver->getTexture("media/sydney.bmp"));
		}
	}
}

void Enemy::LookAt(const vector3df p, const float offset)
{
	const vector3df toTarget = p - node->getPosition();
	node->setRotation(toTarget.getHorizontalAngle() + vector3df(0.0f, offset, 0.0f));
}

void Enemy::MoveTowards(const vector3df p, const float speed) 
{
	vector3df dir = (p - node->getPosition()).normalize();
	node->setPosition(node->getPosition() + (dir * speed));
}

Enemy::~Enemy()
{

}