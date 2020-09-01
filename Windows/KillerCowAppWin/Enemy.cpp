#include "Enemy.h"

Enemy::Enemy(IrrlichtDevice* d, const float distAway)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/cow/cow.md2");
	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);

		RandomPosition(distAway);
		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, true);
			node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			node->setMD2Animation("idle");
			node->setMaterialTexture(0, driver->getTexture("media/cow/cow.png"));
		}
	}

	scene::ITriangleSelector* selector = 0;
	selector = smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop(); // We're done with this selector, so drop it now.
}

void Enemy::RandomPosition(const float distAway)
{
	float x = rand() % 721 + (-360);
	float z = rand() % 721 + (-360);
	vector3df target = vector3df(x, 0.0f, z);
	vector3df p = (target - node->getPosition()).normalize() * distAway;
	node->setPosition(p);
}

void Enemy::RemoveHealth(int damage, const float dt)
{
	//5 as the cows are hard to kill
	healthDepleteTimer += 5.0f * dt;
	if (healthDepleteTimer > healthDepleteRate) {
		health -= damage;
		healthDepleteTimer = 0.0f;
	}
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
		node->setMD2Animation("walk");
	}
}

ENEMY_STATE Enemy::MoveTowards(const vector3df p, const float dt)
{
	float distance = (p - node->getPosition()).getLengthSQ();
	if (isAttacking && attackOnce)
		return ENEMY_STATE::ATTACK;

	else if ((distance < attackDistance)) {
		if (!attackOnce) {
			isAttacking = true;
			attackOnce = true;
			node->setMD2Animation("idle");
			return ENEMY_STATE::ATTACK;
		}
	}

	if (!isAttacking) {
		vector3df dir = (p - node->getPosition()).normalize();
		node->setPosition(node->getPosition() + (dir * (speed * dt)));
	}

	if (distance < 0.5f) {
		node->setMD2Animation("walk");
		return ENEMY_STATE::RESET;
	}

	//we've died :(
	if (!node->isVisible())
		Reset();

	return ENEMY_STATE::NONE;
}

bool Enemy::DeathAnimation(const float dt)
{
	deathAnimationTimer += 1.0f * dt;
	if ((animationID != ENEMY_ANIMATION_DEATH && animationID != ENEMY_ANIMATION_DEATH_IDLE) && deathAnimationTimer > ANIMATION_FRAME_TO_TIME(4)){
		SetAnimationName("death");
		animationID = ENEMY_ANIMATION_DEATH;
		deathAnimationTimer = 0.0f;
	}

	else if (animationID == ENEMY_ANIMATION_DEATH && deathAnimationTimer > ANIMATION_FRAME_TO_TIME(4))
	{
		SetAnimationName("death_idle");
		animationID = ENEMY_ANIMATION_DEATH_IDLE;
		deathAnimationTimer = 0.0f;
	}

	//lets see the death idle animation for 1s then we'll tell the player the enemy is dead
	else if (animationID == ENEMY_ANIMATION_DEATH_IDLE && deathAnimationTimer > 2.0f)
	{
		animationID = ENEMY_ANIMATION_WALK;
		deathAnimationTimer = 0.0f;
		return true;
	}

	return false;
}

void Enemy::Reset()
{
	health = BASE_COW_HEALTH;
	SetDeathAnimationTrigger(false);
	SetAttackStrikeDone(false);
	float distAway = rand() % (50 + 1) + 20;
	node->setMD2Animation("walk");
	RandomPosition(distAway);
	isAttacking = false;
	attackOnce = false;
	node->setVisible(true);
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
		newEnemy.GetNode()->setID(enemyID);
		enemyID++;
		//2.0 units away and for 2 seconds
		newEnemy.SetAttackAttrib(10.0f, 2.0f);
		enemies.push_back(newEnemy);
	}
}

void EnemyFactory::ForceDeath(float& xpMod, float& cowsXp, int& cowsKilled)
{
	for (auto& x : enemies) {
		if (!x.isDeathAnimationTrigger()) {
			cowsXp += ((float)x.GetAttackDamage() / 10) * xpMod;
			cowsKilled += 1;
		}

		x.SetDeathAnimationTrigger(true);
	}
}

Enemy* EnemyFactory::FindEnemy(ISceneNode* s)
{
	for (auto& x : enemies) {
		if (s->getID() == x.GetNode()->getID())
			return &x;
	}

	return NULL;
}

void EnemyFactory::Update(Player& p, const float dt)
{
	for (auto& x : enemies) {
		x.LookAt(p.GetPosition(), -90.0f);
		if (x.isDeathAnimationTrigger()){
			if (x.DeathAnimation(dt)){
				x.GetNode()->setVisible(false);
				x.Reset();
			}
		}

		else
		{
			ENEMY_STATE es = x.MoveTowards(p.GetPosition(), dt);
			switch (es)
			{
			case RESET:
				x.Reset();
				break;
			case ATTACK:
				playerGettingMunched = true;
				x.Attack(dt);
				break;
			case NONE:
				if (x.GetAnimationID() != ENEMY_ANIMATION_WALK){
					x.SetAnimationID(ENEMY_ANIMATION_WALK);
					x.SetAnimationName("walk");
				}
				break;
			}

			if (x.isAttackingFlag() && !x.GetAttackStrikeDone()) {
				x.SetAttackStrikeDone(true);
				p.RemoveHealth(x.GetAttackDamage());
			}
		}
	}
}

Enemy* EnemyFactory::GetNearestEnemy(Player& p) {
	float distance = 999.0f;
	Enemy* e = nullptr;
	for (auto& x : enemies) {
		float thisDist = (p.GetPosition() - x.GetPosition()).getLengthSQ();
		if (thisDist < distance) {
			distance = thisDist;
			e = &x;
		}
	};

	return e;
}

void EnemyFactory::ForceReset()
{
	for (auto& x : enemies) {
		x.Reset();
	};
}

EnemyFactory::~EnemyFactory()
{
	enemies.clear();
}