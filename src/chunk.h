#include "header.h"
#include "atlas.h"

/*
Chunk
- block types [16][16][16]

- mesh 


*/

float verticies[6][18] = {
    {
        // Front face
        1.0f, 0.0f, 1.0f,  // Triangle 1
        0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,  // Triangle 2
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f
    },
    {
        // Back face
        0.0f, 0.0f, 0.0f,  // Triangle 3
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  // Triangle 4
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    },
    {
        // Top face
        1.0f, 1.0f, 0.0f,  // Triangle 5
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,  // Triangle 6
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f
    },
    {
        // Bottom face 
        0.0f, 0.0f, 0.0f,  // Triangle 7
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,  // Triangle 8
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f
    },
    {
        // Right face
        1.0f, 0.0f, 0.0f,  // Triangle 9
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  // Triangle 10
        1.0f, 1.0f, 1.0f
    },
    {
        // Left face
        0.0f, 0.0f, 0.0f,  // Triangle 11
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,  // Triangle 12
        0.0f, 1.0f, 1.0f
    }
};

float uv[6][12] = {
    {
        // Front face
        1.0f, 1.0f,  // Triangle 1
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,  // Triangle 2
        1.0f, 0.0f,
        0.0f, 0.0f,
    },
    {
        // Back face
        1.0f, 1.0f,  // Triangle 3
        1.0f, 0.0f,
        0.0f, 1.0f, 
        1.0f, 0.0f,  // Triangle 4
        0.0f, 0.0f,
        0.0f, 1.0f,
    },
    {
        // Top face
        0.0f, 1.0f,  // Triangle 5
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,  // Triangle 6
        1.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        // Bottom face
        1.0f, 1.0f,   // Triangle 7
        1.0f, 0.0f, 
        0.0f, 0.0f, 
        1.0f, 1.0f,   // Triangle 8
        0.0f, 0.0f, 
        0.0f, 1.0f
    },
    {
        // Right face
        1.0f, 1.0f,   // Triangle 9
        0.0f, 1.0f, 
        1.0f, 0.0f, 
        1.0f, 0.0f,   // Triangle 10
        0.0f, 1.0f, 
        0.0f, 0.0f
    },
    {
        // Left face
        0.0f, 1.0f,   // Triangle 11
        0.0f, 0.0f, 
        1.0f, 1.0f, 
        1.0f, 1.0f,   // Triangle 12
        0.0f, 0.0f, 
        1.0f, 0.0f
    }
};

// // x, y - FOR GRASS BLOCK   
// float offset[6][2] = {
//     {1.0f, 0.0f},   // Front face
//     {1.0f, 0.0f},   // Back face
//     {0.0f, 0.0f},   // Top face
//     {2.0f, 0.0f},   // Bottom face
//     {1.0f, 0.0f},   // Right face
//     {1.0f, 0.0f}    // Left face
// };

// brightness - front, back, top, bottom, right, left
float brightness[6] = {0.86f, 0.86f, 1.0f, 1.0f, 0.8f, 0.8f};

using std::vector;


class Chunk {
    const int LENGTH = 16;
    const int WIDTH = 16;
    const int HEIGHT = 16;

    vector<vector<vector<int>>> blocks;

    glm::vec3 position;
    std::vector<float> solidMesh;
    std::vector<float> transparentMesh;
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


    std::vector<float> getSolidMesh(){
        return solidMesh;
    }

    std::vector<float> getTransparentMesh(){
        return transparentMesh;
    }


