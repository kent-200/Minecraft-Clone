#pragma once
#include "vec3d.h"
#include "mat4.h"
#include <GLFW/glfw3.h>

class Camera {
	Vec3d pos;	// Location of camera in world space
	Vec3d lookDir = Vec3d(0, 0, 1);	// Direction vector along the direction camera points
	float fYaw;		// FPS Camera rotation in XZ plane
	float fPitch;	// FPS Camera rotation in YZ plane
	float mouseSensitivity = 2.0f;
	float keySensitive = 5.0f;

public:

	Camera() = default;
	Camera(Vec3d pos) : pos(pos) {
		//look at z by default
		lookDir = Vec3d(0, 0, 1);
	}

    Camera(Vec3d pos, Vec3d lookDir) : pos(pos), lookDir(lookDir) {}


	Mat4 matView(){
        // Create "Point At" Matrix for camera
        Vec3d vTarget = { 0,0,1 };
        Mat4 matRotX = Mat4::makeRotationX(fPitch);
        Mat4 matRotY = Mat4::makeRotationY(fYaw);
        Mat4 matCameraRot = matRotX * matRotY;
        lookDir = matCameraRot * vTarget;
        vTarget = pos + lookDir;
        Mat4 matCamera = Mat4::pointAt(pos, vTarget, { 0,1,0 });
        return matCamera.quickInverse();
    }

    // if window should close
    bool processKeyboardInput(GLFWwindow * window, float fElapsedTime);

    void processMouseInput(double xoffset, double yoffset, float fElapsedTime);

    Vec3d getPos(){
        return pos;
    }

    Vec3d getLookDir(){
        return lookDir;
    }

    float getYaw(){
        return fYaw;
    }

    float getPitch(){
        return fPitch;
    }

};


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


void Camera::processMouseInput(double xoffset, double yoffset, float fElapsedTime){
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    fYaw += xoffset * fElapsedTime;
    fPitch += yoffset * fElapsedTime;


    //stop pitch going too high or low
    if(fPitch > 1.5f){
        fPitch = 1.5f;
    }

    if(fPitch < -1.5f){
        fPitch = -1.5f;
    }
}