#pragma once
#include "header.h"
#include "chunk.h"
#include "render.h"
#include "terrainGenerator.h"


/*
Chunk Manager
stores chunks in single map, with
handles choosing chunks for rendering
handle chunk generation

when chunk goes into render distance and not generated, generate it
have a queue of chunks to generate, limit waiting for frame

block placement/removal


*/



class ChunkManager {
private:
    const int renderDistance = 5;
    const int worldChunkHeight = 8;                     //16*8 = 256 world height
    std::unordered_map<int, Chunk*> chunkMap;     // store chunks
    std::queue<glm::vec3> renderQueue;                  // queue of chunks to render - for efficiency
    // when generating chunks create their mesh and store in chunk


    // fix this
    int chunkIndex(glm::vec3 position){
        return (position.x * 16 * 16) + (position.y * 16) + position.z;
    }

public:
    ChunkManager(TerrainGenerator & terrainGenerator){
        // generate chunks around 0, 0, 0
        for(int x = -renderDistance; x < renderDistance; x++){
            for(int y = 0; y < worldChunkHeight; y++){
                for(int z = -renderDistance; z < renderDistance; z++){

                    // change to queue
                    glm::vec3 position = glm::vec3(x, y, z);
                    int index = chunkIndex(position);
                    //addChunkToQueue(position);
                    chunkMap[index] = new Chunk(terrainGenerator.generateChunk(position));
                    chunkMap[index]->createMesh();  //change how this words
                }
            }
        }
    }

    void renderWorld(Render & render, glm::vec3 position, glm::mat4 viewMatrix){
        // check if chunks need to be generated 


        // render 3d scene based on position


        for(int x = -renderDistance; x < renderDistance; x++){
            for(int y = 0; y < worldChunkHeight; y++){
                for(int z = -renderDistance; z < renderDistance; z++){
                    glm::vec3 position = glm::vec3(x, y, z);
                    int index = chunkIndex(position);
                    if(chunkMap.count(index) != 0){
                        render.renderData(viewMatrix, chunkMap[index]->getSolidVerticies(), chunkMap[index]->getSolidIndicies(), false);
                        render.renderData(viewMatrix, chunkMap[index]->getTransparentVerticies(), chunkMap[index]->getTransparentIndicies(), true);
                    }
                }
            }
        }
    }

    // add chunk to render queue
    void addChunkToQueue(glm::vec3 position){
        renderQueue.push(position);
    }

    std::queue<glm::vec3> getRenderQueue(){
        return renderQueue;
    }


    // place block
    void placeBlock(glm::vec3 position, int type) {

    }

    // place block 0
    void deleteBlock(glm::vec3 position) {
        this->placeBlock(position, 0); 
    }


    void destroy(){
        for(auto &chunk : chunkMap){
            delete chunk.second;
        }
    } 


};