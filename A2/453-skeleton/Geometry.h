#pragma once

//------------------------------------------------------------------------------
// This file contains simple classes for storing geomtery on the CPU and the GPU
// Later assignments will require you to expand these classes or create your own
// similar classes with the needed functionality
//------------------------------------------------------------------------------

#include "VertexArray.h"
#include "VertexBuffer.h"

#include <glm/glm.hpp>
#include <vector>

using Position = glm::vec3;
using UV = glm::vec2;

// List of vertices and texture coordinates using std::vector and glm::vec3
struct CPU_Geometry
{
	std::vector<Position> positions;
	std::vector<UV> uvs;             // UV = Texture coordinate
};

// VAO and two VBOs for storing vertices and texture coordinates, respectively
class GPU_Geometry {
public:
	GPU_Geometry();
	// Public interface
	void bind() const
    {
		vertexArrayObject.bind();
	}

    void Update(CPU_Geometry const & data);

    void UpdatePositions(size_t count, Position const * data);

    void UpdateUVs(size_t count, UV const * data);

protected:

    VertexArray vertexArrayObject;

	VertexBuffer positionsBuffer;
	VertexBuffer uvsBuffer;

};
