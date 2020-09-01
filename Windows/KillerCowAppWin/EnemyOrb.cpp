#include "EnemyOrb.h"

EnemyOrb::EnemyOrb(IrrlichtDevice* d)
{
	IVideoDriver* driver = d->getVideoDriver();
	ISceneManager* smgr = d->getSceneManager();
	IMesh* mesh = smgr->getMesh("media/power_orb/orb.obj");
	if (mesh)
	{
		node = smgr->addMeshSceneNode(mesh);
		if (node)
		{
			node->setPosition(vector3df(999.0f));
			node->setScale(vector3df(1.6f));
			node->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
			node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			node->setMaterialTexture(0, driver->getTexture("media/power_orb/centre.png"));
		}
	}

	//global lightning settings
	QUAD_SEGMENT_INCREMENT = -3.0f;
	float angle = 360.0f / (float)NUM_LIGHTNING_SPLIT_AXIS;
	for (int i = 0; i < NUM_LIGHTNING_SPLIT_AXIS; i++) {
		bolts[i] = new LightningSceneNode(node, smgr, 666);
		bolts[i]->setMaterialTexture(0, driver->getTexture("media/lightning/red_bolt.png"));
		bolts[i]->setRotation(vector3df(angle * i, 0.0f, 0.0f));
		bolts[i]->setScale(vector3df(0.03f));
		bolts[i]->setVisible(true);
	}

	for (int i = 4; i < NUM_LIGHTNING_SPLIT_AXIS + 4; i++) {
		bolts[i] = new LightningSceneNode(node, smgr, 666);
		bolts[i]->setMaterialTexture(0, driver->getTexture("media/lightning/red_bolt.png"));
		bolts[i]->setRotation(vector3df(0.0f, 0.0f, angle * i));
		bolts[i]->setScale(vector3df(0.03f));
		bolts[i]->setVisible(true);
	}
}

void EnemyOrb::Update(const float dt)
{
	node->setRotation(node->getRotation() + vector3df(80.0f * dt));
	QUAD_SEGMENT_INCREMENT = -3.0f;
	for (int i = 0; i < NUM_LIGHTNING_SPLIT_AXIS; i++) {
		bolts[i]->ArkUpdate(dt);
	}

	for (int i = 4; i < NUM_LIGHTNING_SPLIT_AXIS + 4; i++) {
		bolts[i]->ArkUpdate(dt);
	}
}