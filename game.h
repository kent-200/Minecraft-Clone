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
    Vec3d(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

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
public:
    Vec3d point[3];
    Vec3d normal;
    float color[4] = {0, 0, 0, 0};

    Triangle() {}

    Triangle(Vec3d p1, Vec3d p2, Vec3d p3) {
        point[0] = p1;
        point[1] = p2;
        point[2] = p3;
        normal = (p2 - p1).cross(p3 - p1).normalize();
    }
};


//face of a cube
class CubeFace {
private:
    std::string name;
    //triangles are offsets for the 2 triangles
    Triangle triangles[2];
public:
    CubeFace(std::string name, Triangle t1, Triangle t2) : name(name) {
        triangles[0] = t1;
        triangles[1] = t2;
    }

    void getTriangles(std::vector<Triangle> & tris, float x, float y, float z) {
        tris.push_back(Triangle(triangles[0].point[0] + Vec3d(x, y, z), triangles[0].point[1] + Vec3d(x, y, z), triangles[0].point[2] + Vec3d(x, y, z)));
        tris.push_back(Triangle(triangles[1].point[0] + Vec3d(x, y, z), triangles[1].point[1] + Vec3d(x, y, z), triangles[1].point[2] + Vec3d(x, y, z)));
    }


};


class CubeType {
    public:
    Vec3d position;
    Vec3d color[6];

    CubeType(Vec3d position, Vec3d cfront, Vec3d cback, Vec3d cleft, Vec3d cright, Vec3d ctop, Vec3d cbottom) : position(position) {
        color[0] = cfront;
        color[1] = cback;
        color[2] = cleft;
        color[3] = cright;
        color[4] = ctop;
        color[5] = cbottom;
    }
};


class Cube {
public:
    Vec3d position;
    int type_id;

    Cube(Vec3d position, int type_id) : position(position), type_id(type_id) {}
};

//cubes
//cubeface
//cubetype
//chunk
//world
//player
//camera

class Camera {
public:
    Vec3d position = {0, 0, 0};
    float pitch = 0;
    float yaw = 0;
};


class Mesh {
public:
    std::vector<Triangle> tris;
};



class Renderer {
    //faces for drawing cubes
    std::vector<CubeFace> faces;
    std::vector<CubeType> cubeTypes;

    //shader variables for triangles
    GLuint triShaderProgram;
    GLuint triVAO, triVBO, triEBO;
    GLuint triTexture;   

    //text
    const char* atlas_image = "DS-DIGI-atlas.png";
    const char* atlas_meta = "DS-DIGI-atlas.meta";



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

    //block size
    void addFaces(float bs){
        // using clockwise ordering for vertices, later normalisation
        faces.push_back(CubeFace("front", Triangle(Vec3d(0, 0, 0), Vec3d(0, bs, 0), Vec3d(bs, 0, 0)), Triangle(Vec3d(0, bs, 0), Vec3d(bs, bs, 0), Vec3d(bs, 0, 0))));
        faces.push_back(CubeFace("back", Triangle(Vec3d(bs, 0, bs), Vec3d(0, 0, bs), Vec3d(0, bs, bs)), Triangle(Vec3d(bs, 0, bs), Vec3d(0, bs, bs), Vec3d(bs, bs, bs))));
        faces.push_back(CubeFace("left", Triangle(Vec3d(0, 0, 0), Vec3d(0, 0, bs), Vec3d(0, bs, bs)), Triangle(Vec3d(0, 0, 0), Vec3d(0, bs, bs), Vec3d(0, bs, 0))));
        faces.push_back(CubeFace("right", Triangle(Vec3d(bs, 0, 0), Vec3d(bs, bs, 0), Vec3d(bs, 0, bs)), Triangle(Vec3d(bs, 0, bs), Vec3d(bs, bs, 0), Vec3d(bs, bs, bs))));
        faces.push_back(CubeFace("top", Triangle(Vec3d(bs, bs, 0), Vec3d(0, bs, 0), Vec3d(0, bs, bs)), Triangle(Vec3d(bs, bs, 0), Vec3d(0, bs, bs), Vec3d(bs, bs, bs))));
        faces.push_back(CubeFace("bottom", Triangle(Vec3d(bs, 0, bs), Vec3d(0, 0, bs), Vec3d(0, 0, 0)), Triangle(Vec3d(bs, 0, bs), Vec3d(0, 0, 0), Vec3d(bs, 0, 0))));
    }

