#pragma once
#include "vec3d.h"
#include "mat4.h"
#include "triangle.h"


class Block {
public:
    Vec3d pos;
    int type_id;
    Block(Vec3d pos, int type_id) : pos(pos), type_id(type_id) {}

    // front, back, left, right, top, bottom
    static std::array<Triangle, 2> faceCordinates(int face){
        switch(face){
            //front
            case 0:
                return {
                    Triangle(Vec3d(1, 0, 1), Vec3d(0, 1, 1), Vec3d(0, 0, 1)), 
                    Triangle(Vec3d(1, 0, 1), Vec3d(1, 1, 1), Vec3d(0, 1, 1))
                };

            //back
            case 1:
                return {
                    Triangle(Vec3d(0, 0, 0), Vec3d(0, 1, 0), Vec3d(1, 0, 0)), 
                    Triangle(Vec3d(0, 1, 0), Vec3d(1, 1, 0), Vec3d(1, 0, 0))
                };

            //left
            case 2:
                return {
                    Triangle(Vec3d(0, 0, 0), Vec3d(0, 0, 1), Vec3d(0, 1, 1)), 
                    Triangle(Vec3d(0, 0, 0), Vec3d(0, 1, 1), Vec3d(0, 1, 0))
                };

            //right
            case 3:
                return {
                    Triangle(Vec3d(1, 0, 0), Vec3d(1, 1, 0), Vec3d(1, 0, 1)), 
                    Triangle(Vec3d(1, 0, 1), Vec3d(1, 1, 0), Vec3d(1, 1, 1))
                };

            //top
            case 4:
                return {
                    Triangle(Vec3d(1, 1, 0), Vec3d(0, 1, 0), Vec3d(0, 1, 1)), 
                    Triangle(Vec3d(1, 1, 0), Vec3d(0, 1, 1), Vec3d(1, 1, 1))
                };

            //bottom
            case 5:
                return {
                    Triangle(Vec3d(1, 0, 1), Vec3d(0, 0, 1), Vec3d(0, 0, 0)), 
                    Triangle(Vec3d(1, 0, 1), Vec3d(0, 0, 0), Vec3d(1, 0, 0))
                };

            default:
                throw std::invalid_argument("Invalid face");
        }
    }

    static void faceRelativeTriangles(std::vector<Triangle> & tris, int face, Vec3d relativePos, Vec3d color){
        std::array<Triangle, 2> faceTri = faceCordinates(face);
        tris.push_back(Triangle(faceTri[1].point[0] + relativePos, faceTri[1].point[1] + relativePos, faceTri[1].point[2] + relativePos, color));
        tris.push_back(Triangle(faceTri[0].point[0] + relativePos, faceTri[0].point[1] + relativePos, faceTri[0].point[2] + relativePos, color));
    }

    static void allRelativeTriangles(std::vector<Triangle> & tris, Vec3d pos, std::array<Vec3d, 6> color){
        for(int i = 0; i < 6; i++){
            faceRelativeTriangles(tris, i, pos, color[i]);
        }
    }

};
