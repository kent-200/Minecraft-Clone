#include "game.h"

// g++ -o run game.cpp text.cpp -lglfw3 -lkernel32 -lopengl32 -lglu32 -lglew32 


//cubes
//cubeface
//cubetype
//chunk
//world
//player
//camera



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

    //triangles and face colours - all 12 triangles
    void getCubeData(std::vector<Triangle> & tris, Vec3d pos, int type_id){
        Block::allRelativeTriangles(tris, pos, atlas.getBlockTextures(type_id));
    }

};





class Renderer {
    

    //mesh
    Mesh mesh;

	//3d renderer
	Process3D process3D;

    GLFWRender render;


    //projection matrix
    //rotation matrix

public:
    Renderer(){}
  
	void init(int winWidth, int winHeight){
		process3D.init(winWidth, winHeight);
        render.init();		
	}

    


    void render_process(GLFWwindow * window, Camera camera, float fElapsedTime){
        // Fill the triangles to draw
        for (auto cube : mesh.blocks){
            mesh.getCubeData(mesh.tris, cube.pos, cube.type_id);
        }

		std::vector<std::array<float, 3>> coloursToDraw;
		std::vector<float> drawPoints;

        // Render triangles as 3D
		process3D.Render3D(mesh.tris, camera.matView(), camera.getPos(), coloursToDraw, drawPoints);
        // Render3D(window, camera, coloursToDraw, drawPoints);

		// Render triangles as 2D
		render.render(window, drawPoints, coloursToDraw);

		drawPoints.clear();
		coloursToDraw.clear();
		mesh.tris.clear();

    }



    ~Renderer() {}



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
		glfwSetErrorCallback(GLFWRender::errorCallback);

        
        // Create a GLFW window
        window = glfwCreateWindow(winWidth, winHeight, "OpenGL Triangle", NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }


        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Initialize ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();
	
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130"); // GLSL version


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

        // happen after glfw and glew init 
        renderer.init(winWidth, winHeight);



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
        float average_fps = 0.0;


        while (!glfwWindowShouldClose(window)) {
            // Handle Timing
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			float fElapsedTime = elapsedTime.count();

			// Update average FPS
			fps_update++;
			if (fps_update > 50){
				average_fps = 50.0f / fps_elapsed_time;
				fps_update = 0;
				fps_elapsed_time = 0.0;
			} else {
				fps_elapsed_time += fElapsedTime;
			}


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
            if(camera.processKeyboardInput(window, fElapsedTime)){
                // close window
                glfwSetWindowShouldClose(window, true);
            }

			//camera.printCord();

            // Start the ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Set ImGui window position and size
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(250, 80), ImGuiCond_Always);
		
			// Your ImGui UI code here
			ImGui::Begin("Camera and Performance", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Text("Position: (%.2f, %.2f, %.2f)", camera.getPos().x, camera.getPos().y, camera.getPos().z);
			ImGui::Text("Yaw = %.2f, Pitch = %.2f", camera.getYaw(), camera.getPitch());
			ImGui::Text("FPS: %.1f (%.1f)",  average_fps, ImGui::GetIO().Framerate);
			ImGui::End();
			
			// Rendering
			ImGui::Render();

            //update screen
			// Enable the vertex array functionality
			glEnableClientState(GL_VERTEX_ARRAY);


            // Handle Frame Update
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Render the scene
            renderer.render_process(window, camera, fElapsedTime);
            //draw_texts();


            // Render ImGui ontop
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            

            // Swap buffers
			glfwSwapBuffers(window);
			// Poll for and process events
			glfwPollEvents();            
        }
        return;
    }

    ~Game() {
        ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();


        // Terminate GLFW
        glfwTerminate();
    }


};




int main(void){
    Game game(1200, 800);
    game.run();
    return 0;
}