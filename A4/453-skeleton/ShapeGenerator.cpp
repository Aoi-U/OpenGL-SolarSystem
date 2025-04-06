//
// Created by moham on 3/15/2025.
//

#include "ShapeGenerator.hpp"

#include <glm/ext/matrix_transform.hpp>

//======================================================================================================================

CPU_Geometry ShapeGenerator::Sphere(float const radius, int const slices, int const stacks)
{
	// generate a single curve to be revolved
	std::vector<std::vector<glm::vec3>> positions = GenerateSphere(radius, slices, stacks);
	
	CPU_Geometry geom{};
	// triangulate each section and add it to the geometry
	for (size_t i = 0; i < positions.size() - 1; i++) {
		for (size_t j = 0; j < positions[i].size() - 1; j++) {
			glm::vec3 pOne = positions[i][j]; // top left
			glm::vec3 pTwo = positions[i][j + 1]; // bottom left
			glm::vec3 pThree = positions[i + 1][j + 1]; // bottom right
			glm::vec3 pFour = positions[i + 1][j]; // top right

			geom.positions.push_back(pOne); // top left
			geom.positions.push_back(pThree); // bottom right
			geom.positions.push_back(pTwo); // bottom left
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);

			geom.positions.push_back(pOne); // top left
			geom.positions.push_back(pFour); // top right
			geom.positions.push_back(pThree); // bottom right
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);

			// add the normals to the geometry
			// since this is a unit sphere, the normals are simply just the positions of each vertex
			geom.normals.push_back(pOne); // top left
			geom.normals.push_back(pThree); // bottom right
			geom.normals.push_back(pTwo); // bottom left
			geom.normals.push_back(pOne); // top left
			geom.normals.push_back(pFour); // top right
			geom.normals.push_back(pThree); // bottom right

			// calculate the texture coordinates
			geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(slices), 1.0f - static_cast<float>(j) / static_cast<float>(stacks)); // top left
			geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(slices), 1.0f - static_cast<float>(j + 1) / static_cast<float>(stacks)); // bottom right
			geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(slices), 1.0f - static_cast<float>(j + 1) / static_cast<float>(stacks)); // bottom left
			geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(slices), 1.0f - static_cast<float>(j) / static_cast<float>(stacks)); // top left
			geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(slices), 1.0f - static_cast<float>(j) / static_cast<float>(stacks)); // top right
			geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(slices), 1.0f - static_cast<float>(j + 1) / static_cast<float>(stacks)); // bottom right
		}
	}

	return geom;
}

CPU_Geometry ShapeGenerator::BackgroundSphere(float const radius, int const slices, int const stacks)
{
	// generate a single curve to be revolved
	std::vector<std::vector<glm::vec3>> positions = GenerateSphere(radius, slices, stacks);

	CPU_Geometry geom{};
	// triangulate each section and add it to the geometry
	for (size_t i = 0; i < positions.size() - 1; i++) {
		for (size_t j = 0; j < positions[i].size() - 1; j++) {
			glm::vec3 pOne = positions[i][j]; // top left
			glm::vec3 pTwo = positions[i][j + 1]; // bottom left
			glm::vec3 pThree = positions[i + 1][j + 1]; // bottom right
			glm::vec3 pFour = positions[i + 1][j]; // top right

			geom.positions.push_back(pThree); // bottom right
			geom.positions.push_back(pOne); // top left
			geom.positions.push_back(pTwo); // bottom left
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);

			geom.positions.push_back(pFour); // top right
			geom.positions.push_back(pOne); // top left
			geom.positions.push_back(pThree); // bottom right
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);
			geom.colors.emplace_back(0.f, 1.f, 1.f);

			// add the normals to the geometry
			// since this is a unit sphere, the normals are simply just the positions of each vertex
			geom.normals.push_back(pThree); // bottom right
			geom.normals.push_back(pOne); // top left
			geom.normals.push_back(pTwo); // bottom left
			geom.normals.push_back(pFour); // top right
			geom.normals.push_back(pOne); // top left
			geom.normals.push_back(pThree); // bottom right

			// calculate the texture coordinates
			geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(slices), 1.0f - static_cast<float>(j + 1) / static_cast<float>(stacks)); // bottom right
			geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(slices), 1.0f - static_cast<float>(j) / static_cast<float>(stacks)); // top left
			geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(slices), 1.0f - static_cast<float>(j + 1) / static_cast<float>(stacks)); // bottom left
			geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(slices), 1.0f - static_cast<float>(j) / static_cast<float>(stacks)); // top right
			geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(slices), 1.0f - static_cast<float>(j) / static_cast<float>(stacks)); // top left
			geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(slices), 1.0f - static_cast<float>(j + 1) / static_cast<float>(stacks)); // bottom right
		}
	}

	return geom;
}

