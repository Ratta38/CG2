#include "Model.h"
#include "MathUtility.h"
#include "VertexData.h"
#include "WinApp.h"
#include <DirectXMath.h>
#include <cassert>

using namespace Microsoft::WRL;
using namespace DirectX;

Model::~Model() {}

void Model::SetRootSignature(ComPtr<ID3D12RootSignature> rs) { rootSignature_ = rs; }

void Model::SetPipelineState(ComPtr<ID3D12PipelineState> pso) { pipelineState_ = pso; }

void Model::SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) { srvHandle_ = handle; }

void Model::SetTextureHandle2(D3D12_GPU_DESCRIPTOR_HANDLE handle) { srvHandle2_ = handle; }

void Model::SetTextureHandle3(D3D12_GPU_DESCRIPTOR_HANDLE handle) { srvHandle3_ = handle; }

void Model::Initialize(ComPtr<ID3D12Device> device, const IMeshGenerator& meshGen) {
	MeshData meshData = meshGen.GenerateMeshData();

	vertexCount_ = static_cast<uint32_t>(meshData.vertices.size());
	indexCount_ = static_cast<uint32_t>(meshData.indices.size());

	// 頂点リソースの作成
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * vertexCount_);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * vertexCount_);
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// インデックスデータ
	indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * indexCount_);
	indexBufferView_ = {};
	// リソースの先頭アドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexCount_;
	// インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	// 頂点リソースにデータの書き込み
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, meshData.vertices.data(), sizeof(VertexData) * meshData.vertices.size());
	vertexResource_->Unmap(0, nullptr);

	// インデックスデータの書き込み
	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, meshData.indices.data(), sizeof(uint32_t) * meshData.indices.size());
	indexResource_->Unmap(0, nullptr);

	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(device, sizeof(Material));
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 三角形の色
	material_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	material_.enableLighting = false;
	// uvTransformなどのデータを設定
	material_.uvTransform = MathUtility::MakeIdentity4x4();
	// 今回は赤を書き込んでみる
	*materialData_ = material_;
	materialResource_->Unmap(0, nullptr);

	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(device, sizeof(TransformationMatrix));
	// データを書き込む
	transformMatrixData_ = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformMatrixData_));
	// 単位行列を書き込んでおく
	*transformMatrixData_ = {MathUtility::MakeIdentity4x4(), worldMatrix_};
	wvpResource_->Unmap(0, nullptr);

	// 平行光源用のリソースを作る
	directionalLightResource_ = CreateBufferResource(device, sizeof(DirectionalLight));
	directionalLightData_ = nullptr;
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLight_.direction = {0.0f, -1.0f, 0.0f};
	directionalLight_.intensity = 1.0f;
	// 1. Vector3 → XMVECTOR 変換
	XMVECTOR dirVec = XMVectorSet(
	    directionalLight_.direction.x, directionalLight_.direction.y, directionalLight_.direction.z,
	    0.0f // ← w成分は不要なので0
	);
	// 2. 正規化
	dirVec = XMVector3Normalize(dirVec);
	// 3. XMVECTOR → Vector3 に戻す
	XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&directionalLight_.direction), dirVec);
	*directionalLightData_ = directionalLight_;
	directionalLightResource_->Unmap(0, nullptr);

	transform_ = {
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

	cameraTransform_ = {
	    {1.0f, 1.0f, 1.0f  },
        {0.0f, 0.0f, 0.0f  },
        {0.0f, 0.0f, -10.0f}
    };

	uvTransform_ = {
	    {1.0f, 1.0f, 1.0f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 1.0f},
	};

	useMonsterBall_ = true;

	// RGBを0.01f分推移させる
	stepColor_ = {-0.00f, -0.01f, -0.01f, -0.01f};
}

void Model::Update() {
	// 三角形 回転
	transform_.rotate.y += 0.01f;

	// 座標変換
	worldMatrix_ = MathUtility::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MathUtility::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = MathUtility::Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MathUtility::MakePerspectiveFovMatrix(0.45f, static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MathUtility::Multiply(worldMatrix_, MathUtility::Multiply(viewMatrix, projectionMatrix));

	/*Matrix4x4 uvTransformMatrix = MathUtility::MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix = MathUtility::Multiply(uvTransformMatrix, MathUtility::MakeRollRotateMatrix(uvTransform_.rotate.z));
	uvTransformMatrix = MathUtility::Multiply(uvTransformMatrix, MathUtility::MakeTranslateMatrix(uvTransform_.translate));
	materialData_->uvTransform = uvTransformMatrix;*/

	// 三角形のデータ更新
	*transformMatrixData_ = {worldViewProjectionMatrix, worldMatrix_};
	*materialData_ = material_; // 色の更新
	*directionalLightData_ = directionalLight_;
}

void Model::Draw(ComPtr<ID3D12GraphicsCommandList> commandList) {
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());       // PSOを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView_); // IBVを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// wvp用のBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall_ ? srvHandle2_ : srvHandle3_);
	// ライティングCBufferの場所を指定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 描画!(DrawCall/ドローコール)。6頂点で1つのインスタンス。インスタンスについては今後
	//commandList->DrawInstanced(vertexCount_, 1, 0, 0);
	commandList->DrawIndexedInstanced(indexCount_, 1, 0, 0, 0);
}

ComPtr<ID3D12Resource> Model::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeBytes) {
	if (!device)
		return nullptr;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う
	// 頂点にリソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

void Model::UpdateColor(Material& material) {
	material.color.x += stepColor_.x;
	material.color.y += stepColor_.y;
	material.color.z += stepColor_.z;

	// 値が0.0fまたは1.0fに達したら増減方向を反転
	if (material.color.x >= 1.0f || material.color.x <= 0.0f) {
		stepColor_.x *= -1;
	}
	if (material.color.y >= 1.0f || material.color.y <= 0.0f) {
		stepColor_.y *= -1;
	}
	if (material.color.z >= 1.0f || material.color.z <= 0.0f) {
		stepColor_.z *= -1;
	}
}
