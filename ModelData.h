#pragma once
#include "VertexData.h"
#include "MaterialData.h"
#include <vector>
struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};