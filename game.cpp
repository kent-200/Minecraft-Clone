#include "game.h"

// g++ -o run game.cpp text.cpp -lglfw3 -lkernel32 -lopengl32 -lglu32 -lglew32 


class Mat4;

class Vec3d{
	public:
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1; // Need a 4th term to perform sensible matrix vector multiplication

	Vec3d() = default;
	Vec3d(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3d(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}


	//addition and subtraction of vectors
	Vec3d operator+(Vec3d v){
		return Vec3d(x + v.x, y + v.y, z + v.z);
	}
	Vec3d operator-(Vec3d v) {
		return Vec3d(x - v.x, y - v.y, z - v.z);
	}

	//multiplication of vector by scalar
	Vec3d operator*(float k) {
		return Vec3d(x * k, y * k, z * k);
	}

	//division of vector by scalar
	Vec3d operator/(float k) {
		return Vec3d(x / k, y / k, z / k);
	}

	//dot product
	float dot_product(Vec3d v) {
		return x * v.x + y * v.y + z * v.z;
	}

	//length
	float vec_length(Vec3d v) {
		return sqrt(dot_product(v));
	}

	//normalise
	Vec3d normalise() {
		float l = vec_length(*this);
		return *this / l;
	}

	//cross product
	Vec3d cross_product(const Vec3d& v) const {
		return Vec3d(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

};

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


class Camera {
public:
	Vec3d pos;	// Location of camera in world space
	Vec3d lookDir = Vec3d(0, 0, 1);	// Direction vector along the direction camera points
	float fYaw;		// FPS Camera rotation in XZ plane
	float fPitch;	// FPS Camera rotation in YZ plane
	float mouseSensitivity = 5.0f;
	float keySensitive = 8.0f;

	Camera() = default;
	Camera(Vec3d pos) : pos(pos) {
		//look at z by default
		lookDir = Vec3d(0, 0, 1);
	}


	Mat4 matView() {
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

    void processKeyboardInput(GLFWwindow * window, float fElapsedTime){
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
            glfwSetWindowShouldClose(window, true);
        }
    }


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


	void printCord(){
		std::cout << "x: " << pos.x << " y: " << pos.y << " z: " << pos.z << std::endl;
	}

};



//face of a cube
class CubeFace {
private:
    std::string name;
    //triangles are offsets for the 2 triangles
    Triangle triangles[2];
public:
    CubeFace(std::string name, Triangle t1, Triangle t2) : name(name) {
        triangles[0] = t1;
        triangles[1] = t2;
    }

    void getTriangles(std::vector<Triangle> & tris, Vec3d relativePos, Vec3d color) {
        tris.push_back(Triangle(triangles[1].point[0] + relativePos, triangles[1].point[1] + relativePos, triangles[1].point[2] + relativePos, color));
        tris.push_back(Triangle(triangles[0].point[0] + relativePos, triangles[0].point[1] + relativePos, triangles[0].point[2] + relativePos, color));
    }

};


class CubeType {
    public:
    Vec3d color[6];

    CubeType(Vec3d cfront, Vec3d cback, Vec3d cleft, Vec3d cright, Vec3d ctop, Vec3d cbottom){
        color[0] = cfront;
        color[1] = cback;
        color[2] = cleft;
        color[3] = cright;
        color[4] = ctop;
        color[5] = cbottom;
    }
};


class Cube {
public:
    Vec3d position;
    int type_id;

    Cube(Vec3d position, int type_id) : position(position), type_id(type_id) {}
};

//cubes
//cubeface
//cubetype
//chunk
//world
//player
//camera

class Mesh {
private:
    //faces for drawing cubes
    std::vector<CubeFace> faces;
    std::vector<CubeType> cubeTypes;


    //block size
    void addFaces(float bs){
        // using clockwise ordering for vertices, later normalisation
        faces.push_back(CubeFace("front", Triangle(Vec3d(bs, 0, bs), Vec3d(0, bs, bs), Vec3d(0, 0, bs)), Triangle(Vec3d(bs, 0, bs), Vec3d(bs, bs, bs), Vec3d(0, bs, bs))));
        faces.push_back(CubeFace("back", Triangle(Vec3d(0, 0, 0), Vec3d(0, bs, 0), Vec3d(bs, 0, 0)), Triangle(Vec3d(0, bs, 0), Vec3d(bs, bs, 0), Vec3d(bs, 0, 0))));
        faces.push_back(CubeFace("left", Triangle(Vec3d(0, 0, 0), Vec3d(0, 0, bs), Vec3d(0, bs, bs)), Triangle(Vec3d(0, 0, 0), Vec3d(0, bs, bs), Vec3d(0, bs, 0))));
        faces.push_back(CubeFace("right", Triangle(Vec3d(bs, 0, 0), Vec3d(bs, bs, 0), Vec3d(bs, 0, bs)), Triangle(Vec3d(bs, 0, bs), Vec3d(bs, bs, 0), Vec3d(bs, bs, bs))));
        faces.push_back(CubeFace("top", Triangle(Vec3d(bs, bs, 0), Vec3d(0, bs, 0), Vec3d(0, bs, bs)), Triangle(Vec3d(bs, bs, 0), Vec3d(0, bs, bs), Vec3d(bs, bs, bs))));
        faces.push_back(CubeFace("bottom", Triangle(Vec3d(bs, 0, bs), Vec3d(0, 0, bs), Vec3d(0, 0, 0)), Triangle(Vec3d(bs, 0, bs), Vec3d(0, 0, 0), Vec3d(bs, 0, 0))));
    }


    void addTypes(){
        cubeTypes.push_back(CubeType(Vec3d(1, 0, 0, 1), Vec3d(0, 1, 0, 1), Vec3d(0, 0, 1, 1), Vec3d(1, 1, 0, 1), Vec3d(0, 1, 1, 1), Vec3d(1, 0, 1, 1)));
		//grey cube, top light, side middle, bottom dark
		cubeTypes.push_back(CubeType(Vec3d(0.7, 0.7, 0.7, 1), Vec3d(0.7, 0.7, 0.7, 1), Vec3d(0.6, 0.6, 0.6, 1), Vec3d(0.6, 0.6, 0.6, 1), Vec3d(0.8, 0.8, 0.8, 1), Vec3d(0.2, 0.2, 0.2, 1)));

    }

public:
    std::vector<Cube> cubes;
    std::vector<Triangle> tris;
    Mesh(){
        addFaces(1.0f);
        addTypes();

        // add cube
        cubes.push_back(Cube(Vec3d(0, 0, -3), 0));
		cubes.push_back(Cube(Vec3d(2, 2, 0), 1));
		cubes.push_back(Cube(Vec3d(0, 0, -1), 1));

		cubes.push_back(Cube(Vec3d(2, 2, 5), 1));
		cubes.push_back(Cube(Vec3d(0, 0, 5), 1));

		cubes.push_back(Cube(Vec3d(-2, -2, 0), 1));
		cubes.push_back(Cube(Vec3d(5, 8, -1), 1));


    }

    //triangles and face colours
    void getCubeData(std::vector<Triangle> & tris, int face, Vec3d pos, int type_id){
        //iterate through faces - adds 2 triangles
        faces[face].getTriangles(tris, pos, cubeTypes[type_id].color[face]);
    }



};


//static class for now
class Static3DRenderer {
public:
    static Vec3d Vector_IntersectPlane(Vec3d &plane_p, Vec3d &plane_n, Vec3d &lineStart, Vec3d &lineEnd){
		plane_n = plane_n.normalise();
		float plane_d = -plane_n.dot_product(plane_p);
		float ad = lineStart.dot_product(plane_n);
		float bd = lineEnd.dot_product(plane_n);
		float t = (-plane_d - ad) / (bd - ad);
		Vec3d lineStartToEnd = lineEnd - lineStart;
		Vec3d lineToIntersect = lineStartToEnd * t;
		return lineStart + lineToIntersect;
	}

    static int Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2){
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

};


class Renderer {
    //shader variables for triangles
    GLuint triShaderProgram;
    GLuint triVAO, triVBO, triEBO;
    GLuint triTexture;   

    //text
    const char* atlas_image = "DS-DIGI-atlas.png";
    const char* atlas_meta = "DS-DIGI-atlas.meta";

    //mesh
    Mesh mesh;

    //projection matrix
    Mat4 matProj;
    int windowWidth;
    int windowHeight;



    //projection matrix
    //rotation matrix

    const char* vertex_shader = R"(
        #version 330 core
        layout (location = 0) in vec3 vp;
        void main() {
            gl_Position = vec4(vp, 1.0);
        }
    )";
    
    const char* fragment_shader = R"(
        #version 330 core
        out vec4 frag_color;
        uniform vec4 u_color;
        void main() {
            frag_color = u_color;
        }
    )";

   


    void renderIndivTri(GLFWwindow * window, GLint & color_loc, GLuint & vao, GLuint & shader_programme, int pointStart, int pointNum, std::array<float, 3> color){
        // Draw triangle (color)
        glUniform4fv(color_loc, 1, &color[0]);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, pointStart, pointNum);
    }


  
    
