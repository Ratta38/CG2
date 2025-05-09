#include "Model.h"
#include "MathUtility.h"
#include "VertexData.h"
#include "WinApp.h"
#include <cassert>

using namespace Microsoft::WRL;

Model::~Model() {
}

void Model::SetRootSignature(ComPtr<ID3D12RootSignature> rs) { rootSignature_ = rs; }

void Model::SetPipelineState(ComPtr<ID3D12PipelineState> pso) { pipelineState_ = pso; }

void Model::SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) { srvHandle_ = handle; }

void Model::Initialize(ComPtr<ID3D12Device> device) {
	// 頂点リソースの作成
	vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 6);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = {};
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0].position = {-0.5f, -0.5f, 0.0f, 1.0f};
	vertexData[0].texcoord = {0.0f, 1.0f};
	// 上
	vertexData[1].position = {0.0f, 0.5f, 0.0f, 1.0f};
	vertexData[1].texcoord = {0.5f, 0.0f};
	// 右下
	vertexData[2].position = {0.5f, -0.5f, 0.0f, 1.0f};
	vertexData[2].texcoord = {1.0f, 1.0f};

	// 左下2
	vertexData[3].position = {-0.5f, -0.5f, 0.5f, 1.0f};
	vertexData[3].texcoord = {0.0f, 1.0f};
	// 上2
	vertexData[4].position = {0.0f, 0.0f, 0.0f, 1.0f};
	vertexData[4].texcoord = {0.5f, 0.0f};
	// 右下2
	vertexData[5].position = {0.5f, -0.5f, -0.5f, 1.0f};
	vertexData[5].texcoord = {1.0f, 1.0f};

	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(device, sizeof(Vector4));
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 三角形の色
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	// 今回は赤を書き込んでみる
	*materialData_ = color_;

	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(device, sizeof(Matrix4x4));
	// データを書き込む
	transformMatrixData_ = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformMatrixData_));
	// 単位行列を書き込んでおく
	*transformMatrixData_ = MathUtility::MakeIdentity4x4();

	transform_ = {
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

	cameraTransform_ = {
	    {1.0f, 1.0f, 1.0f },
        {0.0f, 0.0f, 0.0f },
        {0.0f, 0.0f, -5.0f}
    };
	wvpResource_->Unmap(0, nullptr);
	materialResource_->Unmap(0, nullptr);
}

void Model::Update() {
	// 三角形
	transform_.rotate.y += 0.01f;
	Matrix4x4 worldMatrix = MathUtility::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MathUtility::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = MathUtility::Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MathUtility::MakePerspectiveFovMatrix(0.45f, static_cast<float>(WinApp::kClientWidth) / static_cast<float>(WinApp::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MathUtility::Multiply(worldMatrix, MathUtility::Multiply(viewMatrix, projectionMatrix));
	*transformMatrixData_ = worldViewProjectionMatrix;
	*materialData_ = color_; // 色の更新
}

void Model::Draw(ComPtr<ID3D12GraphicsCommandList> commandList) {
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipelineState_.Get());             // PSOを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// wvp用のBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	commandList->SetGraphicsRootDescriptorTable(2, srvHandle_);
	// 描画!(DrawCall/ドローコール)。6頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(6, 1, 0, 0);
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
