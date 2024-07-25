#pragma once
#include <vector>
#include <cmath>



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