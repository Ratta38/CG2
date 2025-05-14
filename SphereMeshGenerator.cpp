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

			// 頂点位置を事前に生成
			Vector4 p1 = MakeSphereVertex(lat, lon);
			Vector4 p2 = MakeSphereVertex(lat + kLatEvery, lon);
			Vector4 p3 = MakeSphereVertex(lat, lon + kLonEvery);
			Vector4 p4 = MakeSphereVertex(lat + kLatEvery, lon + kLonEvery);

			// UV
			float u1 = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision_);
			float u2 = static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision_);
			float v1 = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision_);
			float v2 = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision_);

			// 法線を正規化して代入
			auto GetNormal = [](const Vector4& v) {
				Vector3 n = {v.x, v.y, v.z};
				float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
				return Vector3{n.x / len, n.y / len, n.z / len};
			};

			vertices.push_back({
			    p1, {u1, v1},
                 GetNormal(p1)
            });
			vertices.push_back({
			    p2, {u1, v2},
                 GetNormal(p2)
            });
			vertices.push_back({
			    p3, {u2, v1},
                 GetNormal(p3)
            });

			vertices.push_back({
			    p3, {u2, v1},
                 GetNormal(p3)
            });
			vertices.push_back({
			    p2, {u1, v2},
                 GetNormal(p2)
            });
			vertices.push_back({
			    p4, {u2, v2},
                 GetNormal(p4)
            });
		}
	}

	return vertices;
}

Vector4 SphereMeshGenerator::MakeSphereVertex(float lat, float lon) const { 
	return {std::cosf(lat) * std::cosf(lon), std::sinf(lat), std::cosf(lat) * std::sinf(lon), 1.0f}; 
}