    void renderAll(GLFWwindow * window, std::vector<float> & drawPoints, std::vector<std::array<float, 3>> & colours){        
        // Enable lighting
        // glEnable(GL_LIGHTING);
        // glEnable(GL_LIGHT0);

        // // Set material properties
        // GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
        // GLfloat diffuseColor[] = {0.8f, 0.8f, 0.8f, 1.0f};
        // GLfloat specularColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        // GLfloat shininess = 32.0f;

        // glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
        // glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
        // glMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);
        // glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        // // Set light position
        // GLfloat lightPosition[] = {1.0f, 1.0f, 1.0f, 0.0f}; // Directional light from top-right
        // glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        //print colours
        // std::cout << "start2\n";
        // for(int i = 0; i < colours.size(); i++){
        //     std::cout << "x: " << colours[i].x << " y: " << colours[i].y << " z: " << colours[i].z << std::endl;
        // }
        // std::cout << "end2\n";

        //put points into single array
        // std::vector<float> drawPoints;
        // for(int i = 0; i < (int) points.size(); i++){
        //     for(int j = 0; j < 9; j++){
        //         drawPoints.push_back(points[i][j]);
        //     }
        // }


        // Bind VBO and update vertex data
        // load all triangle points into VBO
        glBindBuffer(GL_ARRAY_BUFFER, triVBO);
        if (!drawPoints.empty()) {
            glBufferData(GL_ARRAY_BUFFER, drawPoints.size() * sizeof(float), drawPoints.data(), GL_DYNAMIC_DRAW);
        }
            
        // draw geometry
        glUseProgram(triShaderProgram);

        // only needs to be checked once
        GLint color_loc = glGetUniformLocation(triShaderProgram, "u_color");
        if (color_loc == -1) {
            std::cerr << "Error: Uniform 'u_color' not found in shader program" << std::endl;
            exit(0);
            return;
        }

        // draw triangles from array

        for(int i = 0; i < (int) colours.size(); i++){
			colours[i][0] -= 0.02f * i;
            renderIndivTri(window, color_loc, triVAO, triShaderProgram, i * 3, 3, colours[i]);
        }

        glBindVertexArray(0);

        // Disable lighting after rendering
        // glDisable(GL_LIGHTING);
    }

    
    


public:
    Renderer(){
        
    }

