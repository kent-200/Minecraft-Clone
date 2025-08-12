/*
3D Graphics Engine using shaders and OpenGL

Last Update: 2/03/2025

*/

#include "header.h"
#include "camera.h"
#include "render.h"
#include "chunk.h"
#include "chunkManager.h"
#include "atlas.h"
#include "terrainGenerator.h"
#include "imguiWrapper.h"


using namespace std;

void errorCallback(int error, const char* description) {
  	std::cerr << "Error: " << description << std::endl;
}

class GameEngine3D{
private:
	Camera camera = Camera(glm::vec3{0, 64, 0});
	int windowWidth;
	int windowHeight;
	GLFWwindow* window;
	Render render;
	Atlas atlas;
	TerrainGenerator terrainGenerator = TerrainGenerator(&atlas);
	ChunkManager chunkManager = ChunkManager(terrainGenerator);
	ImGuiWrapper imGui;


public:
	GameEngine3D(int w, int h){
		windowWidth = w;
		windowHeight = h;

		// Initialize GLFW
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW" << std::endl;
			exit(-1);
		}

		// Set the error callback
		glfwSetErrorCallback(errorCallback);


		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS
		#endif



		// Create a GLFW window
		window = glfwCreateWindow(windowWidth, windowHeight, "Basic 3D Viewer", NULL, NULL);
		if (!window) {
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}

		// Make the window's context current
		glfwMakeContextCurrent(window);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
			glfwTerminate();
			exit(-1);
		}


		// Initialize ImGui wrapper
		imGui.init(window);
	

		
		// Create user resources as part of this thread
		if (!render.init(windowWidth, windowHeight)){
			std::cerr << "Failed on user create" << std::endl;
			exit(-1);
		}


	}

	



	void Run(){
		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		//mouse
		double lastX = 0.0;
		double lastY = 0.0;

		// screen size
		int screenWidth, screenHeight;
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		int fps_update = 0;
		double fps_elapsed_time = 0.0;
		double average_fps = 0.0;
		bool cursorEnabled = false;

		while (!glfwWindowShouldClose(window)){
			// Run as fast as possible

			// check if window size has changed
			int w, h;
			glfwGetFramebufferSize(window, &w, &h);
			if(w != screenWidth || h != screenHeight){
				screenWidth = w;
				screenHeight = h;
				glViewport(0, 0, screenWidth, screenHeight);
			}
			
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
			if(cursorEnabled == false){
				double mouseX, mouseY = 0.0;
				glfwGetCursorPos(window, &mouseX, &mouseY);
				double xoffset = mouseX - lastX;
				double yoffset = lastY - mouseY; // reversed since y-coordinates go from bottom to top
				lastX = mouseX;
				lastY = mouseY;

				float sensitivity = 5.0f;
				xoffset *= sensitivity;
				yoffset *= sensitivity;

				camera.fYaw -= xoffset * fElapsedTime;
				camera.fPitch += yoffset * fElapsedTime;
			}


			//stop pitch going too high or low
			if(camera.fPitch > 1.5f){
				camera.fPitch = 1.5f;
			}

			if(camera.fPitch < -1.5f){
				camera.fPitch = -1.5f;
			}

			glm::vec3 vForward = camera.lookDir * (30.0f * fElapsedTime);
			glm::vec3 vRight = { camera.lookDir.z, 0, -camera.lookDir.x };
			vRight = vRight * (8.0f * fElapsedTime);

			glm::vec3 vUp = { 0,1,0 };
			vUp = vUp * (8.0f * fElapsedTime);

			// Standard FPS Control scheme, but turn instead of strafe
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.pos = camera.pos + vForward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.pos = camera.pos - vForward;
			
			//pan camera left
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.pos = camera.pos + vRight;
			//pan camera right
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.pos = camera.pos - vRight;
		
			//move camera up
			if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.pos.y += 8.0f * fElapsedTime;
			//move camera down
			if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.pos.y -= 8.0f * fElapsedTime;
			
			//escape
			if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

			// toggle cursor
			if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
				if(cursorEnabled){
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					cursorEnabled = false;
				} else {
					cursorEnabled = true;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					// unlock cursor
				}
				// stop further key presses
			}
			


			// Start the ImGui frame and render UI
			imGui.newFrame();
			imGui.renderUI(camera.pos, camera.fYaw, camera.fPitch, average_fps);

			// Handle Frame Update

			//update screen
			// Enable the vertex array functionality
			glEnableClientState(GL_VERTEX_ARRAY);

			// Set the background color to white
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// render 3d scene
			// use chunk manager to render
			chunkManager.renderWorld(render, camera.pos, camera.viewMatrix());


			// Render ImGui ontop
			imGui.render();


			// Disable the vertex array functionality
			glDisableClientState(GL_VERTEX_ARRAY);
			// Swap buffers
			glfwSwapBuffers(window);
			// Poll for and process events
			glfwPollEvents();
				
		}

		imGui.shutdown();


		

		// call destructor for render
		render.destroy();

    	return;
	}

};




int main(){
	GameEngine3D game(1200, 800);

	game.Run();

    return 0;
}
