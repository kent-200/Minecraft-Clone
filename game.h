#pragma once
#include "text.h"
#include "stb_image.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <array>
#include <glm/glm.hpp>

extern const char *stbi_failure_reason(void);
extern void stbi_image_free(void *ptr);
extern stbi_uc * stbi_load(const char *filename, int *x, int *y, int *comp, int req_comp);
extern void stbi_set_flip_vertically_on_load(int flag);
extern unsigned char *stbi_load(const char *filename, int *x, int *y, int *channels, int desired_channels);
extern void stbi_image_free(void *ptr);

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void checkGLError(const char* location) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error at " << location << ": " << error << std::endl;
    }
}


class Vec3d {
public:
    float x, y, z;
    float w = 1;

    Vec3d() : x(0), y(0), z(0) {}
    Vec3d(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3d operator+(const Vec3d& v) const { return Vec3d(x + v.x, y + v.y, z + v.z); }
    Vec3d operator-(const Vec3d& v) const { return Vec3d(x - v.x, y - v.y, z - v.z); }
    Vec3d operator*(const float& f) const { return Vec3d(x * f, y * f, z * f); }
    Vec3d operator/(const float& f) const { return Vec3d(x / f, y / f, z / f); }

    Vec3d& operator+=(const Vec3d& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3d& operator-=(const Vec3d& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3d& operator*=(const float& f) { x *= f; y *= f; z *= f; return *this; }
    Vec3d& operator/=(const float& f) { x /= f; y /= f; z /= f; return *this; }

    Vec3d cross(const Vec3d& v) const { return Vec3d(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    float dot(const Vec3d& v) const { return x * v.x + y * v.y + z * v.z; }
    float length() const { return sqrtf(x * x + y * y + z * z); }
    Vec3d normalize() const { return *this / length(); }
    Vec3d& normalize() { return *this /= length(); }
    Vec3d reflect(const Vec3d& normal) const { return *this - normal * 2 * dot(normal); }
    float distance(const Vec3d& v) const { return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z)); }
};

class Vec2d {
public:
    float x, y;

    Vec2d() : x(0), y(0) {}
    Vec2d(float x, float y) : x(x), y(y) {}

    Vec2d operator+(const Vec2d& v) const { return Vec2d(x + v.x, y + v.y); }
    Vec2d operator-(const Vec2d& v) const { return Vec2d(x - v.x, y - v.y); }
    Vec2d operator*(const float& f) const { return Vec2d(x * f, y * f); }
    Vec2d operator/(const float& f) const { return Vec2d(x / f, y / f); }

    Vec2d& operator+=(const Vec2d& v) { x += v.x; y += v.y; return *this; }
    Vec2d& operator-=(const Vec2d& v) { x -= v.x; y -= v.y; return *this; }
    Vec2d& operator*=(const float& f) { x *= f; y *= f; return *this; }
    Vec2d& operator/=(const float& f) { x /= f; y /= f; return *this; }

    float dot(const Vec2d& v) const { return x * v.x + y * v.y; }
    float length() const { return sqrtf(x * x + y * y); }
    Vec2d normalize() const { return *this / length(); }
    Vec2d& normalize() { return *this /= length(); }
    float distance(const Vec2d& v) const { return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y)); }
};

class Triangle {

};

//cubes
//cubeface
//cubetype
//chunk
//world
//player
//camera


class Mesh {
public:
    std::vector<Triangle> tris;
};



class Renderer {
    //projection matrix
    //rotation matrix

    const char* vertex_shader = R"(
        #version 330 core
        layout (location = 0) in vec3 vp;
        void main() {
            gl_Position = vec4(vp, 1.0);
        }
    )";
    
    const char* fragment_shader = R"(
        #version 330 core
        out vec4 frag_color;
        uniform vec4 u_color;
        void main() {
            frag_color = u_color;
        }
    )";

    void renderIndivTri(GLFWwindow * window, GLint & color_loc, GLuint & vao, GLuint & shader_programme, int pointStart, int pointNum, float color[4]){
        // Draw triangle (color)
        glUniform4fv(color_loc, 1, color);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, pointStart, pointNum);
    }



