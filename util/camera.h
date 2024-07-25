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

    void processMouseInput(double xoffset, double yoffset, float fElapsedTime){
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