    void addTypes(){
        cubeTypes.push_back(CubeType(Vec3d(0, 0, 0), Vec3d(1, 0, 0, 1), Vec3d(0, 1, 0, 1), Vec3d(0, 0, 1, 1), Vec3d(1, 1, 0, 1), Vec3d(0, 1, 1, 1), Vec3d(1, 0, 1, 1)));
    }

    void renderIndivTri(GLFWwindow * window, GLint & color_loc, GLuint & vao, GLuint & shader_programme, int pointStart, int pointNum, float color[4]){
        // Draw triangle (color)
        glUniform4fv(color_loc, 1, color);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, pointStart, pointNum);
    }


    glm::mat4 getProjectionMatrix(float yaw, float pitch, float fov, float aspectRatio, float nearPlane, float farPlane) {
        // convert fov to radians
        float fovRad = fov * 3.14159265358979323846 / 180.0;

        // calculate focal length
        float f = 1.0 / tan(fovRad / 2.0);

        // calculate aspect ratio corrected fov
        float arFov = aspectRatio * f;

        // calculate sin and cos of yaw and pitch
        float cosYaw = cos(yaw);
        float sinYaw = sin(yaw);
        float cosPitch = cos(pitch);
        float sinPitch = sin(pitch);

        // create projection matrix
        glm::mat4 projectionMatrix = {
            arFov, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (farPlane + nearPlane) / (nearPlane - farPlane), -1,
            0, 0, (2 * farPlane * nearPlane) / (nearPlane - farPlane), 0
        };

        return projectionMatrix;
    }


    
    void triangleRender(GLFWwindow * window, std::vector<float> & points, std::vector<Vec3d> & colours){        
        //print colours
        std::cout << "start2\n";
        for(int i = 0; i < colours.size(); i++){
            std::cout << "x: " << colours[i].x << " y: " << colours[i].y << " z: " << colours[i].z << std::endl;
        }
        std::cout << "end2\n";


        // Bind VBO and update vertex data
        glBindBuffer(GL_ARRAY_BUFFER, triVBO);
        if (!points.empty()) {
            glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_DYNAMIC_DRAW);
        }
            
        // draw geometry
        glUseProgram(triShaderProgram);

        // only needs to be checked once
        GLint color_loc = glGetUniformLocation(triShaderProgram, "u_color");
        if (color_loc == -1) {
            std::cerr << "Error: Uniform 'u_color' not found in shader program" << std::endl;
            exit(0);
            return;
        }

        // draw triangles from array
        float color[4];

        for(int i = 0; i < colours.size(); i++){
            color[0] = colours[i].x;
            color[1] = colours[i].y;
            color[2] = colours[i].z;
            color[3] = colours[i].w;

            renderIndivTri(window, color_loc, triVAO, triShaderProgram, i * 3, 3, color);
        }

        glBindVertexArray(0);

        
    }

    //triangles and face colours
    void getCubeData(std::vector<Triangle> & tris, std::vector<Vec3d> & colors, int face, Vec3d pos, int type_id){
        //iterate through faces - adds 2 triangles
        faces[face].getTriangles(tris, pos.x, pos.y, pos.z);
        //add twice for 2 trianges
        colors.push_back(cubeTypes[type_id].color[face]);
        colors.push_back(cubeTypes[type_id].color[face]);
    }



