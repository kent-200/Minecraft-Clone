#pragma once
#include "vec3d.h"
#include "mat4.h"
#include "triangle.h"
#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <math.h>
#include <algorithm>



//static class for now
class Process3D {
    static Vec3d Vector_IntersectPlane(Vec3d &plane_p, Vec3d &plane_n, Vec3d &lineStart, Vec3d &lineEnd);
    static int Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);

	//projection matrix
    Mat4 matProj;
    int windowWidth;
    int windowHeight;

	bool projectionInit(int windowWidth, int windowHeight);
public:
	bool Render3D(std::vector<Triangle> tris3d, Mat4 cameraViewMat, Vec3d cameraPos, std::vector<std::array<float, 3>> & coloursToDraw, std::vector<float> & drawPoints2D);

	Process3D(){}

	void init(int windowWidth, int windowHeight){
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;
		if (!projectionInit(windowWidth, windowHeight)){
			std::cout << "Failed to initialise projection matrix" << std::endl;
			throw;
		}
	}

};



Vec3d Process3D::Vector_IntersectPlane(Vec3d &plane_p, Vec3d &plane_n, Vec3d &lineStart, Vec3d &lineEnd){
    plane_n = plane_n.normalise();
    float plane_d = -plane_n.dot_product(plane_p);
    float ad = lineStart.dot_product(plane_n);
    float bd = lineEnd.dot_product(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vec3d lineStartToEnd = lineEnd - lineStart;
    Vec3d lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

int Process3D::Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2){
    // Make sure plane normal is indeed normal
    plane_n = plane_n.normalise();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](Vec3d &p)
    {
        //Vec3d n = Vector_Normalise(p);
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot_product(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vec3d* inside_points[3];  int nInsidePointCount = 0;
    Vec3d* outside_points[3]; int nOutsidePointCount = 0;

    // Get signed distance of each point in Triangle to plane
    float d0 = dist(in_tri.point[0]);
    float d1 = dist(in_tri.point[1]);
    float d2 = dist(in_tri.point[2]);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.point[0]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.point[0]; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.point[1]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.point[1]; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.point[2]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.point[2]; }

    // Now classify Triangle points, and break the input Triangle into 
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole Triangle
        // It ceases to exist

        return 0; // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the Triangle to simply pass through
        out_tri1 = in_tri;

        return 1; // Just the one returned original Triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the Triangle simply becomes a smaller Triangle

        // Copy appearance info to new Triangle
        out_tri1.col =  in_tri.col;

        // The inside point is valid, so keep that...
        out_tri1.point[0] = *inside_points[0];

        // but the two new points are at the locations where the 
        // original sides of the Triangle (lines) intersect with the plane
        out_tri1.point[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.point[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single Triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped Triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.col =  in_tri.col;

        out_tri2.col =  in_tri.col;

        // The first Triangle consists of the two inside points and a new
        // point determined by the location where one side of the Triangle
        // intersects with the plane
        out_tri1.point[0] = *inside_points[0];
        out_tri1.point[1] = *inside_points[1];
        out_tri1.point[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        // The second Triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // Triangle and the plane, and the newly created point above
        out_tri2.point[0] = *inside_points[1];
        out_tri2.point[1] = out_tri1.point[2];
        out_tri2.point[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        return 2; // Return two newly formed triangles which form a quad
    }
    return 0;
}

bool Process3D::projectionInit(int windowWidth, int windowHeight){
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    // Projection Matrix
    matProj = Mat4::makeProjection(90.0f, (float)windowHeight / (float)windowWidth, 0.1f, 1000.0f);
    return true;
}

bool Process3D::Render3D(std::vector<Triangle> tris3d, Mat4 cameraViewMat, Vec3d cameraPos, std::vector<std::array<float, 3>> &coloursToDraw, std::vector<float> &drawPoints){
    //camera.printCord();
    Mat4 matWorld = Mat4::makeIdentity();	// Form World Matrix

    // Get view matrix from camera class
    Mat4 matView = cameraViewMat;

    // Store triagles for rastering later
    std::vector<Triangle> vecTrianglesToClip;
    

    // Draw Triangles
    for (auto tri : tris3d){
        Triangle triProjected, triTransformed, triViewed;

        // World Matrix Transform
        for(int i = 0; i < 3; i++){
            triTransformed.point[i] = matWorld * tri.point[i];
        }

        // Calculate Triangle Normal
        Vec3d normal, line1, line2;

        // Get lines either side of Triangle
        line1 = triTransformed.point[1] - triTransformed.point[0];
        line2 = triTransformed.point[2] - triTransformed.point[0];

        // Take cross product of lines to get normal to Triangle surface
        normal = line1.cross_product(line2);

        // You normally need to normalise a normal!
        normal = normal.normalise();
        
        // Get Ray from Triangle to camera
        Vec3d vCameraRay = triTransformed.point[0] - cameraPos;


        // If ray is aligned with normal, then Triangle is visible
        if (normal.dot_product(vCameraRay) < 0.0f){
            // Illumination
            Vec3d light_direction = { 0.0f, 1.0f, -0.5f };
            light_direction = light_direction.normalise();

            // How "aligned" are light direction and Triangle surface normal?
            // float dp = std::max(0.2f, (float)(light_direction.dot_product(normal) * 1));
            // dp = std::min(dp, 0.85f);

            // triTransformed.col = dp;
            triTransformed.col = tri.col;   //get colour from face data

            // Convert World Space --> View Space
            for(int i = 0; i < 3; i++){
                triViewed.point[i] = matView * triTransformed.point[i];
            }
            triViewed.col = triTransformed.col;

            // Clip Viewed Triangle against near plane, this could form two additional
            // additional triangles. 
            int nClippedTriangles = 0;
            Triangle clipped[2];
            nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

            // We may end up with multiple triangles form the clip, so project as
            // required
            for (int n = 0; n < nClippedTriangles; n++){
                Vec3d vOffsetView = { 1,1,0 };

                //for each point in the triangle
                for (int i = 0; i < 3; i++){
                    // Project triangles from 3D --> 2D
                    triProjected.point[i] = matProj * clipped[n].point[i];

                    // Scale into view, we moved the normalising into cartesian space
                    // out of the matrix.vector function from the previous videos, so
                    // do this manually
                    triProjected.point[i] = triProjected.point[i] / triProjected.point[i].w;

                    // X/Y are inverted so put them back
                    triProjected.point[i].x *= -1.0f;
                    triProjected.point[i].y *= 1.0f;

                    // Offset verts into visible normalised space
                    triProjected.point[i] = triProjected.point[i] + vOffsetView;

                    // Scale into view
                    triProjected.point[i].x *= 0.5f * (float)windowWidth;
                    triProjected.point[i].y *= 0.5f * (float)windowHeight;
                }
                triProjected.col = clipped[n].col;

                // Store Triangle for sorting
                vecTrianglesToClip.push_back(triProjected);
            }			
        }
    }

    // Sort triangles from back to front
    std::sort(vecTrianglesToClip.begin(), vecTrianglesToClip.end(), [](Triangle &t1, Triangle &t2){
        float z1 = (t1.point[0].z + t1.point[1].z + t1.point[2].z) / 3.0f;
        float z2 = (t2.point[0].z + t2.point[1].z + t2.point[2].z) / 3.0f;
        return z1 > z2;
    });


    // Loop through all transformed, viewed, projected, and sorted triangles
    // Clip triangles against all four screen edges and normalise to OpenGL screen coordinates
    for (auto &triToRaster : vecTrianglesToClip){
        // Clip triangles against all four screen edges, this could yield
        // a bunch of triangles, so create a queue that we traverse to 
        //  ensure we only test new triangles generated against planes
        Triangle clipped[2];
        std::list<Triangle> listTriangles;

        // Add initial Triangle
        listTriangles.push_back(triToRaster);
        int nNewTriangles = 1;

        for (int p = 0; p < 4; p++){
            int nTrisToAdd = 0;
            while (nNewTriangles > 0){
                // Take Triangle from front of queue
                Triangle test = listTriangles.front();
                listTriangles.pop_front();
                nNewTriangles--;

                // Clip it against a plane. We only need to test each 
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified
                switch (p)
                {
                case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)windowHeight - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)windowWidth - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                }

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int w = 0; w < nTrisToAdd; w++)
                    listTriangles.push_back(clipped[w]);
            }
            nNewTriangles = listTriangles.size();
        }


        // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
        for (auto &t : listTriangles){
            // normalise to screen and push to draw list
            for(int i = 0; i < 3; i++){
                t.point[i].x = (t.point[i].x / (windowWidth / 2)) - 1;
                t.point[i].y = (t.point[i].y / (windowHeight / 2)) - 1;					
            }


            //trianglesToDraw.push_back(point);
            for(int i = 0; i < 3; i++){
                drawPoints.push_back(t.point[i].x);
                drawPoints.push_back(t.point[i].y);
                drawPoints.push_back(0.0f);
            }

            std::array<float, 3> color{t.col.x, t.col.y, t.col.z};
            coloursToDraw.push_back(color);				
        }
    }

    return true;
}


