#pragma once
#include <cstdint>
#include <numbers>
#include "VertexData.h"
#include <wrl.h>
#include <d3d12.h>
class Sphere {
public:
	VertexData* CreateVertexData(VertexData* vertexData);

private:
	const float pi_ = static_cast<float>(std::numbers::pi);
	const uint32_t kSubdivision_ = 16;
};