public:
    Renderer(){
        addFaces(1.0f);
        addTypes();
    }
    //return rotation matrix

    void triangleShaderInit() {
        // add some geometry
        float points[] = {
            -0.5f, -0.5f,  0.0f,
                0.5f, -0.5f,  0.0f,
                0.0f,  0.5f,  0.0f
        };

        
        // Create vertex array object (VAO)
        glGenVertexArrays(1, &triVAO);
        glBindVertexArray(triVAO);

        // Create vertex buffer object (VBO) for dynamic points
        glGenBuffers(1, &triVBO);
        glBindBuffer(GL_ARRAY_BUFFER, triVBO);
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

            triShaderProgram = glCreateProgram ();
            glAttachShader (triShaderProgram, fs);
            glAttachShader (triShaderProgram, vs);
            glLinkProgram (triShaderProgram);


            glDeleteShader(vs);
            glDeleteShader(fs);
        }

    }




    void render_process(std::vector<Cube> cubes, GLFWwindow * window, Camera camera){
        //all points and colours
        std::vector<float> allPoints;
        std::vector<Vec3d> allColors;

        // aspect ratio
        float aspectRatio = 800.0f / 600.0f;

        // projection matrix
        glm::mat4 projectionMatrix = getProjectionMatrix(camera.yaw, camera.pitch, 90.0f, aspectRatio, 0.1f, 100.0f);


        // caclulate points for drawing cube
        // for all cubes
        for (int cube_index = 0; cube_index < cubes.size(); cube_index++) {
            // get data- triangles and colours
            std::vector<Triangle> tris;
            //one colour per triangle
            std::vector<Vec3d> colors;

            //get relative block positon
            Vec3d pos = cubes[cube_index].position - camera.position;

            // check if cube is in view with dot product
            if (pos.dot(camera.position) < 0) {
                continue;
            }

            // only draw the faces that are visible
            //currently based on players current position, will update to using normals

            if(pos.z > 0){
                getCubeData(tris, colors, 0, pos, cubes[cube_index].type_id); //front
            } else if(pos.z < 0){
                getCubeData(tris, colors, 1, pos, cubes[cube_index].type_id); //back
            }

            if(pos.x > 0){
                getCubeData(tris, colors, 2, pos, cubes[cube_index].type_id); //left
            } else if(pos.x < 0){
                getCubeData(tris, colors, 3, pos, cubes[cube_index].type_id); //right
            }

            if(pos.y > 0){
                getCubeData(tris, colors, 4, pos, cubes[cube_index].type_id); //top
            } else if(pos.y < 0){
                getCubeData(tris, colors, 5, pos, cubes[cube_index].type_id); //bottom
            }

            //print colours
            std::cout << "start\n";
            for(int i = 0; i < colors.size(); i++){
                std::cout << "x: " << colors[i].x << " y: " << colors[i].y << " z: " << colors[i].z << std::endl;
            }
            std::cout << "end\n";


            //dont bother drawing block if no faces are on screen

            // add to all points and colours
            // for all triangles
            for(int tri_index = 0; tri_index < tris.size(); tri_index++){
                bool drawTri = false;
                
                //point 1, 2, and 3
                
                Vec2d vertex[3];

                // project the triangles
                glm::vec4 projected_point_1 = projectionMatrix * glm::vec4(tris[tri_index].point[0].x, tris[tri_index].point[0].y, tris[tri_index].point[0].z, tris[tri_index].point[0].w);
                glm::vec4 projected_point_2 = projectionMatrix * glm::vec4(tris[tri_index].point[1].x, tris[tri_index].point[1].y, tris[tri_index].point[1].z, tris[tri_index].point[1].w);
                glm::vec4 projected_point_3 = projectionMatrix * glm::vec4(tris[tri_index].point[2].x, tris[tri_index].point[2].y, tris[tri_index].point[2].z, tris[tri_index].point[2].w);

                //project to 2d
                //point 1,2,3
                //normalise with gl
                vertex[0].x = (projected_point_1.x / -projected_point_1.w) * 0.5 + 0.5;
                vertex[0].y = (projected_point_1.y / -projected_point_1.w) * 0.5 + 0.5;
                if(vertex[0].x > -1 && vertex[0].x < 1 && vertex[0].y > -1 && vertex[0].y < 1){
                    drawTri = true;
                }

                vertex[1].x = (projected_point_2.x / -projected_point_2.w) * 0.5 + 0.5;
                vertex[1].y = (projected_point_2.y / -projected_point_2.w) * 0.5 + 0.5;
                if(vertex[1].x > -1 && vertex[1].x < 1 && vertex[1].y > 0 && vertex[1].y < 1){
                    drawTri = true;
                }

                vertex[2].x = (projected_point_3.x / -projected_point_3.w) * 0.5 + 0.5;
                vertex[2].y = (projected_point_3.y / -projected_point_3.w) * 0.5 + 0.5;
                if(vertex[2].x > -1 && vertex[2].x < 1 && vertex[2].y > -1 && vertex[2].y < 1){
                    drawTri = true;
                }
                // save to a points array to be drawn later

                //print all points
                // for(int i = 0; i < 3; i++){
                //     std::cout << "x: " << vertex[i].x << " y: " << vertex[i].y << std::endl;
                // }

                if(drawTri){
                    for(int i = 0; i < 3; i++){
                        allPoints.push_back(vertex[i].x);
                        allPoints.push_back(vertex[i].y);
                        allPoints.push_back(0.0f);
                    }
                     allColors.push_back(colors[tri_index]);
                }
            }

            //triangle ready to be drawn by a drawing function
        }

        // Draw triangles from allPoints and allColors

        triangleRender(window, allPoints, allColors);       
    }

    ~Renderer() {
        //clean up
        glDeleteVertexArrays(1, &triVAO);
        glDeleteBuffers(1, &triVBO);
        glDeleteBuffers(1, &triEBO);
        glDeleteProgram(triShaderProgram);
    }



};




