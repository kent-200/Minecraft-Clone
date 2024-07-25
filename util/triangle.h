#pragma once
#include "vec3d.h"

class Triangle {
public:
	Vec3d point[3];
	Vec3d col;

    Triangle() = default;

    Triangle(Vec3d p0, Vec3d p1, Vec3d p2) {
        point[0] = p0;
        point[1] = p1;
        point[2] = p2;
    }


    Triangle(Vec3d p0, Vec3d p1, Vec3d p2, Vec3d col) {
        point[0] = p0;
        point[1] = p1;
        point[2] = p2;
        this->col = col;
    }
};