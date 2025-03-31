//
// Created by moham on 3/15/2025.
//

#include "ShapeGenerator.hpp"

#include <glm/ext/matrix_transform.hpp>

//======================================================================================================================

CPU_Geometry ShapeGenerator::Sphere(float const radius, int const slices, int const stacks)
{
	CPU_Geometry geom{};

	// generate a single curve to be revolved
	std::vector<glm::vec3> singleCurve{};
	for (float u = 0; u < glm::pi<float>(); u += glm::pi<float>() / stacks)
	{
		singleCurve.emplace_back(radius * glm::sin(u), radius * glm::cos(u), 0.0f);
	}

	// guarantee the section at the bottom
	singleCurve.emplace_back(0.0f, -radius, 0.0f);

	std::vector <std::vector<glm::vec3>> positions{};
	
	// generate a surface of revolution
	for (float u = 0; u < glm::two_pi<float>(); u += glm::two_pi<float>() / slices)
	{
		std::vector<glm::vec3> curve{};

		for (glm::vec3 point : singleCurve)
		{
			curve.emplace_back(point.x * glm::cos(u), point.y, point.x * glm::sin(u));
		}
		
		positions.emplace_back(curve);
	}

	// guarantee the section at 2 pi
	std::vector<glm::vec3> curve{};
	for (glm::vec3 point : singleCurve)
	{
		curve.emplace_back(point.x * glm::cos(glm::two_pi<float>()), point.y, point.x * glm::sin(glm::two_pi<float>()));
	}
	positions.emplace_back(curve);

	// triangulate each section and add it to the geometry
	for (size_t i = 0; i < positions.size() - 1; i++) {
		for (size_t j = 0; j < positions[i].size() - 1; j++) {
			glm::vec3 pOne = positions[i][j];
			glm::vec3 pTwo = positions[i + 1][j];
			glm::vec3 pThree = positions[i][j + 1];
			glm::vec3 pFour = positions[i + 1][j + 1];

			geom.positions.push_back(pOne);
			geom.positions.push_back(pTwo);
			geom.positions.push_back(pThree);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);

			// calculate normals
			glm::vec3 normalOne = glm::normalize(glm::cross(pTwo - pOne, pThree - pOne));
			glm::vec3 normalTwo = glm::normalize(glm::cross(pThree - pTwo, pFour - pTwo));

			geom.normals.push_back(normalOne);
			geom.normals.push_back(normalOne);
			geom.normals.push_back(normalOne);

			geom.positions.push_back(pTwo);
			geom.positions.push_back(pThree);
			geom.positions.push_back(pFour);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);

			geom.normals.push_back(normalTwo);
			geom.normals.push_back(normalTwo);
			geom.normals.push_back(normalTwo);
		}
	}

	return geom;
}

//======================================================================================================================

static void colouredTriangles(CPU_Geometry& geom, glm::vec3 col);
static void positiveZFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom);
static void positiveXFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom);
static void negativeZFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom);
static void negativeXFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom);
static void positiveYFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom);
static void negativeYFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom);

CPU_Geometry ShapeGenerator::UnitCube()
{
	std::vector<glm::vec3> originQuad{};
	originQuad.emplace_back(-0.5, 0.5, 0.0); // top-left
	originQuad.emplace_back(-0.5, -0.5, 0.0); // bottom-left
	originQuad.emplace_back(0.5, 0.5, 0.0); // top-right

	originQuad.emplace_back(-0.5, -0.5, 0.0); // bottom-left
	originQuad.emplace_back(0.5, -0.5, 0.0); // bottom-right
	originQuad.emplace_back(0.5, 0.5, 0.0); // top-right

	CPU_Geometry square{};

	positiveZFace(originQuad, square);
	colouredTriangles(square, { 1.f, 1.f, 0.f });

	positiveXFace(originQuad, square);
	colouredTriangles(square, { 1.f, 0.f, 0.f });

	negativeZFace(originQuad, square);
	colouredTriangles(square, { 0.f, 1.f, 0.f });

	negativeXFace(originQuad, square);
	colouredTriangles(square, { 0.f, 0.f, 1.f });

	positiveYFace(originQuad, square);
	colouredTriangles(square, { 1.f, 0.f, 1.f });

	negativeYFace(originQuad, square);
	colouredTriangles(square, { 0.f, 1.f, 1.f });

	return square;
}

