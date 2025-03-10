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


    
    
public:
    
    // fix this
    int chunkIndex(glm::vec3 position){
        return (position.x * 16 * 16) + (position.y * 16) + position.z;
    }

    Chunk * getChunk(glm::vec3 position){
        if(chunkMap.count(chunkIndex(position)) == 0){
            return nullptr;
        }
        return chunkMap[chunkIndex(position)];
    }


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
                }
            }
        }

        this->placeBlock(glm::vec3(20, 20, 20), 0);
        this->placeBlock(glm::vec3(20, 66, 20), 5);
        // change how this works
        for(int x = -renderDistance; x < renderDistance; x++){
            for(int y = 0; y < worldChunkHeight; y++){
                for(int z = -renderDistance; z < renderDistance; z++){
                    // createMesh(front, back, top, bottom, right, left)
                    chunkMap[chunkIndex(glm::vec3(x, y, z))]->createMesh(getChunk(glm::vec3(x, y, z + 1)), getChunk(glm::vec3(x, y, z - 1)), getChunk(glm::vec3(x, y + 1, z)), getChunk(glm::vec3(x, y - 1, z)), getChunk(glm::vec3(x + 1, y, z)), getChunk(glm::vec3(x - 1, y, z)));
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

    int getBlock(glm::vec3 position){
        glm::vec3 chunkPosition = glm::vec3(floor(position.x / 16), floor(position.y / 16), floor(position.z / 16));
        glm::vec3 blockPosition = glm::vec3(position.x - chunkPosition.x * 16, position.y - chunkPosition.y * 16, position.z - chunkPosition.z * 16);
        Chunk * chunk = getChunk(chunkPosition);
        if(chunk == nullptr) return 0;
        return chunk->getBlock(blockPosition.x, blockPosition.y, blockPosition.z);
    }


    // place block
    void placeBlock(glm::vec3 position, int type) {
        glm::vec3 chunkPosition = glm::vec3(floor(position.x / 16), floor(position.y / 16), floor(position.z / 16));
        glm::vec3 blockPosition = glm::vec3(position.x - chunkPosition.x * 16, position.y - chunkPosition.y * 16, position.z - chunkPosition.z * 16);
        Chunk * chunk = getChunk(chunkPosition);
        if(chunk == nullptr) return;
        chunk->setBlock(blockPosition.x, blockPosition.y, blockPosition.z, type);

        // upate mesh

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