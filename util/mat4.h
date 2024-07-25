#pragma once
#include "vec3d.h"

class Mat4 {
public:
	float m[4][4] = { 0.0f };
	Mat4() = default;

	static Mat4 makeIdentity() {
		Mat4 m;
		for (int i = 0; i < 4; i++) {
			m.m[i][i] = 1.0f;
		}
		return m;
	}

	static Mat4 makeRotationX(float fAngleRad){
		Mat4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = cosf(fAngleRad);
		matrix.m[1][2] = sinf(fAngleRad);
		matrix.m[2][1] = -sinf(fAngleRad);
		matrix.m[2][2] = cosf(fAngleRad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	static Mat4 makeRotationY(float fAngleRad){
		Mat4 matrix;
		matrix.m[0][0] = cosf(fAngleRad);
		matrix.m[0][2] = sinf(fAngleRad);
		matrix.m[2][0] = -sinf(fAngleRad);
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = cosf(fAngleRad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	static Mat4 makeRotationZ(float fAngleRad){
		Mat4 matrix;
		matrix.m[0][0] = cosf(fAngleRad);
		matrix.m[0][1] = sinf(fAngleRad);
		matrix.m[1][0] = -sinf(fAngleRad);
		matrix.m[1][1] = cosf(fAngleRad);
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	static Mat4 makeTranslation(float x, float y, float z){
		Mat4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		matrix.m[3][0] = x;
		matrix.m[3][1] = y;
		matrix.m[3][2] = z;
		return matrix;
	}

	static Mat4 makeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar){
		float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
		Mat4 matrix;
		matrix.m[0][0] = fAspectRatio * fFovRad;
		matrix.m[1][1] = fFovRad;
		matrix.m[2][2] = fFar / (fFar - fNear);
		matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matrix.m[2][3] = 1.0f;
		matrix.m[3][3] = 0.0f;
		return matrix;
	}



	Vec3d operator*(Vec3d i)
	{
		Vec3d v;
		v.x = i.x * m[0][0] + i.y * m[1][0] + i.z * m[2][0] + i.w * m[3][0];
		v.y = i.x * m[0][1] + i.y * m[1][1] + i.z * m[2][1] + i.w * m[3][1];
		v.z = i.x * m[0][2] + i.y * m[1][2] + i.z * m[2][2] + i.w * m[3][2];
		v.w = i.x * m[0][3] + i.y * m[1][3] + i.z * m[2][3] + i.w * m[3][3];
		return v;
	}
	

	Mat4 operator*(Mat4 m2){
		Mat4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m[r][0] * m2.m[0][c] + m[r][1] * m2.m[1][c] + m[r][2] * m2.m[2][c] + m[r][3] * m2.m[3][c];
		return matrix;
	}


	static Mat4 pointAt(Vec3d pos, Vec3d target, Vec3d up){
		// Calculate new forward direction
		Vec3d newForward = target - pos;
		newForward = newForward.normalise();

		// Calculate new Up direction
		Vec3d a = newForward * up.dot_product(newForward);
		Vec3d newUp = up - a;
		newUp = newUp.normalise();

		// New Right direction is easy, its just cross product
		Vec3d newRight = newUp.cross_product(newForward);

		// Construct Dimensioning and Translation Matrix	
		Mat4 matrix;
		matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
		return matrix;

	}


	Mat4 quickInverse(){
		Mat4 matrix;
		matrix.m[0][0] = m[0][0]; matrix.m[0][1] = m[1][0]; matrix.m[0][2] = m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = m[0][1]; matrix.m[1][1] = m[1][1]; matrix.m[1][2] = m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = m[0][2]; matrix.m[2][1] = m[1][2]; matrix.m[2][2] = m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

};