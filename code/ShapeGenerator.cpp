#include "ShapeGenerator.hpp"

#include <glm/ext/matrix_transform.hpp>

//======================================================================================================================

CPU_Geometry ShapeGenerator::Sphere(float const radius, int const slices, int const stacks)
{
	// generate a single curve to be revolved
	std::vector<std::vector<glm::vec3>> positions = GenerateSphere(radius, slices, stacks);
	
	CPU_Geometry geom{};

	glm::vec3 pOne; // top left
	glm::vec3 pTwo; // bottom left
	glm::vec3 pThree; // bottom right
	glm::vec3 pFour; // top right
	float uv;
	float uv2;
	float uv3;
	float uv4;
	// triangulate each section and add it to the geometry
	for (size_t i = 0; i < positions.size() - 1; i++) {
		for (size_t j = 0; j < positions[i].size() - 1; j++) {
			pOne = positions[i][j]; // top left
			pTwo = positions[i][j + 1]; // bottom left
			pThree = positions[i + 1][j + 1]; // bottom right
			pFour = positions[i + 1][j]; // top right

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

			uv = static_cast<float>(i) / static_cast<float>(slices);
			uv2 = static_cast<float>(i + 1) / static_cast<float>(slices);
			uv3 = static_cast<float>(j) / static_cast<float>(stacks);
			uv4 = static_cast<float>(j + 1) / static_cast<float>(stacks);
			// calculate the texture coordinates
			geom.uvs.emplace_back(uv, 1.0f - uv3); // top left
			geom.uvs.emplace_back(uv2, 1.0f - uv4); // bottom right
			geom.uvs.emplace_back(uv, 1.0f - uv4); // bottom left
			geom.uvs.emplace_back(uv, 1.0f - uv3); // top left
			geom.uvs.emplace_back(uv2, 1.0f - uv3); // top right
			geom.uvs.emplace_back(uv2, 1.0f - uv4); // bottom right
		}
	}

	return geom;
}

CPU_Geometry ShapeGenerator::BackgroundSphere(float const radius, int const slices, int const stacks)
{
	// generate a single curve to be revolved
	std::vector<std::vector<glm::vec3>> positions = GenerateSphere(radius, slices, stacks);

	CPU_Geometry geom{};

	glm::vec3 pOne; // top left
	glm::vec3 pTwo; // bottom left
	glm::vec3 pThree; // bottom right
	glm::vec3 pFour; // top right
	float uv;
	float uv2;
	float uv3;
	float uv4;
	// triangulate each section and add it to the geometry
	// winding order is opposite of Sphere()
	for (size_t i = 0; i < positions.size() - 1; i++) {
		for (size_t j = 0; j < positions[i].size() - 1; j++) {
			pOne = positions[i][j]; // top left
			pTwo = positions[i][j + 1]; // bottom left
			pThree = positions[i + 1][j + 1]; // bottom right
			pFour = positions[i + 1][j]; // top right

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
			geom.normals.push_back(pThree); // bottom right
			geom.normals.push_back(pOne); // top left
			geom.normals.push_back(pTwo); // bottom left
			geom.normals.push_back(pFour); // top right
			geom.normals.push_back(pOne); // top left
			geom.normals.push_back(pThree); // bottom right

			uv = static_cast<float>(i) / static_cast<float>(slices);
			uv2 = static_cast<float>(i + 1) / static_cast<float>(slices);
			uv3 = static_cast<float>(j) / static_cast<float>(stacks);
			uv4 = static_cast<float>(j + 1) / static_cast<float>(stacks);
			// calculate the texture coordinates
			geom.uvs.emplace_back(uv2, 1.0f - uv4); // bottom right
			geom.uvs.emplace_back(uv, 1.0f - uv3); // top left
			geom.uvs.emplace_back(uv, 1.0f - uv4); // bottom left
			geom.uvs.emplace_back(uv2, 1.0f - uv3); // top right
			geom.uvs.emplace_back(uv, 1.0f - uv3); // top left
			geom.uvs.emplace_back(uv2, 1.0f - uv4); // bottom right
		}
	}

	return geom;
}

