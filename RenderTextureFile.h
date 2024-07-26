#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <array>

struct textureRef {
    int x;
    int y;
    bool topHalf;
    textureRef(int x, int y, bool topHalf){
        this->x = x;
        this->y = y;
        this->topHalf = topHalf;
    }
};

class RenderTextureFile {
private:
    // shaders
    const char* vertex_shader = R"(
        #version 330 core
        layout (location = 0) in vec3 vertex_position;
        layout (location = 1) in vec2 vertex_uv;
        out vec2 uv;

        void main() {
            // Apply position offset
            vec4 position = vec4(vertex_position, 1.0);
            gl_Position = position;
            uv = vertex_uv;
        }
    )";

    const char* fragment_shader = R"(
        #version 330 core
        in vec2 uv;
        out vec4 frag_color;
        uniform sampler2D basic_texture;
        void main() {
            frag_color = texture(basic_texture, uv);
        }
    )";
    


    GLuint shaderProgram;
    GLuint VAO, VBO;
    GLuint texture;

    const char * imgName;
    int numTextureWidth;   // number of assets in the image along width
    int numTextureHeight;  // number of assets in the image along height
    float textureWidthNorm;  // normalized width of each asset
    float textureHeightNorm; // normalized height of each asset

    void init();
   
public:
    RenderTextureFile(const char * imgName, int numAssetsWidth, int numAssetsHeight){
        this->imgName = imgName;
        this->numTextureWidth = numAssetsWidth;
        this->numTextureHeight = numAssetsHeight;
        this->textureWidthNorm = 1 / (float) numAssetsWidth;
        this->textureHeightNorm = 1 / (float) numAssetsHeight;
        init();
    }


    static void glfwExperimental();

    void render(GLFWwindow * window, std::vector<float> & drawPoints, std::vector<textureRef> & textureCord);

    void checkGLError(const char* location); 
    

    ~RenderTextureFile(){
        // Clean up
        // Clean up OpenGL resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        glDeleteTextures(1, &texture);
    }

};