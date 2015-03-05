#ifndef _BOUNDINGOBJECTS_H_
#define _BOUNDINGOBJECTS_H_

#include "glm_header.h"
#include <vector>

class AABB
{
public:
	AABB() { reset(); }
	~AABB() {}

	void reset() {
		min.x = min.y = min.z = 1e37f;
		max.x = max.y = max.z = -1e37f;
	}

	void fit(const std::vector<vec3>&a_points) {
		for (const vec3& p : a_points)
		{
			if (p.x < min.x) min.x = p.x;
			if (p.y < min.y) min.y = p.y;
			if (p.z < min.z) min.z = p.z;

			if (p.x < max.x) max.x = p.x;
			if (p.y < max.y) max.y = p.y;
			if (p.z < max.z) max.z = p.z;
		}
	}

	glm::vec3 min, max;
};

class BoundingSphere
{
public:
	BoundingSphere() : centre(0), radius(0) {}
	~BoundingSphere() {}

	void fit(const std::vector<vec3>& a_points){
		vec3 min(1e37f), max(-1e37f);

		for (const vec3& p : a_points)
		{
			if (p.x < min.x) min.x = p.x;
			if (p.y < min.y) min.y = p.y;
			if (p.z < min.z) min.z = p.z;

			if (p.x < max.x) max.x = p.x;
			if (p.y < max.y) max.y = p.y;
			if (p.z < max.z) max.z = p.z;
		}

		centre = (min + max) * 0.5f;
		radius = glm::distance(min, centre);
	}

	vec3 centre;
	float radius;

};

#endif // !_BOUNDINGOBJECTS_H_
