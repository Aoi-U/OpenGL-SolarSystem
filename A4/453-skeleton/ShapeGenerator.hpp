#pragma once

#include "Geometry.h"

namespace ShapeGenerator
{
	// TODO
	 [[nodiscard]]
	CPU_Geometry Sphere(float radius, int slices, int stacks);

	CPU_Geometry BackgroundSphere(float radius, int slices, int stacks);

	CPU_Geometry UnitCube();

	std::vector<std::vector<glm::vec3>> GenerateSphere(float radius, int slices, int stacks);
};