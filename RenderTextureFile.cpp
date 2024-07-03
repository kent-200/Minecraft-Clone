#include "RenderTextureFile.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void RenderTextureFile::init(){
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader compilation status
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation status
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return;
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check shader program linking status
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return;
    }

    // Clean up shader resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data and buffers
    float vertices[] = {
        // Positions       // UVs
        0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Top Right
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Bottom Left
       // -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // Top Left
    };


    // vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex buffer object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // UV attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Unbind VAO
    glBindVertexArray(0);

    // Load texture
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imgName, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "Failed to load texture\n";
        return;
    }

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // Set texture unit 0
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "basic_texture"), 0);


    

    // Check for OpenGL errors
    checkGLError("init");

}



void RenderTextureFile::render(GLFWwindow * window, std::vector<float> & drawPoints, std::vector<textureRef> & textureCord){
    // Check if anything to draw
    // Must be triangles
    if(drawPoints.size() == 0 && drawPoints.size() % 3 != 0){
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // input into shader
    // std::vector<float> point1 = {
    //     // Positions       // UVs
    //     0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Top Right
    //     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom Right
    //     -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Bottom Left
    //    // -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // Top Left
    // };

    // draw points only contains the positions
    //uv needs to be added and calculated based on the textureX and textureY

    std::vector<float> tri1 = {
        0.98f, 0.98f, // Top Right
        0.02f, 0.02f, // Bottom Left
        0.98f, 0.02f, // Bottom Right
    };

    std::vector<float> tri2 = {
        0.98f, 0.98f, // Top Right
        0.02f, 0.02f, // Bottom Left
        0.02f, 0.98f, // Top Left
    };

    

    int numTriangles = drawPoints.size() / 9; // will always be int 

    // for every 3 (triangle x y z), add 2 for vw (img x y)
    std::vector<float> finalPoints;
    finalPoints.resize(numTriangles * 15);

    for(int i = 0; i < numTriangles; i++){
        int startingFinal = i * 15;
        int startingDraw = i * 9;

        int textx = textureCord[i].x;
        int texty = numTextureHeight - textureCord[i].y - 1;

        // x y z
        finalPoints[startingFinal] = drawPoints[startingDraw];
        finalPoints[startingFinal + 1] = drawPoints[startingDraw + 1];
        finalPoints[startingFinal + 2] = drawPoints[startingDraw + 2];

        // uv 1
        if(textureCord[i].topHalf){
            finalPoints[startingFinal + 3] = textureWidthNorm * (float)(textx + tri1[0]);
            finalPoints[startingFinal + 4] = textureHeightNorm * (float)(texty + tri1[1]);
        } else {
            finalPoints[startingFinal + 3] = textureWidthNorm * (float) (textx + tri2[0]);
            finalPoints[startingFinal + 4] = textureHeightNorm * (float) (texty + tri2[1]);
        }

        // x y z point 2
        finalPoints[startingFinal + 5] = drawPoints[startingDraw + 3];
        finalPoints[startingFinal + 6] = drawPoints[startingDraw + 4];
        finalPoints[startingFinal + 7] = drawPoints[startingDraw + 5];

        // uv 2
        if(textureCord[i].topHalf){
            finalPoints[startingFinal + 8] = textureWidthNorm * float(textx + tri1[2]);
            finalPoints[startingFinal + 9] = textureHeightNorm * float(texty + tri1[3]);
        } else {
            finalPoints[startingFinal + 8] = textureWidthNorm * float(textx + tri2[2]);
            finalPoints[startingFinal + 9] = textureHeightNorm * float(texty + tri2[3]);
        }

        // x y z point 3
        finalPoints[startingFinal + 10] = drawPoints[startingDraw + 6];
        finalPoints[startingFinal + 11] = drawPoints[startingDraw + 7];
        finalPoints[startingFinal + 12] = drawPoints[startingDraw + 8];

        // uv 3
        if(textureCord[i].topHalf){
            finalPoints[startingFinal + 13] = textureWidthNorm * float(textx + tri1[4]);
            finalPoints[startingFinal + 14] = textureHeightNorm * float(texty + tri1[5]);
        } else {
            finalPoints[startingFinal + 13] = textureWidthNorm * float(textx + tri2[4]);
            finalPoints[startingFinal + 14] = textureHeightNorm * float(texty + tri2[5]);
        }

    }

   // exit(0);
    
    glBufferData(GL_ARRAY_BUFFER, finalPoints.size() * sizeof(float), finalPoints.data(), GL_DYNAMIC_DRAW);
    
    // else draw nothing

    // Use shader program and bind VAO
    glUseProgram(shaderProgram);

    // Check if shader program is valid
    if (glGetError() != GL_NO_ERROR) {
        // Handle shader program errors
        std::cerr << "Shader program error\n";
    }


    // Bind VAO
    glBindVertexArray(VAO);
    
    // bind texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // Draw the textured quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    checkGLError("render");
}


void RenderTextureFile::checkGLError(const char* location) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error at " << location << ": " << error << std::endl;
    }
}
    
void RenderTextureFile::glfwExperimental(){
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        exit(1);
    }
}