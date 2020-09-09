#include "Player.h"

Player::Player(IrrlichtDevice* d)
{
	animationTimer = 0.0f;
	nodeShieldY = 0.0f;
	animationID = PLAYER_ANIMATION_IDLE;
	oldAnimationID = 0;
	energyDepleteTimer = 0.0f;
	energyDepleteRate = 0.1f;
	energyRestoreTimer = 0.0f;
	energyRestoreRate = 0.2f;
	energy = 100;
	health = 100;

	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/player/player.md2");

	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);
		node->setPosition(vector3df(3.0f, -1.0f, -3.0f));
		node->setScale(vector3df(0.7f, 0.7f, 0.7f));
		if (node)
		{
			node->setMaterialFlag(EMF_LIGHTING, true);
			node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			//node->getMaterial(0).SpecularColor = SColor(255, 255, 255, 255);
			node->setMaterialTexture(0, driver->getTexture("media/player/player.png"));
			//set idle animation of player
			animationID = PLAYER_ANIMATION_IDLE;
			SetAnimationName("idle");
		}
	}

	mesh = smgr->getMesh("media/player/player_shield.obj");

	if (mesh)
	{
		nodeShield = smgr->addAnimatedMeshSceneNode(mesh);
		nodeShield->setParent(node);
		nodeShield->setScale(vector3df(4.0f));
		if (nodeShield)
		{
			nodeShield->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
			nodeShield->setMaterialFlag(EMF_LIGHTING, false);
			nodeShield->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			nodeShield->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			nodeShield->setMaterialTexture(0, driver->getTexture("media/shields/shield_blue.png"));
			nodeShield->setVisible(false);
		}
	}

	mesh = smgr->getMesh("media/player/player_shield.obj");

	if (mesh)
	{
		orb = smgr->addAnimatedMeshSceneNode(mesh);
		orb->setScale(vector3df(4.0f));
		if (nodeShield)
		{
			orb->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
			orb->setMaterialFlag(EMF_LIGHTING, false);
			orb->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			orb->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			orb->setMaterialTexture(0, driver->getTexture("media/shields/shield_red.png"));
			orb->setVisible(false);
		}
	}

	orb->setVisible(false);
	orb->setPosition(node->getPosition());

	//weapon firing lighting effect
	weaponFiringLight = smgr->addLightSceneNode(node, vector3df(0.0f, 10.0f, -70.0f), lightning_types[currentLightningType].col);
	weaponFiringLight->getLightData().Type = ELT_SPOT;
	weaponFiringLight->getLightData().InnerCone = 10.0f;
	weaponFiringLight->getLightData().OuterCone = 20.0f;
	weaponFiringLight->getLightData().Falloff = 0.0f;
	WeaponFiringLightToggle(false);
}

void Player::AddEnergy(const float dt)
{
	energyRestoreTimer += 1.0f * dt;
	if (energyRestoreTimer > energyRestoreRate) {
		energy += 1;
		energyRestoreTimer = 0.0f;
	}
}

void Player::RemoveEnergy(const float dt)
{
	energyDepleteTimer += 1.0f * dt;
	if (energyDepleteTimer > energyDepleteRate) {
		energy -= 1;
		energyDepleteTimer = 0.0f;
	}
}

void Player::FiringAnimation(const float dt)
{
	animationTimer += 1.0f * dt;
	//presume we've started on 'attack_start'
	if (animationID == PLAYER_ANIMATION_IDLE && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("attack_s");
		//animation 'attack_start'
		oldAnimationID = animationID;
		animationID = PLAYER_ANIMATION_ATTACK_START;
		animationTimer = 0.0f;
	}

	else if (animationID == PLAYER_ANIMATION_ATTACK_START && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("attack_main");
		//animation 'attack_main'
		oldAnimationID = animationID;
		animationID = PLAYER_ANIMATION_ATTACK_MAIN;
		animationTimer = 0.0f;
	}
}

void Player::DeathAnimation(const float dt)
{
	animationTimer += 1.0f * dt;
	//presume we've started on 'attack_start'
	if (animationID != PLAYER_ANIMATION_DEATH_END && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("death");
		//animation 'death start'
		oldAnimationID = animationID;
		animationID = PLAYER_ANIMATION_DEATH_END;
		animationTimer = 0.0f;
	}

	else if (animationID == PLAYER_ANIMATION_DEATH_END && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("death_idle");
		//animation 'death idle'
		oldAnimationID = animationID;
		animationID = PLAYER_ANIMATION_DEATH_END;
		animationTimer = 0.0f;
	}
}

void Player::NotFiringAnimation(const float dt)
{
	animationTimer += 1.0f * dt;
	if (animationID == PLAYER_ANIMATION_CRAWL_WALK && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("idle");
		//animation 'idle'
		oldAnimationID = animationID;
		animationID = PLAYER_ANIMATION_IDLE;
		animationTimer = 0.0f;
	}
	//presume we're on the 'attack_main', 0.2f fade off animation
	else if ((animationID == PLAYER_ANIMATION_ATTACK_MAIN || animationID == PLAYER_ANIMATION_ATTACK_START) && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("attack_end");
		//animation 'idle'
		oldAnimationID = animationID;
		animationID = PLAYER_ANIMATION_ATTACK_END;
		animationTimer = 0.0f;
	}

	else if (animationID == PLAYER_ANIMATION_ATTACK_END && animationTimer > ANIMATION_FRAME_TO_TIME(4)) {
		SetAnimationName("idle");
		//animation 'idle'
		oldAnimationID = animationID;
		animationID = PLAYER_ANIMATION_IDLE;
		animationTimer = 0.0f;
	}
}

void Player::ShieldUVScroll(const float dt)
{
	nodeShield->getMaterial(0).getTextureMatrix(0).setTextureTranslate(0, nodeShieldY);
	nodeShieldY -= 1.2f * dt;
	if (nodeShieldY >= 1.0f)
		nodeShieldY = 0.0f;
}

ISceneNode* Player::Fire(IrrlichtDevice* device, const float length)
{
	WeaponFiringLightToggle(true);

	core::line3d<f32> ray;
	ray.start = node->getPosition() + vector3df(0.0f, 1.0f, 0.0f);
	ray.end = ray.start + SceneNodeDir(node) * length;
	// Tracks the current intersection point with the level or a mesh
	core::vector3df intersection;
	// Used to show with triangle has been hit
	core::triangle3df hitTriangle;
	scene::ISceneNode* selectedSceneNode = device->getSceneManager()->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(
		ray, intersection, hitTriangle, 0, 0);
	if (selectedSceneNode)
		return selectedSceneNode;
	return NULL;
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

bool Player::MoveTowards(const vector3df p, const float dt)
{
	float distance = (p - node->getPosition()).getLengthSQ();
	if ((distance > 1.0f)) {
		vector3df dir = (p - node->getPosition()).normalize();
		node->setPosition(node->getPosition() + (dir * (1.0f * dt)));
		return true;
	}

	return false;
}

Player::~Player()
{

}