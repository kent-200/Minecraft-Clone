#pragma once
#include "chunk.h"
#include "blockAtlas.h"
#include "camera.h"

class World {
private:
    //chunks
    std::vector<Chunk> chunks;
    Camera camera = Camera(Vec3d(-5, 0, 0));

public:
    World(blockAtlas * atlas){
        // create chunks
        chunks.push_back(Chunk(Vec3d(0, 0, 0)));
        chunks.push_back(Chunk(Vec3d(1, 0, 0)));
        chunks.push_back(Chunk(Vec3d(1, 0, 1)));
        chunks.push_back(Chunk(Vec3d(0, 0, 1)));

        // set blocks
        chunks[0].setHorizontalFace(0, 5);
        chunks[1].setHorizontalFace(5, 4);
        chunks[2].setHorizontalFace(10, 3);
        chunks[3].setHorizontalFace(15, 1);

        // update mesh
        for(int i = 0; i < (int) chunks.size(); i++){
            chunks[i].updateMesh(atlas);
        }        
    }

    int numChunks(){
        return (int)chunks.size();
    }

    Chunk getChunk(int index){
        return chunks[index];
    }

    std::vector<Chunk> getChunks(){
        return chunks;
    }

    Camera * getCamera(){
        return &camera;
    }





};