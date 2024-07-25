#pragma once
#include "vec3d.h"

class blockAtlas {
private:
    struct blockData{
        std::array<int, 6> textures;
        std::string name;
        int id;
    };
    std::vector<Vec3d> allTextures;
    std::vector<blockData> blocks;

public:
    blockAtlas(){}

    //returns id - front, back, left, right, top, bottom
    int addBlock(std::string name, Vec3d cfront, Vec3d cback, Vec3d cleft, Vec3d cright, Vec3d ctop, Vec3d cbottom){
        // front, back, left, right, top, bottom
        allTextures.push_back(cfront);
        allTextures.push_back(cback);
        allTextures.push_back(cleft);
        allTextures.push_back(cright);
        allTextures.push_back(ctop);
        allTextures.push_back(cbottom);

        std::array<int, 6> blockTextures = {
            (int) (allTextures.size()) -6, 
            (int) (allTextures.size()) -5,
            (int) (allTextures.size()) -4, 
            (int) (allTextures.size()) -3, 
            (int) (allTextures.size()) -2, 
            (int) (allTextures.size()) -1
        };
        blocks.push_back(blockData{blockTextures, name, (int) blocks.size()});
        return blocks.size()-1;
    }

    //returns id - front, back, left, right, top, bottom
    int addBlock(std::string name, Vec3d cside, Vec3d ctop, Vec3d cbottom){
        allTextures.push_back(cside);
        allTextures.push_back(ctop);
        allTextures.push_back(cbottom);

        std::array<int, 6> blockTextures = {
            (int) (allTextures.size()) -3, 
            (int) (allTextures.size()) -3, 
            (int) (allTextures.size()) -3, 
            (int) (allTextures.size()) -3, 
            (int) (allTextures.size()) -2, 
            (int) (allTextures.size()) -1
        };

        blocks.push_back({blockTextures, name, (int) blocks.size()});
        return blocks.size()-1;
    }

    // returns id - front, back, left, right, top, bottom
    int addBlock(std::string name, Vec3d c){
        allTextures.push_back(c);

        std::array<int, 6> blockTextures = {
            (int) (allTextures.size())-1,
            (int) (allTextures.size())-1,
            (int) (allTextures.size())-1,
            (int) (allTextures.size())-1,
            (int) (allTextures.size())-1,
            (int) (allTextures.size())-1
        };

        blocks.push_back({blockTextures, name, (int) blocks.size()});
        return blocks.size()-1;
    }


    std::string getBlockName(int id){
        return blocks[id].name;
    }

    std::array<Vec3d, 6> getBlockTextures(int id){
        std::array<Vec3d, 6> textures;
        for(int i = 0; i < 6; i++){
            textures[i] = allTextures[blocks[id].textures[i]];
        }
        return textures;
    }
  
    Vec3d getBlockTexture(int id, int face){
        return allTextures[blocks[id].textures[face]];
    }
};