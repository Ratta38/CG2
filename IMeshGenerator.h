#pragma once
#include "MeshData.h"
#include <vector>
class IMeshGenerator
{
public:
	virtual ~IMeshGenerator() = default;
	virtual MeshData GenerateMeshData() const = 0;
};