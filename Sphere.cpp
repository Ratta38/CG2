#include "Sphere.h"
#include <cmath>

using namespace Microsoft::WRL;

VertexData* Sphere::CreateVertexData(VertexData* vertexData) {
	const float kLonEvery = pi_ * 2.0f / static_cast<float>(kSubdivision_);
	const float kLatEvery = pi_ / static_cast<float>(kSubdivision_);

	for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
		float lat = -pi_ / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision_ + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			float u1 = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision_);
			float u2 = static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision_);
			float v1 = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision_);
			float v2 = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision_);

			vertexData[start + 0].position = {std::cosf(lat) * std::cosf(lon), 
				std::sinf(lat), 
				std::cosf(lat) * std::sinf(lon), 
				1.0f};
			vertexData[start + 0].texcoord = {u1, v1};

			vertexData[start + 1].position = {std::cosf(lat + kLatEvery) * std::cosf(lon), 
				std::sinf(lat + kLatEvery), 
				std::cosf(lat + kLatEvery) * std::sinf(lon), 
				1.0f};
			vertexData[start + 1].texcoord = {u1, v2};

			vertexData[start + 2].position = {std::cosf(lat) * std::cosf(lon + kLonEvery), 
				std::sinf(lat), 
				std::cosf(lat) * std::sinf(lon + kLonEvery), 
				1.0f};
			vertexData[start + 2].texcoord = {u2, v1};

			vertexData[start + 3] = vertexData[start + 2];

			vertexData[start + 4] = vertexData[start + 1];

			vertexData[start + 5].position = {std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery), 
				std::sinf(lat + kLatEvery), 
				std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery), 
				1.0f};
			vertexData[start + 5].texcoord = {u2, v2};
		}
	}
	return vertexData;
}

//void Sphere::Draw(ComPtr<ID3D12GraphicsCommandList> commandList) {
//	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
//	commandList->SetGraphicsRootSignature(rootSignature_.Get());
//	commandList->SetPipelineState(pipelineState_.Get());       // PSOを設定
//	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
//	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
//	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// マテリアルCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
//	// wvp用のBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
//	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
//	commandList->SetGraphicsRootDescriptorTable(2, srvHandle_);
//	// 描画!(DrawCall/ドローコール)。6頂点で1つのインスタンス。インスタンスについては今後
//	commandList->DrawInstanced(1536, 1, 0, 0);
//}
