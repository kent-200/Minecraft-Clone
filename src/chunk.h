#include "header.h"

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

float offset[6][2] = {
    {1.0f, 0.0f},   // Front face
    {1.0f, 0.0f},   // Back face
    {0.0f, 0.0f},   // Top face
    {2.0f, 0.0f},   // Bottom face
    {1.0f, 0.0f},   // Right face
    {1.0f, 0.0f}    // Left face
};

using std::vector;


class Chunk {
    const int LENGTH = 16;
    const int WIDTH = 16;
    const int HEIGHT = 16;
    const GLfloat UV_WIDTH = 1.0f / 16.0f;
    const GLfloat UV_HEIGHT = 1.0f / 16.0f;

    vector<vector<vector<int>>> blocks;

    glm::vec3 position;
    std::vector<float> mesh;

public:
    Chunk() : position(glm::vec3(0, 0, 0)) {
        blocks = vector<vector<vector<int>>>(LENGTH, vector<vector<int>>(WIDTH, vector<int>(HEIGHT, 0)));
    }

    Chunk(glm::vec3 position) : position(position) {
        blocks = vector<vector<vector<int>>>(LENGTH, vector<vector<int>>(WIDTH, vector<int>(HEIGHT, 0)));
    }
    

    Chunk(glm::vec3 position, int type) : position(position) {
        blocks = vector<vector<vector<int>>>(LENGTH, vector<vector<int>>(WIDTH, vector<int>(HEIGHT, type)));
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


    std::vector<float> getMesh(){
        return mesh;
    }


    void createMesh(){
        mesh.clear();

        for(int x = 0; x < LENGTH; x++){
            for(int y = 0; y < WIDTH; y++){
                for(int z = 0; z < HEIGHT; z++){
                    float cord[3] = {(float) x, (float) y, (float) z};


                    // add a cube to the mesh, all 6 faces
                    for(int i = 0; i < 6; i++){
                        // First triangle
                        for(int j = 0; j < 3; j++) mesh.push_back(verticies[i][j] + cord[j]);   // Vertex 1
                        mesh.push_back(uv[i][0] * UV_WIDTH + UV_WIDTH * offset[i][0]);               
                        mesh.push_back(uv[i][1] * UV_HEIGHT + UV_HEIGHT * offset[i][1]);              
                
                        for(int j = 3; j < 6; j++) mesh.push_back(verticies[i][j] + cord[j - 3]);   // Vertex 2
                        mesh.push_back(uv[i][2] * UV_WIDTH + UV_WIDTH * offset[i][0]);               
                        mesh.push_back(uv[i][3] * UV_HEIGHT + UV_HEIGHT * offset[i][1]);   
                        
                        for(int j = 6; j < 9; j++) mesh.push_back(verticies[i][j] + cord[j - 6]);   // Vertex 3
                        mesh.push_back(uv[i][4] * UV_WIDTH + UV_WIDTH * offset[i][0]);               
                        mesh.push_back(uv[i][5] * UV_HEIGHT + UV_HEIGHT * offset[i][1]);   
                
                        // Second triangle
                        for(int j = 9; j < 12; j++) mesh.push_back(verticies[i][j] + cord[j - 9]);  // Vertex 4
                        mesh.push_back(uv[i][6] * UV_WIDTH + UV_WIDTH * offset[i][0]);               
                        mesh.push_back(uv[i][7] * UV_HEIGHT + UV_HEIGHT * offset[i][1]);   
                
                        for(int j = 12; j < 15; j++) mesh.push_back(verticies[i][j] + cord[j - 12]); // Vertex 5
                        mesh.push_back(uv[i][8] * UV_WIDTH + UV_WIDTH * offset[i][0]);               
                        mesh.push_back(uv[i][9] * UV_HEIGHT + UV_HEIGHT * offset[i][1]);   
                
                        for(int j = 15; j < 18; j++) mesh.push_back(verticies[i][j] + cord[j - 15]); // Vertex 6
                        mesh.push_back(uv[i][10] * UV_WIDTH + UV_WIDTH * offset[i][0]);               
                        mesh.push_back(uv[i][11] * UV_HEIGHT + UV_HEIGHT * offset[i][1]);   
                    }


                }
            }
        }

        std::cout << "Mesh size: " << mesh.size() << std::endl;

        

    }




};