public:
    void triangleShaderInit(GLuint & vao, GLuint & vbo, GLuint & shader_programme) {
        // add some geometry
        float points[] = {
            -0.5f, -0.5f,  0.0f,
                0.5f, -0.5f,  0.0f,
                0.0f,  0.5f,  0.0f
        };

        
        // Create vertex array object (VAO)
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Create vertex buffer object (VBO) for dynamic points
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);

        // Define vertex attribute pointer
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        // shader for the geometry
        {
            
            GLuint vs = glCreateShader (GL_VERTEX_SHADER);
            glShaderSource (vs, 1, &vertex_shader, NULL);
            glCompileShader (vs);

            GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
            glShaderSource (fs, 1, &fragment_shader, NULL);
            glCompileShader (fs);

            shader_programme = glCreateProgram ();
            glAttachShader (shader_programme, fs);
            glAttachShader (shader_programme, vs);
            glLinkProgram (shader_programme);


            glDeleteShader(vs);
            glDeleteShader(fs);
        }

    }


    void triangleRender(GLFWwindow * window, GLuint & vao, GLuint & vbo, GLuint & shader_programme){

        // (x, y, z) (vertex2) (vertex3)
        std::array<float, 27> points{
            //first triangle
            -0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,

            //second triangle
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
            0.5f, 0.0f, 0.0f,

            //third triangle
            -0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f,
            -0.5f, 0.0f, 0.0f     
        };


        //0.0f + 0.5f * sinf(frame / 100.0f),  0.5f + 0.5f * cosf(frame / 100.0f), 0.0f,
        // -0.5f + 0.5f * sinf(frame / 100.0f),  0.5f + 0.5f * cosf(frame / 100.0f), 0.0f
        // 0.5f + 0.5f * sinf(frame / 100.0f), -0.5f + 0.5f * cosf(frame / 100.0f), 0.0f,
        // 0.0f + 0.5f * sinf(frame / 100.0f),  0.5f + 0.5f * cosf(frame / 100.0f), 0.0f
        

        // Bind VBO and update vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if (!points.empty()) {
            glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_DYNAMIC_DRAW);
        }
            
        // draw geometry
        glUseProgram(shader_programme);

        // only needs to be checked once
        GLint color_loc = glGetUniformLocation(shader_programme, "u_color");
        if (color_loc == -1) {
            std::cerr << "Error: Uniform 'u_color' red not found in shader program" << std::endl;
            return;
        }

        
        // Draw first triangle (red)
        float red_color[4] = {1.0f, 0.0f, 0.0f, 1.0f}; // Red color
        renderIndivTri(window, color_loc, vao, shader_programme, 0, 3, red_color);
        


        // Draw second triangle (blue)
        float blue_color[4] = {0.0f, 0.0f, 1.0f, 1.0f}; // Blue color
        renderIndivTri(window, color_loc, vao, shader_programme, 3, 3, blue_color);

        // Draw third triangle (green)
        float green_color[4] = {0.0f, 1.0f, 0.0f, 1.0f}; // Green color
        renderIndivTri(window, color_loc, vao, shader_programme, 6, 3, green_color);
        
    }
};




class Game {
private:
    GLFWwindow* window;
    Renderer renderer;  
    int winWidth, winHeight; 

    //shader variables for triangles
    GLuint triShaderProgram;
    GLuint triVAO, triVBO, triEBO;
    GLuint triTexture;   

    //text
    const char* atlas_image = "DS-DIGI-atlas.png";
    const char* atlas_meta = "DS-DIGI-atlas.meta";



public:
    Game(int winWidth, int winHeight) {
        this->winWidth = winWidth;
        this->winHeight = winHeight;

        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(-1);
        }

        
        // Create a GLFW window
        window = glfwCreateWindow(winWidth, winHeight, "OpenGL Triangle", NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);


        //insert - issue
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW\n";
            exit(-1);
        }
        const GLubyte* glRenderer = glGetString (GL_RENDERER);
        const GLubyte* glVersion = glGetString (GL_VERSION);
        printf ("Renderer: %s\n", glRenderer);
        printf ("OpenGL version: %s\n", glVersion);



        // initialize shader for triangles
        renderer.triangleShaderInit(triVAO, triVBO, triShaderProgram);
        // init_text_rendering (atlas_image, atlas_meta, winWidth, winHeight);
        // int hello_id = add_text (
        //     "Hello world!",
        //     -0.9f, 0.5f, 35.0f, 0.5f, 0.5f, 1.0f, 1.0f);
        
        // //position of text on screen
        // move_text (hello_id, -1.0f, 0.0f);


    }


    void run(){
        // Main loop
        while (!glfwWindowShouldClose(window)) {
            
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT);
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Render the scene
            renderer.triangleRender(window, triVAO, triVBO, triShaderProgram);
            //draw_texts();
            

            // Poll for and process events
            glfwPollEvents();
            glfwSwapBuffers (window);
            
        }
        return;
    }

    ~Game() {
        //clean up
        glDeleteVertexArrays(1, &triVAO);
        glDeleteBuffers(1, &triVBO);
        glDeleteBuffers(1, &triEBO);
        glDeleteProgram(triShaderProgram);



        // Terminate GLFW
        glfwTerminate();
    }


};






