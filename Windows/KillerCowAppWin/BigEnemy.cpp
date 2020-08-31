#include "BigEnemy.h"

BigEnemy::BigEnemy(IrrlichtDevice* d, const float distAway)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/cow/bigcow.md2");
	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);

		RandomPosition(distAway);
		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, true);
			node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			node->setMD2Animation("attack_main");
			node->setMaterialTexture(0, driver->getTexture("media/cow/cow.png"));
			//way to find out which enemy/bigenemy we killed
			node->setID(667);
			node->setScale(vector3df(2.0f));
		}
	}

	scene::ITriangleSelector* selector = 0;
	selector = smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop(); // We're done with this selector, so drop it now.
}

void BigEnemy::RandomPosition(const float distAway)
{
	bool inv_x = rand() % 2;
	bool inv_z = rand() % 2;
	float x = distAway;
	float z = x;
	if (inv_x)
		x = -x;
	if (inv_z)
		z = -z;

	vector3df target = vector3df(x, 0.0f, z);
	node->setPosition(target);
	cachedSpawnPosition = target;
}

void BigEnemy::RemoveHealth(int damage, const float dt)
{
	//5 as the cows are hard to kill
	healthDepleteTimer += 5.0f * dt;
	if (healthDepleteTimer > healthDepleteRate) {
		health -= damage;
		healthDepleteTimer = 0.0f;
	}
}

void BigEnemy::LookAt(const vector3df p, const float offset)
{
	const vector3df toTarget = p - node->getPosition();
	node->setRotation((toTarget.getHorizontalAngle() + vector3df(0.0f, offset, 0.0f)) * vector3df(0.0f, 1.0f, 0.0f));
}

void BigEnemy::Attack(const float dt)
{
	currAttackLength += 1.0f * dt;
	//do some attacky stuff here...
	if (currAttackLength > attackLength) {
		isAttacking = false;
		currAttackLength = 0.0f;
		node->setMD2Animation("idle");
	}
}

bool BigEnemy::MoveTowards(const vector3df p, const float dt)
{
	float distance = (p - node->getPosition()).getLengthSQ();
	if (distance > 0.5f) {
		vector3df dir = (p - node->getPosition()).normalize();
		node->setPosition(node->getPosition() + (dir * (speed * dt)));
		return true;
	}

	return false;
}

void BigEnemy::Reset()
{
	health = BASE_COW_HEALTH;
	SetAttackStrikeDone(false);
	float distAway = rand() % (50 + 1) + 20;
	node->setMD2Animation("idle");
	RandomPosition(distAway);
	isAttacking = false;
	attackOnce = false;
	node->setVisible(true);
}

BigEnemy::~BigEnemy()
{

}