CPU_Geometry ShapeGenerator::Ring(float radius, float width, int resolution)
{
	CPU_Geometry geom{};

	std::vector < glm::vec3 > innerRing{};
	std::vector<glm::vec3> outerRing{};

	// generate the inner and outer rings
	for (float u = 0; u < glm::two_pi<float>(); u += glm::two_pi<float>() / resolution)
	{
		innerRing.emplace_back(radius * glm::cos(u), 0.0f, radius * glm::sin(u));
		outerRing.emplace_back((radius + width) * glm::cos(u), 0.0f, (radius + width) * glm::sin(u));
	}

	// guarantee a section at the end
	innerRing.emplace_back(radius, 0.0f,  0.0f);
	outerRing.emplace_back((radius + width) * glm::cos(glm::two_pi<float>()), 0.0f, 0.0f);

	glm::vec3 pOne; // outer first
	glm::vec3 pTwo; // outer second
	glm::vec3 pThree; // inner first
	glm::vec3 pFour; // inner second
	float uv;
	float uv2;
	// triangulate the outer and inner rings
	for (size_t i = 0; i < outerRing.size() - 1; i++)
	{
		pOne = outerRing[i]; // outer first
		pTwo = outerRing[i + 1]; // outer second
		pThree = innerRing[i]; // inner first
		pFour = innerRing[i + 1]; // inner second

		geom.positions.push_back(pOne);
		geom.positions.push_back(pThree);
		geom.positions.push_back(pTwo);
		geom.colors.emplace_back(0.f, 1.f, 1.f);
		geom.colors.emplace_back(0.f, 1.f, 1.f); 
		geom.colors.emplace_back(0.f, 1.f, 1.f);

		geom.positions.push_back(pTwo);
		geom.positions.push_back(pThree);
		geom.positions.push_back(pFour);
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

		uv = static_cast<float>(i) / static_cast<float>(resolution);
		uv2 = static_cast<float>(i + 1) / static_cast<float>(resolution);

		// calculate the texture coordinates
		geom.uvs.emplace_back(1.0f, uv); // outer first
		geom.uvs.emplace_back(0.0f, uv); // inner first
		geom.uvs.emplace_back(1.0f, uv2); // outer second
		geom.uvs.emplace_back(1.0f, uv2); // outer second
		geom.uvs.emplace_back(0.0f, uv); // inner first
		geom.uvs.emplace_back(0.0f, uv2); // inner second

	}

	// connect the start and end of the rings together
	pOne = outerRing[outerRing.size() - 1]; // outer first
	pTwo = outerRing[0]; // outer second
	pThree = innerRing[innerRing.size() - 1]; // inner first
	pFour = innerRing[0]; // inner second

	geom.positions.push_back(pOne);
	geom.positions.push_back(pThree);
	geom.positions.push_back(pTwo);
	geom.colors.emplace_back(0.f, 1.f, 1.f);
	geom.colors.emplace_back(0.f, 1.f, 1.f);
	geom.colors.emplace_back(0.f, 1.f, 1.f);

	geom.positions.push_back(pTwo);
	geom.positions.push_back(pThree);
	geom.positions.push_back(pFour);
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
	uv = static_cast<float>(outerRing.size() - 1) / static_cast<float>(resolution);
	uv2 = static_cast<float>(0) / static_cast<float>(resolution);
	
	geom.uvs.emplace_back(1.0f, uv); // outer first
	geom.uvs.emplace_back(0.0f, uv); // inner first
	geom.uvs.emplace_back(1.0f, uv2); // outer second
	geom.uvs.emplace_back(1.0f, uv2); // outer second
	geom.uvs.emplace_back(0.0f, uv); // inner first
	geom.uvs.emplace_back(0.0f, uv2); // inner second

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

	// generate a sphere using surface of revolution
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
