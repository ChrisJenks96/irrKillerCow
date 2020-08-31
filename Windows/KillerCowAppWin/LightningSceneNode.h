#pragma once

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//how many quads do we want
#define QUAD_SEGMENTS 10
//how big is each quad
#define QUAD_SIZE 0.35f
//how far per quad do we go
extern int QUAD_SEGMENT_INCREMENT;
//#define QUAD_SEGMENT_INCREMENT -10.0f

constexpr int TOTAL_VERTS = (QUAD_SEGMENTS * 4);
constexpr int TOTAL_EXTRA_IND = (QUAD_SEGMENTS / 2) * 6;
constexpr int TOTAL_IND = (TOTAL_VERTS + (TOTAL_VERTS / 2)) + TOTAL_EXTRA_IND;

class LightningSceneNode : public ISceneNode
{
public:
    LightningSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
        : scene::ISceneNode(parent, mgr, id)
    {
        Material.Wireframe = false;
        Material.Lighting = false;
        Material.BackfaceCulling = false;

        int quadCounter = 0;
        //start from 4 because there is a nasty bug somewhere?!?!? (possible overflow from indices calc)
        for (int i = 4; i < TOTAL_VERTS; i+=4)
        {
            int randomOffset = rand() % 8 + (-4);
            float negQuad = (QUAD_SEGMENT_INCREMENT * quadCounter) + -QUAD_SIZE;
            float posQuad = (QUAD_SEGMENT_INCREMENT * quadCounter) + QUAD_SIZE;

            Vertices[i] = video::S3DVertex(-QUAD_SIZE + randomOffset, posQuad, 0, 1, 1, 0,
                video::SColor(255, 255, 255, 255), 0, 1);
            Vertices[i+1] = video::S3DVertex(QUAD_SIZE + randomOffset, posQuad, 0, 1, 0, 0,
                video::SColor(255, 255, 255, 255), 1, 1);
            Vertices[i+2] = video::S3DVertex(QUAD_SIZE + randomOffset, negQuad, 0, 0, 1, 1,
                video::SColor(255, 255, 255, 255), 1, 0);
            Vertices[i+3] = video::S3DVertex(-QUAD_SIZE + randomOffset, negQuad, 0, 0, 0, 1,
                video::SColor(255, 255, 255, 255), 0, 0);

            if (i >= 8)
            {
                indices[indCounter] = ((i-4) * 1) + 3;
                indices[indCounter + 1] = ((i-4) * 1) + 2;
                indices[indCounter + 2] = (i * 1) + 1;

                indices[indCounter + 3] = (i * 1) + 1;
                indices[indCounter + 4] = (i * 1);
                indices[indCounter + 5] = ((i-4) * 1) + 3;
                indCounter += 6;
            }

            indices[indCounter] = (i * 1);
            indices[indCounter + 1] = (i * 1) + 1;
            indices[indCounter + 2] = (i * 1) + 2;

            indices[indCounter + 3] = (i * 1) + 2;
            indices[indCounter + 4] = (i * 1) + 3;
            indices[indCounter + 5] = (i * 1);
            indCounter += 6;

            quadCounter++;
        }

        Box.reset(Vertices[0].Pos);
        for (s32 i = 0; i < TOTAL_VERTS; i++)
            Box.addInternalPoint(Vertices[i].Pos);
    }

    void ArkUpdate(const float dt)
    {
        ArkUpdateTimer += 1.0f * dt;
        if (ArkUpdateTimer > ArkUpdateRate)
        {
            //how many quads in do we want the ark to start changing (root of lightning shouldnt change) 4 = default
            //go in increments of 4
            int arkStart = 8;
            int arkVertInc = 0;
            float arkUpperLimitY = -3.0f;
            float arkLowerLimitY = -12.0f;
            for (int i = arkStart; i < TOTAL_VERTS; i += 4) {
                float randomOffset = (rand() % 4 + (-2)) * 1.33f;
                if (randomOffset != 0)
                {
                    float newVertY = Vertices[i].Pos.Y - arkVertInc;
                    if (newVertY < arkLowerLimitY && randomOffset <= 0) {
                        randomOffset = -randomOffset;
                    }

                    else if (newVertY > arkUpperLimitY && randomOffset >= 0) {
                        randomOffset = -randomOffset;
                    }

                    Vertices[i].Pos += vector3df(randomOffset);
                    Vertices[i + 1].Pos += vector3df(randomOffset);
                    Vertices[i + 2].Pos += vector3df(randomOffset);
                    Vertices[i + 3].Pos += vector3df(randomOffset);
                }

                arkVertInc += QUAD_SEGMENT_INCREMENT;
            }

            ArkUpdateTimer = 0.0f;
        }
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
        driver->drawVertexPrimitiveList(&Vertices[0], TOTAL_VERTS, &indices[0], indCounter / 3, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
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
        float ArkUpdateTimer{ 0.0f };
        float ArkUpdateRate{ 0.05f };
        bool ArkValueNegative{ false };
		core::aabbox3d<f32> Box;
        int indCounter = 0;
        u16 indices[TOTAL_IND];
		video::S3DVertex Vertices[TOTAL_VERTS];
		video::SMaterial Material;
};