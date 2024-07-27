#pragma once
#include "block.h"
#include "triangle.h"
#include "blockAtlas.h"

class Chunk {
private:
    // 16x16x128 chunk by default
    const int CHUNK_LENGTH = 16;
    const int CHUNK_HEIGHT = 128;
    const int CHUNK_DEPTH = 16;

    // int refers to block type from an atlas
    int blocks[16][128][16] = {0};
    Vec3d position;

    // mesh
    std::vector<Triangle> tris;

    //update to save mesh in chunk class and only change when needed, on a block update

    //remove triangles that won't be seen 

    //subchunks

public:
    Chunk(Vec3d position){
        this->position = position;
    }

    Chunk(){
        this->position = Vec3d(0, 0, 0);
    }

    Vec3d getPosition(){
        return position;
    }

    void setPosition(Vec3d position){
        this->position = position;
    }

    // Get block type at position
    int getBlock(int x, int y, int z){
        return blocks[x][y][z];
    }

    // Set block type at position
    void setBlock(int x, int y, int z, int type){
        blocks[x][y][z] = type;
    }

    // set row of blocks
    void setRow(int x, int y, int z, int length, int type){
        for(int i = 0; i < length; i++){
            blocks[x + i][y][z] = type;
        }
    }

    //set face (x and z) of blocks
    void setHorizontalFace(int y, int type){
        for(int x = 0; x < CHUNK_LENGTH; x++){
            for(int z = 0; z < CHUNK_DEPTH; z++){
                blocks[x][y][z] = type;
            }
        }
    }

    void setHorizontalFace(int y, int height, int type){
        for(int x = 0; x < CHUNK_LENGTH; x++){
            for(int z = 0; z < CHUNK_DEPTH; z++){
                for(int i = 0; i < height; i++){
                    blocks[x][y + i][z] = type;
                }
            }
        }
    }


    void updateMesh(blockAtlas *atlas){
        tris.clear();
        Vec3d positionOffset = Vec3d(
            position.x * CHUNK_LENGTH, 
            position.y * CHUNK_HEIGHT, 
            position.z * CHUNK_DEPTH
        );
        for(int x = 0; x < CHUNK_LENGTH; x++){
            for(int y = 0; y < CHUNK_HEIGHT; y++){
                for(int z = 0; z < CHUNK_DEPTH; z++){
                    if(blocks[x][y][z] != 0){
                        Block::allRelativeTriangles(
                            tris, 
                            Vec3d(x, y, z) + positionOffset, 
                            atlas->getBlockTextures(blocks[x][y][z])
                        );
                    }
                }
            }
        }
        return;
    }

    std::vector<Triangle> getMesh(){
        return tris;
    }


};   