CPU_Geometry ShapeGenerator::Ring(float radius, float width, float resolution)
{
	CPU_Geometry geom{};

	// generate a single curve to be revolved
	std::vector<glm::vec3> outerRing{};
	std::vector < glm::vec3 > innerRing{};

	for (float u = 0; u < glm::two_pi<float>(); u += glm::two_pi<float>() / resolution)
	{
		outerRing.emplace_back(radius * glm::sin(u), 0.0f, radius * glm::cos(u));
		innerRing.emplace_back((radius - width) * glm::sin(u), 0.0f, (radius - width) * glm::cos(u));
	}

	// guarantee a section at the end
	outerRing.emplace_back(radius * glm::sin(glm::two_pi<float>()), radius * glm::cos(glm::two_pi<float>()), 0.0f);
	innerRing.emplace_back((radius - width) * glm::sin(glm::two_pi<float>()), (radius - width) * glm::cos(glm::two_pi<float>()), 0.0f);

	for (size_t i = 0; i < outerRing.size() - 1; i++)
	{
		glm::vec3 pOne = outerRing[i]; // outer left
		glm::vec3 pTwo = outerRing[i + 1]; // outer right
		glm::vec3 pThree = innerRing[i]; // inner left
		glm::vec3 pFour = innerRing[i + 1]; // inner right

		geom.positions.push_back(pOne);
		geom.positions.push_back(pTwo);
		geom.positions.push_back(pThree);
		geom.colors.emplace_back(0.f, 1.f, 1.f);
		geom.colors.emplace_back(0.f, 1.f, 1.f); 
		geom.colors.emplace_back(0.f, 1.f, 1.f);

		geom.positions.push_back(pTwo);
		geom.positions.push_back(pFour);
		geom.positions.push_back(pThree);
		geom.colors.emplace_back(0.f, 1.f, 1.f);
		geom.colors.emplace_back(0.f, 1.f, 1.f);
		geom.colors.emplace_back(0.f, 1.f, 1.f);

		// add the normals to the geometry
		geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

		// calculate the texture coordinates
		geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(resolution), 1.0f - static_cast<float>(i) / static_cast<float>(resolution)); // outer left
		geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(resolution), 1.0f - static_cast<float>(i + 1) / static_cast<float>(resolution)); // outer right
		geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(resolution), 1.0f - static_cast<float>(i) / static_cast<float>(resolution)); // inner left
		geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(resolution), 1.0f - static_cast<float>(i + 1) / static_cast<float>(resolution)); // outer right
		geom.uvs.emplace_back(static_cast<float>(i + 1) / static_cast<float>(resolution), 1.0f - static_cast<float>(i + 1) / static_cast<float>(resolution)); // inner right
		geom.uvs.emplace_back(static_cast<float>(i) / static_cast<float>(resolution), 1.0f - static_cast<float>(i) / static_cast<float>(resolution)); // inner left
	}

	glm::vec3 pOne = outerRing[outerRing.size() - 1]; // outer left
	glm::vec3 pTwo = outerRing[0]; // outer right
	glm::vec3 pThree = innerRing[innerRing.size() - 1]; // inner left
	glm::vec3 pFour = innerRing[0]; // inner right

	geom.positions.push_back(pOne);
	geom.positions.push_back(pTwo);
	geom.positions.push_back(pThree);
	geom.colors.emplace_back(0.f, 1.f, 1.f);
	geom.colors.emplace_back(0.f, 1.f, 1.f);
	geom.colors.emplace_back(0.f, 1.f, 1.f);

	geom.positions.push_back(pTwo);
	geom.positions.push_back(pFour);
	geom.positions.push_back(pThree);
	geom.colors.emplace_back(0.f, 1.f, 1.f);
	geom.colors.emplace_back(0.f, 1.f, 1.f);
	geom.colors.emplace_back(0.f, 1.f, 1.f);
	// add the normals to the geometry
	geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	geom.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	// calculate the texture coordinates
	geom.uvs.emplace_back(static_cast<float>(outerRing.size() - 1) / static_cast<float>(resolution), 1.0f - static_cast<float>(outerRing.size() - 1) / static_cast<float>(resolution)); // outer left
	geom.uvs.emplace_back(static_cast<float>(0) / static_cast<float>(resolution), 1.0f - static_cast<float>(0) / static_cast<float>(resolution)); // outer right
	geom.uvs.emplace_back(static_cast<float>(outerRing.size() - 1) / static_cast<float>(resolution), 1.0f - static_cast<float>(outerRing.size() - 1) / static_cast<float>(resolution)); // inner left
	geom.uvs.emplace_back(static_cast<float>(0) / static_cast<float>(resolution), 1.0f - static_cast<float>(0) / static_cast<float>(resolution)); // outer right
	geom.uvs.emplace_back(static_cast<float>(0) / static_cast<float>(resolution), 1.0f - static_cast<float>(0) / static_cast<float>(resolution)); // inner right
	geom.uvs.emplace_back(static_cast<float>(outerRing.size() - 1) / static_cast<float>(resolution), 1.0f - static_cast<float>(outerRing.size() - 1) / static_cast<float>(resolution)); // inner left
	
	return geom;
}

std::vector<std::vector< glm::vec3 >> ShapeGenerator::GenerateSphere(float radius, int slices, int stacks)
{
	std::vector<std::vector<glm::vec3>> positions{};
	// generate a single curve to be revolved
	std::vector<glm::vec3> singleCurve{};
	for (float u = 0; u < glm::pi<float>(); u += glm::pi<float>() / stacks)
	{
		singleCurve.emplace_back(radius * glm::sin(u), radius * glm::cos(u), 0.0f);
	}
	// guarantee the section at the bottom
	singleCurve.emplace_back(0.0f, -radius, 0.0f);

	// generate a sphere via surface of revolution
	for (float u = 0; u < glm::two_pi<float>(); u += glm::two_pi<float>() / slices)
	{
		std::vector<glm::vec3> curve{};
		for (glm::vec3 point : singleCurve)
		{
			curve.emplace_back(point.x * glm::cos(u), point.y, point.x * glm::sin(u));
		}
		positions.emplace_back(curve);
	}
	// guarantee the section at end
	std::vector<glm::vec3> curve{};
	for (glm::vec3 point : singleCurve)
	{
		curve.emplace_back(point.x * glm::cos(glm::two_pi<float>()), point.y, point.x * glm::sin(glm::two_pi<float>()));
	}
	positions.emplace_back(curve);

	return positions;
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
