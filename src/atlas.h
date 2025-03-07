#pragma once
#include "header.h"
#include <unordered_map>
#include <array>



// block atlas for types of blocks
// used to fetch the correct texture for a block id

class Atlas {
private:
    const int TEXTURE_WIDTH = 16;
    const int TEXTURE_HEIGHT = 16; 
    

    
    // texture index for each block type
    // index : front, back, top, bottom, right, left
    // coordinates
    std::unordered_map<int, std::array<int, 6>> blockMap = {
        {1, {1, 1, 0, 2, 1, 1}},    // grass
        {2, {2, 2, 2, 2, 2, 2}},    // dirt
        {3, {3, 3, 3, 3, 3, 3}},    // stone
        {4, {16, 16, 16, 16, 16, 16}},    // sand
        {5, {18, 18, 19, 19, 18, 18}},    // log
        {6, {20, 20, 20, 20, 20, 20}},    // leaves
        {7, {17, 17, 17, 17, 17, 17}},    // glass
    };

    // convert index to uv coordinates
    std::vector<float> getUVCoordinates(int index){
        return { (index % TEXTURE_WIDTH) * UV_WIDTH, (index / TEXTURE_HEIGHT) * UV_HEIGHT};
    }

public:
    float UV_WIDTH = 1.0f / (float)TEXTURE_WIDTH;
    float UV_HEIGHT = 1.0f / (float)TEXTURE_HEIGHT;

    // list of blocks and ids
    enum BlockTypes {
        AIR = 0,
        GRASS = 1,
        DIRT = 2,
        STONE = 3,
        SAND = 4,
        Log = 5,
        Leaves = 6,
        Glass = 7
    };

    Atlas(){}

    std::vector<float> getBlockCoordinates(int blockType, int face){
        if(blockMap.find(blockType) == blockMap.end()){
            std::cerr << "Block type not found" << std::endl;
            return {0.0f, 0.0f};
        }
        return getUVCoordinates(blockMap[blockType][face]);
    }

};