	const char * getAtlasImage(){
		return atlas_image;
	}

	const char * getAtlasMeta(){
		return atlas_meta;
	}


    bool projectionInit(int windowWidth, int windowHeight){
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        // Projection Matrix
		matProj = Mat4::makeProjection(90.0f, (float)windowHeight / (float)windowWidth, 0.1f, 1000.0f);
		return true;
    }


    void objectShaderInit() {
        // add some geometry
        float points[] = {
            -0.5f, -0.5f,  0.0f,
                0.5f, -0.5f,  0.0f,
                0.0f,  0.5f,  0.0f
        };

        
        // Create vertex array object (VAO)
        glGenVertexArrays(1, &triVAO);
        glBindVertexArray(triVAO);

        // Create vertex buffer object (VBO) for dynamic points
        glGenBuffers(1, &triVBO);
        glBindBuffer(GL_ARRAY_BUFFER, triVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);

        // Define vertex attribute pointer
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


        // shader for the geometry
        {
            
            GLuint vs = glCreateShader (GL_VERTEX_SHADER);
            glShaderSource (vs, 1, &vertex_shader, NULL);
            glCompileShader (vs);

            GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
            glShaderSource (fs, 1, &fragment_shader, NULL);
            glCompileShader (fs);

            triShaderProgram = glCreateProgram ();
            glAttachShader (triShaderProgram, fs);
            glAttachShader (triShaderProgram, vs);
            glLinkProgram (triShaderProgram);


            glDeleteShader(vs);
            glDeleteShader(fs);
        }

    }


