#pragma once
#include "header.h"
#include "atlas.h"

/*
Chunk
stores blocks in chunk [16][16][16]

stores block rendering information
uses atlas to get block texture coordinates for mesh
creates/cache mesh data for solid and transparent blocks


*/

// note 8 verticies per cube
float vertices[6][4][3] = {
    { {0,0,0}, {0,1,0}, {1,1,0}, {1,0,0} }, // Back with reverse winding
    { {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1} }, // Front
    { {0,1,1}, {1,1,1}, {1,1,0}, {0,1,0} },  // Top face with reversed winding
    { {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1} }, // Bottom
    { {1,0,0}, {1,1,0}, {1,1,1}, {1,0,1} },  // Right face with reversed winding
    { {0,0,0}, {0,0,1}, {0,1,1}, {0,1,0} }  // Left
};

// need seperate uv coordinates for each face due to orientation 
float uv[6][4][2] = {
    // Front face
    {{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}},

    // Back face
    {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}},

    // Top face
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},{0.0f, 1.0f}},

    // Bottom face
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}},

    // Right face
    {{1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}},

    // Left face
    {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}}
};

// const float indicies[2][3] = {
//     {0, 1, 2},
//     {0, 2, 3}
// };


// brightness for block face - front, back, top, bottom, right, left
float brightness[6] = {0.86f, 0.86f, 1.0f, 1.0f, 0.8f, 0.8f};

using std::vector;




class Chunk {
    const int LENGTH = 16;
    const int WIDTH = 16;
    const int HEIGHT = 16;

    vector<vector<vector<int>>> blocks;
    glm::vec3 position;

    std::vector<float> solidVerticies;
    std::vector<unsigned int> solidIndicies;

    std::vector<float> transparentVerticies;
    std::vector<unsigned int> transparentIndicies;

    Atlas * atlas;

public:
    Chunk(glm::vec3 position, Atlas * atlas) : position(position) {
        blocks = vector<vector<vector<int>>>(LENGTH, vector<vector<int>>(WIDTH, vector<int>(HEIGHT, 0)));
        this->atlas = atlas;
    }
    

    Chunk(glm::vec3 position, Atlas * atlas, int type) : position(position) {
        blocks = vector<vector<vector<int>>>(LENGTH, vector<vector<int>>(WIDTH, vector<int>(HEIGHT, type)));
        this->atlas = atlas;
    }

    vector<vector<vector<int>>> getBlocks(){
        return blocks;
    }

    
    int getBlock(int x, int y, int z){
        return blocks[x][y][z];
    }

    void setBlock(int x, int y, int z, int type){
        blocks[x][y][z] = type;
    }


    std::vector<float> getSolidVerticies(){
        return solidVerticies;
    }

    std::vector<unsigned int> getSolidIndicies(){
        return solidIndicies;
    }

    std::vector<float> getTransparentVerticies(){
        return transparentVerticies;
    }

    std::vector<unsigned int> getTransparentIndicies(){
        return transparentIndicies;
    }


    // create mesh data for chunk
    // need to consider other chunks
    // access manager to get other chunks
    void createMesh(Chunk * frontChunk, Chunk * backChunk, Chunk * topChunk, Chunk * bottomChunk, Chunk * rightChunk, Chunk * leftChunk){
        solidVerticies.clear();
        solidIndicies.clear();
        transparentVerticies.clear();
        transparentIndicies.clear();

        for(int x = 0; x < LENGTH; x++){
            for(int y = 0; y < WIDTH; y++){
                for(int z = 0; z < HEIGHT; z++){
                    if(blocks[x][y][z] == 0) continue;

                    // only add face if visible
                    // TODO: check if block next to is transparent, then would need to add
                    // Front face
                    // for z = 0, check chunk next to, otherwise check block in front
                    if(z == 0){
                        if(backChunk == nullptr || backChunk->getBlock(x, y, LENGTH - 1) == 0) addBlockFace(x, y, z, 0, blocks[x][y][z]);
                    } else {
                        if(blocks[x][y][z - 1] == 0) addBlockFace(x, y, z, 0, blocks[x][y][z]);
                    }

                   
                    // Back face
                    if(z == LENGTH - 1){
                        if(frontChunk == nullptr || frontChunk->getBlock(x, y, 0) == 0) addBlockFace(x, y, z, 1, blocks[x][y][z]);
                    } else {
                        if(blocks[x][y][z + 1] == 0) addBlockFace(x, y, z, 1, blocks[x][y][z]);
                    }
                    
                    // Top face
                    if(y == HEIGHT - 1){
                        if(topChunk == nullptr || topChunk->getBlock(x, 0, z) == 0) addBlockFace(x, y, z, 2, blocks[x][y][z]);
                    } else {
                        if(blocks[x][y + 1][z] == 0) addBlockFace(x, y, z, 2, blocks[x][y][z]);
                    }
             
                    
                    // Bottom face
                    if(y == 0){
                        if(bottomChunk == nullptr || bottomChunk->getBlock(x, HEIGHT - 1, z) == 0) addBlockFace(x, y, z, 3, blocks[x][y][z]);
                    } else {
                        if(blocks[x][y - 1][z] == 0) addBlockFace(x, y, z, 3, blocks[x][y][z]);
                    }



                    // Right face
                    if(x == WIDTH - 1){
                        if(rightChunk == nullptr || rightChunk->getBlock(0, y, z) == 0) addBlockFace(x, y, z, 4, blocks[x][y][z]);
                    } else {
                        if(blocks[x + 1][y][z] == 0) addBlockFace(x, y, z, 4, blocks[x][y][z]);
                    }



                    // Left face
                    if(x == 0){
                        if(leftChunk == nullptr || leftChunk->getBlock(WIDTH - 1, y, z) == 0) addBlockFace(x, y, z, 5, blocks[x][y][z]);
                    } else {
                        if(blocks[x - 1][y][z] == 0) addBlockFace(x, y, z, 5, blocks[x][y][z]);
                    }

                }
            }
        }
    }

    void addBlockFace(int x, int y, int z, int face, int type){
        // accound for chunk position + block position
        float cord[3] = {(float) x + position.x * WIDTH, (float) y + position.y * HEIGHT, (float) z + position.z * LENGTH};

        vector<float> offset = atlas->getBlockCoordinates(type, face);

        vector<float> * verticies;
        vector<unsigned int> * indicies;
        if(atlas->isTransparent(type)){
            verticies = &transparentVerticies;
            indicies = &transparentIndicies;
        } else {
            verticies = &solidVerticies;
            indicies = &solidIndicies;
        } 

        int indexOffset = verticies->size() / 6;



        // add face verticies
        for(int i = 0; i < 4; i++){
            // 3 values for x, y, z
            verticies->push_back(cord[0] + vertices[face][i][0]);
            verticies->push_back(cord[1] + vertices[face][i][1]);
            verticies->push_back(cord[2] + vertices[face][i][2]);
            
            // 2 values for u, v
            verticies->push_back(uv[face][i][0] * atlas->UV_WIDTH + offset[0]);
            verticies->push_back(uv[face][i][1] * atlas->UV_HEIGHT + offset[1]);

            // 1 value for brightness of texture
            verticies->push_back(brightness[face]);
        }

        // add face indicies
        // Add indices for two triangles
        indicies->push_back(indexOffset);
        indicies->push_back(indexOffset + 1);
        indicies->push_back(indexOffset + 2);
        indicies->push_back(indexOffset);
        indicies->push_back(indexOffset + 2);
        indicies->push_back(indexOffset + 3);
        
    }


};