void colouredTriangles(CPU_Geometry& geom, glm::vec3 col)
{
	geom.colors.emplace_back(col);
	geom.colors.emplace_back(col);
	geom.colors.emplace_back(col);
	geom.colors.emplace_back(col);
	geom.colors.emplace_back(col);
	geom.colors.emplace_back(col);
}

void positiveZFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom)
{
	const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.5));
	for (auto i = originQuad.begin(); i < originQuad.end(); ++i)
	{
		geom.positions.emplace_back(T * glm::vec4((*i), 1.0));
	}
	geom.normals.emplace_back(0.0, 0.0, 1.0);
	geom.normals.emplace_back(0.0, 0.0, 1.0);
	geom.normals.emplace_back(0.0, 0.0, 1.0);
	geom.normals.emplace_back(0.0, 0.0, 1.0);
	geom.normals.emplace_back(0.0, 0.0, 1.0);
	geom.normals.emplace_back(0.0, 0.0, 1.0);
}

void positiveXFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom)
{
	const glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f));
	for (auto i = originQuad.begin(); i < originQuad.end(); ++i)
	{
		geom.positions.emplace_back(glm::vec3(T * R * glm::vec4((*i), 1.0)));
	}
	geom.normals.emplace_back(glm::vec3(1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(1.0, 0.0, 0.0));
}

void negativeZFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom)
{
	const glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f));
	for (auto i = originQuad.begin(); i < originQuad.end(); ++i)
	{
		geom.positions.emplace_back(glm::vec3(T * R * glm::vec4((*i), 1.0)));
	}
	geom.normals.emplace_back(glm::vec3(0.0, 0.0, -1.0));
	geom.normals.emplace_back(glm::vec3(0.0, 0.0, -1.0));
	geom.normals.emplace_back(glm::vec3(0.0, 0.0, -1.0));
	geom.normals.emplace_back(glm::vec3(0.0, 0.0, -1.0));
	geom.normals.emplace_back(glm::vec3(0.0, 0.0, -1.0));
	geom.normals.emplace_back(glm::vec3(0.0, 0.0, -1.0));
}

void negativeXFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom)
{
	const glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
	for (auto i = originQuad.begin(); i < originQuad.end(); ++i)
	{
		geom.positions.emplace_back(glm::vec3(T * R * glm::vec4((*i), 1.0)));
	}
	geom.normals.emplace_back(glm::vec3(-1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(-1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(-1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(-1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(-1.0, 0.0, 0.0));
	geom.normals.emplace_back(glm::vec3(-1.0, 0.0, 0.0));
}

void positiveYFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom)
{
	const glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
	for (auto i = originQuad.begin(); i < originQuad.end(); ++i)
	{
		geom.positions.emplace_back(glm::vec3(T * R * glm::vec4((*i), 1.0)));
	}
	geom.normals.emplace_back(glm::vec3(0.0, 1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, 1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, 1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, 1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, 1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, 1.0, 0.0));
}

void negativeYFace(std::vector<glm::vec3> const& originQuad, CPU_Geometry& geom)
{
	const glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
	for (auto i = originQuad.begin(); i < originQuad.end(); ++i)
	{
		geom.positions.emplace_back(glm::vec3(T * R * glm::vec4((*i), 1.0)));
	}
	geom.normals.emplace_back(glm::vec3(0.0, -1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, -1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, -1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, -1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, -1.0, 0.0));
	geom.normals.emplace_back(glm::vec3(0.0, -1.0, 0.0));
}

//======================================================================================================================
