#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <array>



class GLFWRender {
private:
    //shader variables for triangles
    GLuint triShaderProgram;
    GLuint triVAO, triVBO, triEBO;
    GLuint triTexture;   

    //shaders
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

    void objectShaderInit() {
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



    void renderIndivTri(GLFWwindow * window, GLint & color_loc, GLuint & vao, GLuint & shader_programme, int pointStart, int pointNum, std::array<float, 3> color){
        // Draw triangle (color)
        glUniform4fv(color_loc, 1, &color[0]);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, pointStart, pointNum);
    }


  
    
    void renderAll(GLFWwindow * window, std::vector<float> & drawPoints, std::vector<std::array<float, 3>> & colours){        
        // Enable lighting
        // glEnable(GL_LIGHTING);
        // glEnable(GL_LIGHT0);

        // // Set material properties
        // GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
        // GLfloat diffuseColor[] = {0.8f, 0.8f, 0.8f, 1.0f};
        // GLfloat specularColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        // GLfloat shininess = 32.0f;

        // glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
        // glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
        // glMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);
        // glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        // // Set light position
        // GLfloat lightPosition[] = {1.0f, 1.0f, 1.0f, 0.0f}; // Directional light from top-right
        // glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        //print colours
        // std::cout << "start2\n";
        // for(int i = 0; i < colours.size(); i++){
        //     std::cout << "x: " << colours[i].x << " y: " << colours[i].y << " z: " << colours[i].z << std::endl;
        // }
        // std::cout << "end2\n";

        //put points into single array
        // std::vector<float> drawPoints;
        // for(int i = 0; i < (int) points.size(); i++){
        //     for(int j = 0; j < 9; j++){
        //         drawPoints.push_back(points[i][j]);
        //     }
        // }


        // Bind VBO and update vertex data
        // load all triangle points into VBO
        glBindBuffer(GL_ARRAY_BUFFER, triVBO);
        if (!drawPoints.empty()) {
            glBufferData(GL_ARRAY_BUFFER, drawPoints.size() * sizeof(float), drawPoints.data(), GL_DYNAMIC_DRAW);
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

        for(int i = 0; i < (int) colours.size(); i++){
			colours[i][0] -= 0.02f * i;
            renderIndivTri(window, color_loc, triVAO, triShaderProgram, i * 3, 3, colours[i]);
        }

        glBindVertexArray(0);

        // Disable lighting after rendering
        // glDisable(GL_LIGHTING);
    }


    



public:
    GLFWRender(){}

    static void errorCallback(int error, const char* description) {
        std::cerr << "Error: " << description << std::endl;
    }

    void init(){    
        // initialize shader for triangles
        objectShaderInit();
    }


    void render(GLFWwindow * window, std::vector<float> & drawPoints, std::vector<std::array<float, 3>> & coloursToDraw){
        renderAll(window, drawPoints, coloursToDraw);
    }

    ~GLFWRender() {
        //clean up
        glDeleteVertexArrays(1, &triVAO);
        glDeleteBuffers(1, &triVBO);
        glDeleteBuffers(1, &triEBO);
        glDeleteProgram(triShaderProgram);
    }
};
