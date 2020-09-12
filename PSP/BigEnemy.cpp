#include "BigEnemy.h"

BigEnemy::BigEnemy(engineDevice* d, void* FMODSystem, const float distAway)
{
	animationTimer = 0.0f;
	animationRate = 4.0f;
	newPositionTimer = 0.0f;
	newPositionRate = 4.0f;
	healthDepleteTimer = 0.0f;
	healthDepleteRate = 0.1f;
	health = BASE_BOSS_HEALTH;
	attackStrikeDone = false;
	animationID = BIG_BOSS_ANIM_IDLE;
	attackDamage = 6;
	attackOnce = false;
	isAttacking = false;
	attackDistance = 0.0f;
	attackLength = 0.0f;
	currAttackLength = 0.0f;
	speed = 3.7f;

	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/cow/bigcow.md2");
	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);

		RandomPosition(distAway, true);
		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, true);
			node->setMaterialFlag(EMF_NORMALIZE_NORMALS, false);
			node->setMD2Animation("attack_main");
			node->setMaterialTexture(0, driver->getTexture("media/cow/cow.png"));
			node->getMaterial(0).EmissiveColor = SColor(255, 255, 0, 0);
			//way to find out which enemy/bigenemy we killed
			node->setID(667);
			node->setScale(vector3df(1.2f, 1.2f, 1.2f));
		}
	}

	mesh = smgr->getMesh("media/cow/big_cow_cap.md2");
	if (mesh)
	{
		node_cap = smgr->addAnimatedMeshSceneNode(mesh);
		if (node_cap)
		{
			node_cap->setRotation(vector3df(0.0f, 20.0f, 0.0f));
			node_cap->setPosition(vector3df(999.0f, 999.0f, 999.0f));
			node_cap->setScale(vector3df(2.1f, 2.1f, 2.1f));
			node_cap->setMaterialFlag(EMF_LIGHTING, true);
			node_cap->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			node_cap->setMaterialFlag(EMF_NORMALIZE_NORMALS, false);
			node_cap->setMD2Animation("close_idle");
			node_cap->setMaterialTexture(0, driver->getTexture("media/cow/big_cow_cap.png"));
		}
	}

	mesh = smgr->getMesh("media/base_plane/dirt.obj");
	if (mesh)
	{
		node_dirt = smgr->addAnimatedMeshSceneNode(mesh);
		if (node_dirt)
		{
			node_dirt->setScale(vector3df(1.5f, 1.5f, 1.5f));
			node_dirt->setMaterialFlag(EMF_LIGHTING, true);
			node_dirt->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			node_dirt->setMaterialFlag(EMF_NORMALIZE_NORMALS, false);
			node_dirt->setPosition(vector3df(999.0f, 999.0f, 999.0f));
			node_dirt->setMaterialTexture(0, driver->getTexture("media/base_plane/dirt.png"));
		}
	}

	mesh = smgr->getMesh("media/cow/cap_drill.obj");
	if (mesh)
	{
		node_drill = smgr->addAnimatedMeshSceneNode(mesh, node_cap);
		if (node_drill)
		{
			node_drill->setMaterialFlag(EMF_LIGHTING, true);
			node_drill->setMaterialFlag(EMF_NORMALIZE_NORMALS, false);
			node_drill->setPosition(vector3df(0.0f, 2.5f, 0.0f));
			node_drill->setMaterialTexture(0, driver->getTexture("media/cow/drill.png"));
		}
	}

	/*scene::ITriangleSelector* selector = 0;
	selector = smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop(); // We're done with this selector, so drop it now.*/

	//FMODSystem->createSound("media/music/BigMoo.mp3", FMOD_DEFAULT | FMOD_LOOP_OFF, 0, &bigMooEffect);
	//FMODSystem->createChannelGroup("Moo", &channelGroupBigMoo);
}

void BigEnemy::DeathAnimation(const float dt, void* FMODSystem)
{
	animationTimer += 1.0f * dt;
	//presume we've started on 'attack_start'
	if (animationID == BIG_BOSS_ANIM_DEATH_END && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("death_idle");
		animationID = BIG_BOSS_ANIM_DEATH_END2;
		animationTimer = 0.0f;
	}

	else if ((animationID != BIG_BOSS_ANIM_DEATH_END2 && animationID != BIG_BOSS_ANIM_DEATH_END) && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("death");
		animationID = BIG_BOSS_ANIM_DEATH_END;
		animationTimer = 0.0f;
		if (firstDeath) {
			bool cowDeathFlag = false;
			//channel->isPlaying(&cowDeathFlag);
			if (!cowDeathFlag) {
				//channel->setMode(FMOD_LOOP_OFF);
				//FMODSystem->playSound(bigMooEffect, channelGroupBigMoo, false, &channel);
				//channel->setVolume(0.4f);
			}

			firstDeath = false;
		}
	}

	else if (animationID == BIG_BOSS_ANIM_DEATH_END2 && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		animationID = BIG_BOSS_ANIM_DEATH_END3;
		animationTimer = 0.0f;
	}
}

vector3df BigEnemy::RandomPosition(const float distAway, bool setPos)
{
	bool inv_x = rand() % 2;
	bool inv_z = rand() % 2;
	float x = distAway;
	float z = x;
	if (inv_x)
		x = -x;
	if (inv_z)
		z = -z;

	vector3df target = vector3df(x, BASE_BOSS_HEIGHT, z);
	if (setPos){
		node->setPosition(target);
		cachedSpawnPosition = target;
	}
	
	return target;
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

void BigEnemy::LookAt(vector3df p, const float offset)
{
	vector3df toTarget = p - node->getPosition();
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

bool BigEnemy::MoveTowards(const vector3df p, const float dt, bool includeCap)
{
	float distance = (p - node->getPosition()).getLengthSQ();
	if (distance > 0.5f) {
		vector3df dir = (p - node->getPosition()).normalize();
		if (includeCap)
			node_cap->setPosition((node->getPosition() + vector3df(-0.2f, 3.5f, -0.2f)) + (dir * (speed * dt)));
		node->setPosition(node->getPosition() + (dir * (speed * dt)));
		return true;
	}

	return false;
}

bool BigEnemy::PollNewPosition(const float dt)
{
	newPositionTimer += 1.0f * dt;
	if (newPositionTimer > newPositionRate){
		newPositionTimer = 0.0f;
		return true;
	}

	return false;
}

void BigEnemy::Reset()
{
	
}

BigEnemy::~BigEnemy()
{
	//bigMooEffect->release();
	//bigMooEffect = 0;
}