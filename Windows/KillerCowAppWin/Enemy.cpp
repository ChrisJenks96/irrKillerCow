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
	if (animationID != ENEMY_ANIMATION_ATTACK) {
		animationID = ENEMY_ANIMATION_ATTACK;
		node->setMD2Animation("attack");
	}
	
	if (currAttackLength > ANIMATION_FRAME_TO_TIME(9)) {
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

EnemyFactory::EnemyFactory(IrrlichtDevice* d, const int size, const int usable)
{
	//20 - 70
	this->usable = usable;
	for (int i = 0; i < size; i++) {
		float distAway = rand() % (50 + 1) + 20;
		Enemy newEnemy(d, distAway);
		newEnemy.GetNode()->setID(enemyID);
		enemyID++;
		//2.0 units away and for 2 seconds
		newEnemy.SetAttackAttrib(10.0f, 2.0f);
		newEnemy.GetNode()->setVisible(false);
		enemies.push_back(newEnemy);
	}
}

void EnemyFactory::ForceDeath(float& xpMod, float& cowsXp, int& cowsKilled)
{
	for (int i = 0; i < usable; i++) {
		if (!enemies[i].isDeathAnimationTrigger()) {
			cowsXp += ((float)enemies[i].GetAttackDamage() / 10) * xpMod;
			cowsKilled += 1;
		}

		enemies[i].SetDeathAnimationTrigger(true);
	}
}

Enemy* EnemyFactory::FindEnemy(ISceneNode* s)
{
	for (int i = 0; i < usable; i++) {
		if (s->getID() == enemies[i].GetNode()->getID())
			return &enemies[i];
	}

	return NULL;
}

void EnemyFactory::Update(Player& p, bool& shieldActive, int& cowsKilled, const float dt)
{
	for (int i = 0; i < usable; i++) {
		enemies[i].LookAt(p.GetPosition(), -90.0f);
		if (enemies[i].isDeathAnimationTrigger()){
			if (enemies[i].DeathAnimation(dt)){
				enemies[i].GetNode()->setVisible(false);
				enemies[i].Reset();
			}
		}

		else
		{
			ENEMY_STATE es = enemies[i].MoveTowards(p.GetPosition(), dt);
			switch (es)
			{
			case RESET:
				enemies[i].Reset();
				break;
			case ATTACK:
				playerGettingMunched = true;
				enemies[i].Attack(dt);
				break;
			case NONE:
				enemies[i].GetNode()->setVisible(true);
				if (enemies[i].GetAnimationID() != ENEMY_ANIMATION_WALK){
					enemies[i].SetAnimationID(ENEMY_ANIMATION_WALK);
					enemies[i].SetAnimationName("walk");
				}
				break;
			}

			if (enemies[i].isAttackingFlag() && !enemies[i].GetAttackStrikeDone()) {
				enemies[i].SetAttackStrikeDone(true);
				if (!shieldActive)
					p.RemoveHealth(enemies[i].GetAttackDamage());
				else if (shieldActive) {
					
					enemies[i].SetDeathAnimationTrigger(true);
					enemies[i].SetHealth(0);
					cowsKilled += 1;
				}
			}
		}
	}
}

Enemy* EnemyFactory::GetNearestEnemy(Player& p) {
	float distance = 999.0f;
	Enemy* e = nullptr;
	for (int i = 0; i < usable; i++) {
		float thisDist = (p.GetPosition() - enemies[i].GetPosition()).getLengthSQ();
		if (thisDist < distance) {
			distance = thisDist;
			e = &enemies[i];
		}
	}

	return e;
}

void EnemyFactory::ForceReset()
{
	for (int i = 0; i < usable; i++) {
		enemies[i].Reset();
	}
}

void EnemyFactory::SetEnemyCount(int c) {
	if (c >= enemies.size())
		c = enemies.size();
	for (int i = 0; i < c; i++) {
		enemies[i].GetNode()->setVisible(true);
	}

	usable = c;
}

EnemyFactory::~EnemyFactory()
{
	enemies.clear();
}