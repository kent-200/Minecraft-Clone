#include "chunk.h"
#include "terrainGenerator.h"
#include <unordered_map>

// Custom hash function
namespace std {
    template <>
    struct hash<Vec3d> {
        std::size_t operator()(const Vec3d& v) const noexcept {
            std::size_t hx = std::hash<int>()(v.x);
            std::size_t hy = std::hash<int>()(v.y);
            std::size_t hz = std::hash<int>()(v.z);
            return hx ^ (hy << 1) ^ (hz << 2); // Combine hashes
        }
    };
}


class ChunkManager {
private:
    // 16x16x16 chunk by default
    const int CHUNK_LENGTH = 16;
    const int CHUNK_HEIGHT = 16;
    const int CHUNK_DEPTH = 16;

    //chunks
    std::vector<Chunk> chunks;
    std::unordered_map<Vec3d, int> chunkMap;

    int renderRadius;

    // terrain generator
    TerrainGenerator generator;


    // get chunk index from position
    int getChunkIndex(Vec3d position);

    void generateSpawnChunks();
    

public:
    ChunkManager(int spawnRadius, int renderRadius){
        // generate intial chunks
        this->renderRadius = renderRadius;

        // create a terrain generator
        generator = TerrainGenerator(0);
    

        // generate spawn chunks
        generateSpawnChunks();
    }

    ChunkManager() : ChunkManager(4, 4){}
        

    int numChunks(){
        return (int)chunks.size();
    }

    int getBlock(int x, int y, int z);
    void setBlock(int x, int y, int z, int type);     

    void fillChunk(int index, int type);
    void fillBlocks(int x, int y, int z, int length, int height, int depth, int type);

    // mesh - need to change to different approach 
    std::vector<Triangle> getMesh();
    void updateMesh(blockAtlas *atlas);
        
};


int ChunkManager::getChunkIndex(Vec3d position){
    // convert to chunk position
    Vec3d chunkPosition = Vec3d(
        floor(position.x / CHUNK_LENGTH),
        floor(position.y / CHUNK_HEIGHT),
        floor(position.z / CHUNK_DEPTH)
    );

    // check if chunk exists in map 
    if(chunkMap.find(chunkPosition) == chunkMap.end()) return -1;
    return chunkMap[chunkPosition];
}


void ChunkManager::generateSpawnChunks(){
    // generate spawn chunks
    for(int x = -renderRadius; x < renderRadius; x++){
        for(int z = -renderRadius; z < renderRadius; z++){
            for(int y = 0; y < 4; y++){
                Vec3d chunkPosition = Vec3d(x, y, z);
                Chunk chunk = generator.generateTerrain(chunkPosition);
                chunks.push_back(chunk);
                chunkMap[chunkPosition] = (int)chunks.size() - 1;
            }
        }
    }
}


int ChunkManager::getBlock(int x, int y, int z){
    int chunkIndex = getChunkIndex(Vec3d(x, y, z));
    if(chunkIndex == -1) return -1;
    return chunks[chunkIndex].getBlock(x % CHUNK_LENGTH, y % CHUNK_HEIGHT, z % CHUNK_DEPTH);
}

void ChunkManager::setBlock(int x, int y, int z, int type){
    int chunkIndex = getChunkIndex(Vec3d(x, y, z));
    if(chunkIndex == -1) return;
    chunks[chunkIndex].setBlock(x % CHUNK_LENGTH, y % CHUNK_HEIGHT, z % CHUNK_DEPTH, type);
}

void ChunkManager::fillChunk(int index, int type){
    if(index < 0 || index >= (int)chunks.size()) return;
    chunks[index].fillChunk(type);
}

void ChunkManager::fillBlocks(int x, int y, int z, int length, int height, int depth, int type){
    for(int i = 0; i < length; i++){
        for(int j = 0; j < height; j++){
            for(int k = 0; k < depth; k++){
                setBlock(x + i, y + j, z + k, type);
            }
        }
    }
}


std::vector<Triangle> ChunkManager::getMesh(){
    std::vector<Triangle> tris;
    for(int i = 0; i < (int)chunks.size(); i++){
        std::vector<Triangle> chunkTris = chunks[i].getMesh();
        for(int j = 0; j < (int)chunkTris.size(); j++){
            tris.push_back(chunkTris[j]);
        }
    }
    return tris;
}

void ChunkManager::updateMesh(blockAtlas *atlas){
    for(int i = 0; i < (int)chunks.size(); i++){
        chunks[i].updateMesh(atlas);
    }
}