    void render_process(GLFWwindow * window, Camera camera, float fElapsedTime){
        // Fill the triangles to draw
        for (auto cube : mesh.cubes){
            for (int i = 0; i < 6; i++){
                mesh.getCubeData(mesh.tris, i, cube.position, cube.type_id);
            }
        }

		std::vector<std::array<float, 3>> coloursToDraw;
		std::vector<float> drawPoints;

        // Render triangles as 3D
        Render3D(window, camera, coloursToDraw, drawPoints);

		// Render triangles as 2D
		renderAll(window, drawPoints, coloursToDraw);

		drawPoints.clear();
		coloursToDraw.clear();
		mesh.tris.clear();

    }


    bool Render3D(GLFWwindow * window, Camera camera, std::vector<std::array<float, 3>> & coloursToDraw, std::vector<float> & drawPoints){
		//camera.printCord();
		Mat4 matWorld = Mat4::makeIdentity();	// Form World Matrix

		// Get view matrix from camera class
		Mat4 matView = camera.matView();

		// Store triagles for rastering later
		std::vector<Triangle> vecTrianglesToClip;
		

		// Draw Triangles
		for (auto tri : mesh.tris){
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
			Vec3d vCameraRay = triTransformed.point[0] - camera.pos;


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
				nClippedTriangles = Static3DRenderer::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

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
					case 0:	nTrisToAdd = Static3DRenderer::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Static3DRenderer::Triangle_ClipAgainstPlane({ 0.0f, (float)windowHeight - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Static3DRenderer::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Static3DRenderer::Triangle_ClipAgainstPlane({ (float)windowWidth - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
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



    ~Renderer() {
        //clean up
        glDeleteVertexArrays(1, &triVAO);
        glDeleteBuffers(1, &triVBO);
        glDeleteBuffers(1, &triEBO);
        glDeleteProgram(triShaderProgram);
    }



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
		glfwSetErrorCallback(errorCallback);
        
        // Create a GLFW window
        window = glfwCreateWindow(winWidth, winHeight, "OpenGL Triangle", NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Create user resources as part of this thread
		if (!renderer.projectionInit(winWidth, winHeight)) {
			std::cerr << "Failed on user create" << std::endl;
			exit(-1);
		}


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



        // initialize shader for triangles
        renderer.objectShaderInit();

        // init_text_rendering (renderer.getAtlasImage(), renderer.getAtlasMeta(), winWidth, winHeight);
        // int hello_id = add_text (
        //     "Hello world!",
        //     -0.9f, 0.5f, 35.0f, 0.5f, 0.5f, 1.0f, 1.0f);
        
        // //position of text on screen
        // move_text (hello_id, -1.0f, 0.0f);


    }


    void run(){
		//sensitivity
		camera.keySensitive = 5.0f;
		camera.mouseSensitivity = 2.0f;

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


        while (!glfwWindowShouldClose(window)) {
            // Handle Timing
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTime = tp2 - tp1;
            tp1 = tp2;
            float fElapsedTime = elapsedTime.count();

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
            camera.processKeyboardInput(window, fElapsedTime);

			//camera.printCord();


            // Handle Frame Update
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Render the scene
            renderer.render_process(window, camera, fElapsedTime);
            //draw_texts();
            

            // Swap buffers
			glfwSwapBuffers(window);
			// Poll for and process events
			glfwPollEvents();


            // Update Title Bar
            fps_update++;
            if (fps_update > 25){
                double average_fps = 25.0f / fps_elapsed_time;
                // Update window title with FPS
                std::string windowTitle = "GLFW game engine - FPS: " + std::to_string(average_fps);
                glfwSetWindowTitle(window, windowTitle.c_str());
                fps_update = 0;
                fps_elapsed_time = 0.0;
            } else {
                fps_elapsed_time += fElapsedTime;
            }
            
        }
        return;
    }

    ~Game() {
        // Terminate GLFW
        glfwTerminate();
    }


};




int main(void){
    Game game(1200, 800);
    game.run();
    return 0;
}