    void createMesh(){
        solidMesh.clear();
        transparentMesh.clear();

        for(int x = 0; x < LENGTH; x++){
            for(int y = 0; y < WIDTH; y++){
                for(int z = 0; z < HEIGHT; z++){
                    if(blocks[x][y][z] == 0) continue;

                    // Front face
                    if(z == HEIGHT - 1 || blocks[x][y][z + 1] == 0) addBlockFace(x, y, z, 0, blocks[x][y][z]);

                    // Back face
                    if(z == 0 || blocks[x][y][z - 1] == 0) addBlockFace(x, y, z, 1, blocks[x][y][z]);
                    
                    // Top face
                    if(y == WIDTH - 1 || blocks[x][y + 1][z] == 0) addBlockFace(x, y, z, 2, blocks[x][y][z]);
                    
                    // Bottom face
                    if(y == 0 || blocks[x][y - 1][z] == 0) addBlockFace(x, y, z, 3, blocks[x][y][z]);

                    // Right face
                    if(x == LENGTH - 1 || blocks[x + 1][y][z] == 0) addBlockFace(x, y, z, 4, blocks[x][y][z]);

                    // Left face
                    if(x == 0 || blocks[x - 1][y][z] == 0) addBlockFace(x, y, z, 5, blocks[x][y][z]);

                }
            }
        }

        std::cout << "Solid Mesh size: " << solidMesh.size() << std::endl;
        std::cout << "Transparent Mesh size: " << transparentMesh.size() << std::endl;
        

    }

    void addBlockFace(int x, int y, int z, int face, int type){
        // accound for chunk position + block position
        float cord[3] = {(float) x + position.x * WIDTH, (float) y + position.y * HEIGHT, (float) z + position.z * LENGTH};

        vector<float> offset = atlas->getBlockCoordinates(type, face);

        vector<float> * mesh;
        if(atlas->isTransparent(type)){
            mesh = &transparentMesh;
        } else {
            mesh = &solidMesh;
        }


        // add a cube to the mesh, all 6 faces
        int i = face;


        // First triangle
        for(int j = 0; j < 3; j++) mesh->push_back(verticies[i][j] + cord[j]);   // Vertex 1
        mesh->push_back(uv[i][0] * atlas->UV_WIDTH + offset[0]);               
        mesh->push_back(uv[i][1] * atlas->UV_HEIGHT + offset[1]); 
        mesh->push_back(brightness[i]);  // brightness


        for(int j = 3; j < 6; j++) mesh->push_back(verticies[i][j] + cord[j - 3]);   // Vertex 2
        mesh->push_back(uv[i][2] * atlas->UV_WIDTH + offset[0]);               
        mesh->push_back(uv[i][3] * atlas->UV_HEIGHT + offset[1]);   
        mesh->push_back(brightness[i]);  // brightness
        
        for(int j = 6; j < 9; j++) mesh->push_back(verticies[i][j] + cord[j - 6]);   // Vertex 3
        mesh->push_back(uv[i][4] * atlas->UV_WIDTH + offset[0]);               
        mesh->push_back(uv[i][5] * atlas->UV_HEIGHT + offset[1]);   
        mesh->push_back(brightness[i]);  // brightness

        // Second triangle
        for(int j = 9; j < 12; j++) mesh->push_back(verticies[i][j] + cord[j - 9]);  // Vertex 4
        mesh->push_back(uv[i][6] * atlas->UV_WIDTH + offset[0]);               
        mesh->push_back(uv[i][7] * atlas->UV_HEIGHT + offset[1]);   
        mesh->push_back(brightness[i]);  // brightness

        for(int j = 12; j < 15; j++) mesh->push_back(verticies[i][j] + cord[j - 12]); // Vertex 5
        mesh->push_back(uv[i][8] * atlas->UV_WIDTH + offset[0]);               
        mesh->push_back(uv[i][9] * atlas->UV_HEIGHT + offset[1]);   
        mesh->push_back(brightness[i]);  // brightness

        for(int j = 15; j < 18; j++) mesh->push_back(verticies[i][j] + cord[j - 15]); // Vertex 6
        mesh->push_back(uv[i][10] * atlas->UV_WIDTH + offset[0]);               
        mesh->push_back(uv[i][11] * atlas->UV_HEIGHT + offset[1]); 
        mesh->push_back(brightness[i]);  // brightness
        
    }




};