class Game {
private:
    GLFWwindow* window;
    Renderer renderer;  
    int winWidth, winHeight; 
    Camera camera;

    std::vector<Cube> cubes;

    


public:
    Game(int winWidth, int winHeight) {
        this->winWidth = winWidth;
        this->winHeight = winHeight;

        // add cube
        cubes.push_back(Cube(Vec3d(2, -1, -3), 0));

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
        renderer.triangleShaderInit();
        // init_text_rendering (atlas_image, atlas_meta, winWidth, winHeight);
        // int hello_id = add_text (
        //     "Hello world!",
        //     -0.9f, 0.5f, 35.0f, 0.5f, 0.5f, 1.0f, 1.0f);
        
        // //position of text on screen
        // move_text (hello_id, -1.0f, 0.0f);


    }


    void run(){
        double oldMouseX, oldMouseY = 0.0;
        // Main loop
        while (!glfwWindowShouldClose(window)) {
            // Handle Esc input
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            // Handle key inputs
            float moveAmount = 0.005;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.position.z -= moveAmount;
            } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.position.z += moveAmount;
            } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.position.x -= moveAmount;
            } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.position.x += moveAmount;
            } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                camera.position.y += moveAmount;
            } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera.position.y -= moveAmount;
            }

            //handle mouse inputs
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            // adjust yaw after full rotation using mod
            camera.yaw += (mouseX - oldMouseX) * 0.001;
            if (camera.yaw > 3.14) camera.yaw -= 6.28;
            if (camera.yaw < -3.14) camera.yaw += 6.28;
            

            // limit pitch to avoid gimbal lock
            camera.pitch += (mouseY - oldMouseY) * 0.005;
            if (camera.pitch > 1.57) camera.pitch = 1.57;
            if (camera.pitch < -1.57) camera.pitch = -1.57;

           // std::cout << "yaw: " << camera.yaw << " pitch: " << camera.pitch << std::endl;

            oldMouseX = mouseX;
            oldMouseY = mouseY;
            

            
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT);
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Render the scene
            renderer.render_process(cubes, window, camera);
            //draw_texts();
            

            // Poll for and process events
            glfwPollEvents();
            glfwSwapBuffers (window);
            
        }
        return;
    }

    ~Game() {
        // Terminate GLFW
        glfwTerminate();
    }


};






