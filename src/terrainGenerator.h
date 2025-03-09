#pragma once
#include "header.h"
#include "chunk.h"
#include "atlas.h"


/*
Terrain Generator

basic generator for now

TODO:
implement perlin noise
implement trees
implement rivers
implement biomes
implement caves
implement ore generation

*/


// enum BlockTypes {
//     AIR = 0,
//     GRASS = 1,
//     DIRT = 2,
//     STONE = 3,
//     SAND = 4,
//     Log = 5,
//     Leaves = 6,
//     Glass = 7
// };


class TerrainGenerator {
private:
    Atlas * atlas;


public:
    TerrainGenerator(Atlas * atlas){
        this->atlas = atlas;
    }

    // generate stone with gass ontop
    Chunk generateChunk(glm::vec3 position){
        // top layer
        if(position.y == 3){
            // dirt with grass ontop
            Chunk chunk = Chunk(position, atlas, 2);

            // grass
            for(int x = 0; x < 16; x++){
                for(int z = 0; z < 16; z++){
                    chunk.setBlock(x, 15, z, 1);
                }
            }

            return chunk;

        } else if(position.y < 3) {
            // stone chunk
            Chunk chunk = Chunk(position, atlas, 3);
            return chunk;
        } else {
            // air chunk
            Chunk chunk = Chunk(position, atlas, 0);
            return chunk;
        }
    }
};