#include "SphereMeshGenerator.h"
#include "MathUtility.h"

std::vector<VertexData> SphereMeshGenerator::GenerateVertices() const {
	std::vector<VertexData> vertices;
	const float kLonEvery = MathUtility::pi_ * 2.0f / static_cast<float>(kSubdivision_);
	const float kLatEvery = MathUtility::pi_ / static_cast<float>(kSubdivision_);

	vertices.reserve((kSubdivision_ + 1) * (kSubdivision_ + 1) * 6);
	for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
		float lat = -MathUtility::pi_ / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			float u1 = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision_);
			float u2 = static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision_);
			float v1 = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision_);
			float v2 = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision_);

			vertices.push_back({
			    MakeSphereVertex(lat, lon), {u1, v1}
            });
			vertices.push_back({
			    MakeSphereVertex(lat + kLatEvery, lon), {u1, v2}
            });
			vertices.push_back({
			    MakeSphereVertex(lat, lon + kLonEvery), {u2, v1}
            });

			vertices.push_back({
			    MakeSphereVertex(lat, lon + kLonEvery), {u2, v1}
            });
			vertices.push_back({
			    MakeSphereVertex(lat + kLatEvery, lon), {u1, v2}
            });
			vertices.push_back({
			    MakeSphereVertex(lat + kLatEvery, lon + kLonEvery), {u2, v2}
            });
		}
	}
	return vertices;
}

Vector4 SphereMeshGenerator::MakeSphereVertex(float lat, float lon) const { 
	return {std::cosf(lat) * std::cosf(lon), std::sinf(lat), std::cosf(lat) * std::sinf(lon), 1.0f}; 
}
