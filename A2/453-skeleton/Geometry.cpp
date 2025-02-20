#include "Geometry.h"

#include <utility>

//======================================================================================================================

GPU_Geometry::GPU_Geometry()
    : vertexArrayObject()
    , positionsBuffer(0, sizeof(Position) / sizeof(float), GL_FLOAT)
    , uvsBuffer(1, sizeof(UV) / sizeof(float), GL_FLOAT)
{
}

//======================================================================================================================

void GPU_Geometry::Update(CPU_Geometry const & data)
{
    assert(data.positions.size() == data.uvs.size());
    UpdatePositions(data.positions.size(), data.positions.data());
    UpdateUVs(data.uvs.size(), data.uvs.data());
}

//======================================================================================================================

void GPU_Geometry::UpdatePositions(size_t const count, Position const * data)
{
    positionsBuffer.uploadData(sizeof(Position) * count, data, GL_STATIC_DRAW);
}

//======================================================================================================================

void GPU_Geometry::UpdateUVs(size_t const count, UV const * data)
{
    uvsBuffer.uploadData(sizeof(UV) * count, data, GL_STATIC_DRAW);
}

//======================================================================================================================
