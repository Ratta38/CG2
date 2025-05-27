#pragma once
#include "VertexData.h"
#include <vector>
class MeshData {
public:
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
};
