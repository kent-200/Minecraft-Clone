#include "game.h"

// g++ -o run game.cpp text.cpp -lglfw3 -lkernel32 -lopengl32 -lglu32 -lglew32 

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void checkGLError(const char* location) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error at " << location << ": " << error << std::endl;
    }
}


//cubes
//cubeface
//cubetype
//chunk
//world
//player
//camera

bool Camera::processKeyboardInput(GLFWwindow * window, float fElapsedTime){  
    Vec3d vTarget = { 0,0,1 };
    Mat4 matRotX = Mat4::makeRotationX(fPitch);
    Mat4 matRotY = Mat4::makeRotationY(fYaw);
    Mat4 matCameraRot = matRotX * matRotY;
    lookDir = matCameraRot * vTarget;


    Vec3d vForward = lookDir * (keySensitive * fElapsedTime);
    Vec3d vRight = { lookDir.z, 0, -lookDir.x };
    vRight = vRight * (keySensitive * fElapsedTime);

    Vec3d vUp = { 0,1,0 };
    vUp = vUp * (keySensitive * fElapsedTime);

    // Standard FPS Control scheme, but turn instead of strafe
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        pos = pos + vForward;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        pos = pos - vForward;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        //pan camera left
        pos = pos + vRight;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        //pan camera right
        pos = pos - vRight;
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        //move camera up
        pos.y += (fElapsedTime * keySensitive);
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        //move camera down
        pos.y -= (fElapsedTime * keySensitive);
    }

    //escape
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        return true;
    }

    return false;    
}



class Mesh {
private:
    blockAtlas atlas;

    void addTypes(){
        atlas.addBlock("colour", Vec3d(1, 0, 0, 1), Vec3d(0, 1, 0, 1), Vec3d(0, 0, 1, 1), Vec3d(1, 1, 0, 1), Vec3d(0, 1, 1, 1), Vec3d(1, 0, 1, 1));
		//grey cube, top light, side middle, bottom dark
		atlas.addBlock("grey", Vec3d(0.7, 0.7, 0.7, 1), Vec3d(0.7, 0.7, 0.7, 1), Vec3d(0.6, 0.6, 0.6, 1), Vec3d(0.6, 0.6, 0.6, 1), Vec3d(0.8, 0.8, 0.8, 1), Vec3d(0.2, 0.2, 0.2, 1));
    }

public:
    std::vector<Block> blocks;
    std::vector<Triangle> tris;
    Mesh(){
        addTypes();

        // add cube
        blocks.push_back(Block(Vec3d(0, 0, -3), 0));
		blocks.push_back(Block(Vec3d(2, 2, 0), 1));
		blocks.push_back(Block(Vec3d(0, 0, -1), 1));

		blocks.push_back(Block(Vec3d(2, 2, 5), 1));
		blocks.push_back(Block(Vec3d(0, 0, 5), 1));

		blocks.push_back(Block(Vec3d(-2, -2, 0), 1));
		blocks.push_back(Block(Vec3d(5, 8, -1), 1));


    }

    //triangles and face colours
    void getCubeData(std::vector<Triangle> & tris, int face, Vec3d pos, int type_id){
        //iterate through faces - adds 2 triangles
		Block::faceRelativeTriangles(tris, face, pos, atlas.getBlockTexture(type_id, face));
        // faces[face].getTriangles(tris, pos, cubeTypes[type_id].color[face]);
    }



};





class Renderer {
    //shader variables for triangles
    GLuint triShaderProgram;
    GLuint triVAO, triVBO, triEBO;
    GLuint triTexture;   

    //mesh
    Mesh mesh;

	//3d renderer
	Process3D process3D;


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
    Renderer(){}
  
	bool projectionInit(int winWidth, int winHeight){
		process3D.init(winWidth, winHeight);
		return true;
	}

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


    void render_process(GLFWwindow * window, Camera camera, float fElapsedTime){
        // Fill the triangles to draw
        for (auto cube : mesh.blocks){
            for (int i = 0; i < 6; i++){
                mesh.getCubeData(mesh.tris, i, cube.pos, cube.type_id);
            }
        }

		std::vector<std::array<float, 3>> coloursToDraw;
		std::vector<float> drawPoints;

        // Render triangles as 3D
		process3D.Render3D(mesh.tris, camera.matView(), camera.getPos(), coloursToDraw, drawPoints);
        // Render3D(window, camera, coloursToDraw, drawPoints);

		// Render triangles as 2D
		renderAll(window, drawPoints, coloursToDraw);

		drawPoints.clear();
		coloursToDraw.clear();
		mesh.tris.clear();

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
    Camera camera = Camera(Vec3d(0, 0, -5));

    

    


public:
    Game(int winWidth, int winHeight) {
        this->winWidth = winWidth;
        this->winHeight = winHeight;
        

        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(-1);
        }

        // Set the error callback
		glfwSetErrorCallback(errorCallback);
        
        // Create a GLFW window
        window = glfwCreateWindow(winWidth, winHeight, "OpenGL Triangle", NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Create user resources as part of this thread
		if (!renderer.projectionInit(winWidth, winHeight)) {
			std::cerr << "Failed on user create" << std::endl;
			exit(-1);
		}


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
        renderer.objectShaderInit();

        // init_text_rendering (renderer.getAtlasImage(), renderer.getAtlasMeta(), winWidth, winHeight);
        // int hello_id = add_text (
        //     "Hello world!",
        //     -0.9f, 0.5f, 35.0f, 0.5f, 0.5f, 1.0f, 1.0f);
        
        // //position of text on screen
        // move_text (hello_id, -1.0f, 0.0f);


    }


    void run(){
		//sensitivity
		// camera.keySensitive = 5.0f;
		// camera.mouseSensitivity = 2.0f;

        // timing
        auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		//mouse
		double lastX = 0.0;
		double lastY = 0.0;

        // hide cursor and lock it in window
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		int fps_update = 0;
		double fps_elapsed_time = 0.0; 


        while (!glfwWindowShouldClose(window)) {
            // Handle Timing
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTime = tp2 - tp1;
            tp1 = tp2;
            float fElapsedTime = elapsedTime.count();

            //handle mouse - use change in mouse position to rotate camera
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            double xoffset = mouseX - lastX;
            double yoffset = lastY - mouseY; // reversed since y-coordinates go from bottom to top
            lastX = mouseX;
			lastY = mouseY;
            // process mouse
            camera.processMouseInput(xoffset, yoffset, fElapsedTime);

            // process keyboard
            camera.processKeyboardInput(window, fElapsedTime);

			//camera.printCord();


            // Handle Frame Update
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Render the scene
            renderer.render_process(window, camera, fElapsedTime);
            //draw_texts();
            

            // Swap buffers
			glfwSwapBuffers(window);
			// Poll for and process events
			glfwPollEvents();


            // Update Title Bar
            fps_update++;
            if (fps_update > 25){
                double average_fps = 25.0f / fps_elapsed_time;
                // Update window title with FPS
                std::string windowTitle = "GLFW game engine - FPS: " + std::to_string(average_fps);
                glfwSetWindowTitle(window, windowTitle.c_str());
                fps_update = 0;
                fps_elapsed_time = 0.0;
            } else {
                fps_elapsed_time += fElapsedTime;
            }
            
        }
        return;
    }

    ~Game() {
        // Terminate GLFW
        glfwTerminate();
    }


};




int main(void){
    Game game(1200, 800);
    game.run();
    return 0;
}