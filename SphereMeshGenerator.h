#pragma once
#include "IMeshGenerator.h"
#include <cmath>
class SphereMeshGenerator : public IMeshGenerator
{
public:
	SphereMeshGenerator(uint32_t subdivision) : kSubdivision_(subdivision) {}

	MeshData GenerateMeshData() const override;

private:
	Vector4 MakeSphereVertex(float lat, float lon) const;

private:
	const uint32_t kSubdivision_ = 16;
};
