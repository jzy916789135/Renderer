#pragma once

#include "Utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class IntersectType
{
	DISJOINT = 0,
	INTERSECT = 1,
	CONTAIN = 2
};

struct BoundingFrustum
{
	glm::vec4 paramLeft;
	glm::vec4 paramRight;
	glm::vec4 paramTop;
	glm::vec4 paramBottom;
	glm::vec4 paramNear;
	glm::vec4 paramFar;


	void ExtractFrustumFromMatrix(glm::mat4 matrix);

	IntersectType ContainAxisAlignedBox(AxisAlignedBoundingBox localAABB);

	
	IntersectType FastIntersectAxisAlignedBoxPlane(glm::vec4 plane, float radius);

};