#pragma once

#include "Geometry.h"

namespace ShapeGenerator
{
	// TODO
	 [[nodiscard]]
	CPU_Geometry Sphere(float radius, int slices, int stacks); // creates a sphere cpu geometry 

	// does the same as Sphere() but in opposite winding order so the textures can properly load on the inside of the sphere
	CPU_Geometry BackgroundSphere(float radius, int slices, int stacks); 

	CPU_Geometry Ring(float radius, float width, int resolution); // creates a ring cpu geometry 

	CPU_Geometry UnitCube();

	std::vector<std::vector<glm::vec3>> GenerateSphere(float radius, int slices, int stacks); // creates a sphere with its positions
};