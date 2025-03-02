/*
This is a 3D Graphics engine that has been adapted from working in the console to basic openGL using GLFW3. It is based on the work of Javidx9.
https://www.github.com/onelonecoder

Compile
g++ -o run main.cpp -lglfw3 -lkernel32 -lopengl32 -lglu32 -lglew32 -Wall -lwinmm -Werror -pedantic

Last Update: 25/04/2024

*/

#include "header.h"
#include <stdexcept>


using namespace std;

void errorCallback(int error, const char* description) {
  	std::cerr << "Error: " << description << std::endl;
}

class GameEngine3D{
private:
	Mesh meshCube;
	//glm::mat4 matProj;	// Matrix that converts from view space to screen space
	Camera camera = Camera(glm::vec3{0, 0, -5});
	int windowWidth;
	int windowHeight;
	GLFWwindow* window;
	std::string filename;


	string vertexShaderPath = "shaders/shader.vert";
	string fragmentShaderPath = "shaders/shader.frag";

	GLuint shaderProgram;
	GLuint VAO, VBO;

	void shaderInit(){
		// 1. load the shader files
		string vertexShaderCode;
		string fragmentShaderCode;

		try {
			ifstream vertexShaderStream(vertexShaderPath, ios::in);
			ifstream fragmentShaderStream(fragmentShaderPath, ios::in);

			if(!vertexShaderStream.is_open()) throw std::runtime_error("Cannot open vertex shader file");
			if(!fragmentShaderStream.is_open()) throw std::runtime_error("Cannot open fragment shader file");

			stringstream sstr;
			sstr << vertexShaderStream.rdbuf();
			vertexShaderCode = sstr.str();
			vertexShaderStream.close();

			sstr.str("");
			sstr << fragmentShaderStream.rdbuf();
			fragmentShaderCode = sstr.str();
			fragmentShaderStream.close();
			
		} catch (exception e){
			std::cerr << "Error: " << e.what() << std::endl;
		}

		if (vertexShaderCode.empty() || fragmentShaderCode.empty()) {
			std::cerr << "Error: Shader code is empty." << std::endl;
			return;
		}
		

		const char * vertexShaderPtr = vertexShaderCode.c_str();
		const char * fragmentShaderPtr = fragmentShaderCode.c_str();


		// 2. compile the shaders
		int success;
		// vertex
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexShaderPtr, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cerr << "Error: Vertex Shader Compilation Failed\n" << infoLog << std::endl;
		}



		// fragment
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentShaderPtr, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cerr << "Error: Fragment Shader Compilation Failed\n" << infoLog << std::endl;
		}

		// create shader program
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertex);
		glAttachShader(shaderProgram, fragment);
		glLinkProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);	// check sucess
		if(!success){
			char infoLog[512];
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cerr << "Error: Shader Program Linking Failed\n" << infoLog << std::endl;
		}

		// delete shaders - now linked to program, no longer needed
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}


	void createBuffers(){
		GLfloat vertices[] = {
			// Positions         // Colors
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Top Right
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Right
		   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // Bottom Left
		   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f   // Top Left
		};

		// Create Vertex Array Object
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create a Vertex Buffer Object and copy the vertex data to it
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		// define the vertex attribute pointer
		// for positions - layer 0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// for colors - layer 1
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);


		// Unbind the VAO
		glBindVertexArray(0);
	}


	bool GraphicsInit(){
		// Load object file
		meshCube.LoadFromObjectFile(filename);

		if(meshCube.tris.empty()){
			std::cerr << "Error: Mesh is empty" << std::endl;
			throw std::runtime_error("Mesh is empty");
		} else {
			cout << "Loaded mesh with " << meshCube.tris.size() << " triangles" << endl;
		}

		// Create the shader program
		shaderInit();	


		// Create the vertex buffer object
		createBuffers();

		// Projection Matrix
		//matProj = glm::perspective(glm::radians(90.0f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);
		return true;
	}




