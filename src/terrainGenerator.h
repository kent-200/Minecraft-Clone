#pragma once
#include "header.h"
#include "chunk.h"
#include "atlas.h"

#define STB_PERLIN_IMPLEMENTATION
#include "../lib/stb_perlin.h"



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
    int seed;
    float frequency = 0.05f;
    float amplitude = 5.0f;

public:
    TerrainGenerator(Atlas * atlas, int seed = 1337){
        this->atlas = atlas;
        this->seed = seed;

    }

    // generate stone with gass ontop
    Chunk generateChunk(glm::vec3 position){
        // top layer
        if(position.y == 3){
            // dirt with grass ontop
            Chunk chunk = Chunk(position, atlas, 0);

            // grass
            for(int x = 0; x < 16; x++){
                for(int z = 0; z < 16; z++){
                    float nx = (x + position.x * 16) * frequency;
                    float nz = (z + position.z * 16) * frequency;
                    float height = stb_perlin_noise3_seed(nx, position.y, nz, 0, 0, 0, seed);

                    // Normalize height to a suitable range
                    int blockHeight = static_cast<int>((height + 1.0f) * amplitude); // Adjust scaling as needed

                    // fill dirt
                    for(int y = 0; y < blockHeight; y++){
                        chunk.setBlock(x, y, z, 2);
                    }

                    // grass
                    chunk.setBlock(x, blockHeight, z, 1);
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