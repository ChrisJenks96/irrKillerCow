#include "Player.h"

Player::Player(IrrlichtDevice* d)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	mesh = smgr->getMesh("media/player/player.md2");

	if (mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);
		node->setPosition(vector3df(3.0f, 0.0f, -3.0f));
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

ISceneNode* Player::Fire(IrrlichtDevice* device)
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