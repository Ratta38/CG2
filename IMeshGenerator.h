#pragma once
#include "VertexData.h"
#include <vector>
class IMeshGenerator
{
public:
	virtual ~IMeshGenerator() = default;
	virtual std::vector<VertexData> GenerateVertices() const = 0;
};