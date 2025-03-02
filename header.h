#pragma once

#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cmath>
#include <string>
#include <chrono>
#include <vector>
#include <array>
#include <list>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>	// for rotate

// for intersectRayPlane
#include <glm/gtx/intersect.hpp>

// include imgui
#include "lib/imgui/imgui.h"
#include "lib/imgui/backends/imgui_impl_glfw.h"
#include "lib/imgui/backends/imgui_impl_opengl3.h"





class Triangle {
public:
	glm::vec3 p[3];
	float col;

	Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		p[0] = p1;
		p[1] = p2;
		p[2] = p3;
		col = 0.5f;
	}
};




class Mesh {
public:
	std::vector<Triangle> tris;

	bool LoadFromObjectFile(std::string sFilename)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		std::vector<glm::vec3> verts;
		// no point preallocating, only done once

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::stringstream s;
			s << line;

			char type;	// either v for vertex or f for face

			if (line[0] == 'v')
			{
				glm::vec3 v;
				s >> type >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> type >> f[0] >> f[1] >> f[2];
				tris.push_back(Triangle(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
			}
		}
		return true;
	}
};

class Camera {
public:
	glm::vec3 pos;	// Location of camera in world space
	glm::vec3 lookDir;	// Direction vector along the direction camera points
	float fYaw = 0.0f;		// FPS Camera rotation in XZ plane
	float fPitch = 0.0f;	// FPS Camera rotation in YZ plane

	Camera() = default;
	Camera(glm::vec3 pos) : pos(pos) {
		//look at z by default
		lookDir = {0,0,1};
	}


	glm::mat4 matView() {
		// Calculate the forward vector from yaw and pitch angles
		glm::vec3 forward;
		forward.x = cos(fPitch) * sin(fYaw);
		forward.y = sin(fPitch);
		forward.z = cos(fPitch) * cos(fYaw);
	
		// Normalize the forward vector to get the look direction
		lookDir = glm::normalize(forward);
	
		// Define the up vector (world up direction)
		glm::vec3 up = {0.0f, 1.0f, 0.0f};
	
		// Calculate the right vector
		glm::vec3 right = glm::normalize(glm::cross(up, lookDir));
	
		// Recalculate the orthogonal up vector
		up = glm::cross(lookDir, right);
	
		// Calculate the target point the camera is looking at
		glm::vec3 target = pos + lookDir;
	
		// Create and return the view matrix using glm::lookAt
		return glm::lookAt(pos, target, up);
	}
};