#include "Enemy.h"

Enemy::Enemy(IrrlichtDevice* d, const float distAway)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/sydney.md2");
	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);

		RandomPosition(distAway);
		node->setScale(vector3df(0.05f, 0.05f, 0.05f));
		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, true);
			node->setMD2Animation(scene::EMAT_RUN);
			node->setMaterialTexture(0, driver->getTexture("media/sydney.bmp"));
		}
	}
}

void Enemy::RandomPosition(const float distAway)
{
	float x = rand() % 721 + (-360);
	float z = rand() % 721 + (-360);
	vector3df target = vector3df(x, 0.0f, z);
	vector3df p = (target - node->getPosition()).normalize() * distAway;
	node->setPosition(p);
}

void Enemy::LookAt(const vector3df p, const float offset)
{
	const vector3df toTarget = p - node->getPosition();
	node->setRotation((toTarget.getHorizontalAngle() + vector3df(0.0f, offset, 0.0f)) * vector3df(0.0f, 1.0f, 0.0f));
}

void Enemy::Attack(const float dt) 
{
	currAttackLength += 1.0f * dt;
	//do some attacky stuff here...
	if (currAttackLength > attackLength) {
		isAttacking = false;
		currAttackLength = 0.0f;
		node->setMD2Animation(scene::EMAT_RUN);
	}
}

ENEMY_STATE Enemy::MoveTowards(const vector3df p, const float speed) 
{
	float distance = (p - node->getPosition()).getLengthSQ();
	if (isAttacking && attackOnce)
		return ENEMY_STATE::ATTACK;

	else if ((distance < attackDistance)) {
		if (!attackOnce) {
			isAttacking = true;
			attackOnce = true;
			node->setMD2Animation(scene::EMAT_ATTACK);
			return ENEMY_STATE::ATTACK;
		}
	}

	if (!isAttacking) {
		vector3df dir = (p - node->getPosition()).normalize();
		node->setPosition(node->getPosition() + (dir * speed));
	}

	if (distance < 0.5f) {
		node->setMD2Animation(scene::EMAT_DEATH_FALLBACK);
		return ENEMY_STATE::RESET;
	}

	return ENEMY_STATE::NONE;
}

void Enemy::Reset()
{
	float distAway = rand() % (50 + 1) + 20;
	node->setMD2Animation(scene::EMAT_RUN);
	RandomPosition(distAway);
	attackOnce = false;
}

Enemy::~Enemy()
{

}

EnemyFactory::EnemyFactory(IrrlichtDevice* d, const int size)
{
	//20 - 70
	for (int i = 0; i < size; i++) {
		float distAway = rand() % (50 + 1) + 20;
		Enemy newEnemy(d, distAway);
		//2.0 units away and for 2 seconds
		newEnemy.SetAttackAttrib(10.0f, 2.0f);
		enemies.push_back(newEnemy);
	}
}

void EnemyFactory::Update(const vector3df t, const float dt)
{
	for (auto& x : enemies) {
		x.LookAt(t, -90.0f);
		ENEMY_STATE es = x.MoveTowards(t, 5.0f * dt);
		switch (es)
		{
			case RESET:
				x.Reset();
				break;
			case ATTACK:
				x.Attack(dt);
				break;
			default:
				break;
		}
	}
}

EnemyFactory::~EnemyFactory()
{
	enemies.clear();
}