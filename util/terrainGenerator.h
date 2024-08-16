#pragma once
#include "vec3d.h"
#include "chunk.h"
#include <stdexcept>

class TerrainGenerator{
private:


public:
    TerrainGenerator(int seed){}

    TerrainGenerator() : TerrainGenerator(0){}

    Chunk generateTerrain(Vec3d position);

    ~TerrainGenerator(){}
};


Chunk TerrainGenerator::generateTerrain(Vec3d chunkPosition){
    // check position is in range
    if(chunkPosition.y < 0){
        throw std::invalid_argument("Position y must be greater than 0");
    }

    // create chunk
    Chunk chunk(chunkPosition);

    // generate terrain

    // 0-58 stone 
    // 69-63 dirt
    // 64 grass


    // based on y
    if(chunkPosition.y < 3){
        //fill with stone
        chunk.fillChunk(4);

    } else if(chunkPosition.y == 3){ 
        //fill 10 stone
        chunk.setHorizontalFace(0, 10, 4);
        //fill 4 dirt
        chunk.setHorizontalFace(10, 4, 3);

        // fill 1 grass
        chunk.setHorizontalFace(14, 1, 1);
    } 
    // do nothing if above

    return chunk;
}