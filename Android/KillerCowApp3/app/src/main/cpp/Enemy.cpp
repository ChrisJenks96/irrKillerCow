#include "Enemy.h"

Enemy::Enemy(IrrlichtDevice* d)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/sydney.md2");
	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);
		node->setPosition(vector3df(0.0f, 0.0f, 15.0f));
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
	core::vector3df Rot;
	float X; float Y;
	X = p.X - node->getPosition().X;
	Y = p.Z - node->getPosition().Z;
	Rot.Y = (irr::f32)(((atan2(X, Y) * 180 / PI2) + 180) + offset);
	node->setRotation(Rot);
}

void Enemy::MoveTowards(const vector3df p, const float speed) 
{
	vector3df dir = (p - node->getPosition()).normalize();
	node->setPosition(node->getPosition() + (dir * speed));
}

Enemy::~Enemy()
{

}