public:
	GameEngine3D(int w, int h, string _filename){
		windowWidth = w;
		windowHeight = h;
		filename = _filename;

		// Initialize GLFW
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW" << std::endl;
			exit(-1);
		}

		// Set the error callback
		glfwSetErrorCallback(errorCallback);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


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
	

		
		// Create user resources as part of this thread
		if (!GraphicsInit()){
			std::cerr << "Failed on user create" << std::endl;
			exit(-1);
		}

	}

	bool Render(float fElapsedTime){
		glm::mat4 matProj = glm::perspective(glm::radians(90.0f), (float) windowWidth / (float) windowHeight, 0.1f, 1000.0f);
		glm::mat4 matWorld = glm::mat4(1.0f);	// Form World Matrix

		// Get view matrix from camera class
		glm::mat4 matView = camera.matView();	

		// Use the shader program
		glUseProgram(shaderProgram);


		// Pass matrices to the shader
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matWorld));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(matProj));


		// Prepare vertex and color data
		std::vector<float> vertexData;
		vertexData.reserve(meshCube.tris.size() * 6 * 3); // 3 vertices per triangle, 6 floats per vertex (3 for position, 3 for color)

		for (const auto& tri : meshCube.tris) {
			for (int i = 0; i < 3; ++i) {
				// Vertex positions
				vertexData.push_back(tri.p[i].x);
				vertexData.push_back(tri.p[i].y);
				vertexData.push_back(tri.p[i].z);

				// Vertex colors (assuming tri.col is a float representing grayscale intensity)
				vertexData.push_back(tri.col); // Red
				vertexData.push_back(tri.col); // Green
				vertexData.push_back(tri.col); // Blue
			}
		}

		if(vertexData.empty()){
			cout << "Vertex Data is empty" << endl;
			return false;
		}

		// 5. Update Vertex Buffer Object (VBO) - new data
		// use method: glBufferSubData not glMapBuffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);


		// 6. render the object
		glBindVertexArray(VAO);
		// draw all triangles in buffer at once
		glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 6);
		glBindVertexArray(0);

		return true;
		
	}

	void Run(){
		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		//mouse
		double lastX = 0.0;
		double lastY = 0.0;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		int fps_update = 0;
		double fps_elapsed_time = 0.0;
		double average_fps = 0.0;

		while (!glfwWindowShouldClose(window)){
			// Run as fast as possible
			
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

			float sensitivity = 5.0f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			camera.fYaw += xoffset * fElapsedTime;
			camera.fPitch += yoffset * fElapsedTime;


			//stop pitch going too high or low
			if(camera.fPitch > 1.5f){
				camera.fPitch = 1.5f;
			}

			if(camera.fPitch < -1.5f){
				camera.fPitch = -1.5f;
			}

			glm::vec3 vForward = camera.lookDir * (8.0f * fElapsedTime);
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
			


			// Start the ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Set ImGui window position and size
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(250, 80), ImGuiCond_Always);
		
			// Your ImGui UI code here
			ImGui::Begin("Camera and Performance", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::Text("Position: (%.2f, %.2f, %.2f)", camera.pos.x, camera.pos.y, camera.pos.z);
			ImGui::Text("Yaw = %.2f, Pitch = %.2f", camera.fYaw, camera.fPitch);
			ImGui::Text("FPS: %.1f (%.1f)",  average_fps, ImGui::GetIO().Framerate);
			ImGui::End();
			
			// Rendering
			ImGui::Render();

			// Handle Frame Update

			//update screen
			// Enable the vertex array functionality
			glEnableClientState(GL_VERTEX_ARRAY);

			// Set the background color to white
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// render 3d scene
			Render(fElapsedTime);


			// Render ImGui ontop
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			// Disable the vertex array functionality
			glDisableClientState(GL_VERTEX_ARRAY);
			// Swap buffers
			glfwSwapBuffers(window);
			// Poll for and process events
			glfwPollEvents();

			

				
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();


		// Clean up and exit
    	glfwTerminate();
    	return;
	}

};




int main(){
	GameEngine3D game(1200, 800, "teapot.obj");

	game.Run();

    return 0;
}
