#pragma once

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//how big is each quad
#define QUAD_SIZE 1
//how many quads do we want
#define QUAD_SEGMENTS 4
//how far per quad do we go
#define QUAD_SEGMENT_INCREMENT -10
#define TOTAL_VERTS (QUAD_SEGMENTS * 4)
#define TOTAL_IND ((TOTAL_VERTS + (TOTAL_VERTS/2)) * 2) - 12

class LightningSceneNode : public ISceneNode
{
public:
    LightningSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
        : scene::ISceneNode(parent, mgr, id)
    {
        Material.Wireframe = false;
        Material.Lighting = false;
        Material.BackfaceCulling = false;

        int quadCounter = 1;
        int indCounter = 0;
        srand(time(NULL));
        for (int i = 0; i < TOTAL_VERTS; i+=4)
        {
            int randomOffset = rand() % 19 + (-9);
            int negQuad = (QUAD_SEGMENT_INCREMENT * quadCounter) + -QUAD_SIZE;
            int posQuad = (QUAD_SEGMENT_INCREMENT * quadCounter) + QUAD_SIZE;
            Vertices[i] = video::S3DVertex(-QUAD_SIZE + randomOffset, posQuad, 0, 1, 1, 0,
                video::SColor(255, 255, 255, 255), 0, 1);
            Vertices[i+1] = video::S3DVertex(QUAD_SIZE + randomOffset, posQuad, 0, 1, 0, 0,
                video::SColor(255, 255, 255, 255), 1, 1);
            Vertices[i+2] = video::S3DVertex(QUAD_SIZE + randomOffset, negQuad, 0, 0, 1, 1,
                video::SColor(255, 255, 255, 255), 1, 0);
            Vertices[i+3] = video::S3DVertex(-QUAD_SIZE + randomOffset, negQuad, 0, 0, 0, 1,
                video::SColor(255, 255, 255, 255), 0, 0);

            indices[indCounter] = (i * 1);
            indices[indCounter+1] = (i * 1) + 1;
            indices[indCounter+2] = (i * 1) + 2;

            indices[indCounter+3] = (i * 1) + 2;
            indices[indCounter+4] = (i * 1) + 3;
            indices[indCounter+5] = (i * 1);

            indCounter += 6;
            if (i != 0 && i != (TOTAL_VERTS - 4))
            {
                indices[indCounter] = (i * 1) + 3;
                indices[indCounter + 1] = (i * 1) + 2;
                indices[indCounter + 2] = ((i+4) * 1) + 1;

                indices[indCounter + 3] = ((i+4) * 1) + 1;
                indices[indCounter + 4] = ((i+4) * 1);
                indices[indCounter + 5] = (i * 1) + 3;
                indCounter += 6;
            }

            quadCounter++;
        }

        Box.reset(Vertices[0].Pos);
        for (s32 i = 1; i < QUAD_SEGMENTS * 4; ++i)
            Box.addInternalPoint(Vertices[i].Pos);
    }

    virtual void OnRegisterSceneNode()
    {
        if (IsVisible)
            SceneManager->registerNodeForRendering(this);

        ISceneNode::OnRegisterSceneNode();
    }

    virtual void render()
    {
        video::IVideoDriver* driver = SceneManager->getVideoDriver();

        driver->setMaterial(Material);
        driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
        driver->drawVertexPrimitiveList(&Vertices[0], TOTAL_VERTS, &indices[0], TOTAL_VERTS/2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
    }

    virtual const core::aabbox3d<f32>& getBoundingBox() const
    {
        return Box;
    }

    virtual u32 getMaterialCount() const
    {
        return 1;
    }

    virtual video::SMaterial& getMaterial(u32 i)
    {
        return Material;
    }

	private:
		core::aabbox3d<f32> Box;
        u16 indices[TOTAL_IND];
		video::S3DVertex Vertices[TOTAL_VERTS];
		video::SMaterial Material;
};