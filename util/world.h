#pragma once
#include "chunkManager.h"
#include "blockAtlas.h"
#include "camera.h"

class World {
private:
    //chunks
    ChunkManager chunkManager;
    Camera camera = Camera(Vec3d(-5, 0, 0));

public:
    World(blockAtlas * atlas){
        // create a chunk manager
        
        // update mesh
        chunkManager.updateMesh(atlas);
    }


    Camera * getCamera(){
        return &camera;
    }


    std::vector<Triangle> getMesh(){
        return chunkManager.getMesh();
    }


};