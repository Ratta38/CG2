#include "Sprite.h"
#include "VertexData.h"
#include "Matrix4x4.h"
#include "MathUtility.h"
#include "WinApp.h"
#include <cassert>

using namespace Microsoft::WRL;

Sprite::~Sprite() {
}

void Sprite::Initialize(ComPtr<ID3D12Device> device) {
	// スプライトの頂点リソースを作る
	vertexResource_ = CreateBufferResource(device.Get(), sizeof(VertexData) * 6);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 1枚目の三角形
	vertexData[0].position = {0.0f, 360.0f, 0.0f, 1.0f}; // 左下
	vertexData[0].texcoord = {0.0f, 1.0f};
	vertexData[1].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 左上
	vertexData[1].texcoord = {0.0f, 0.0f};
	vertexData[2].position = {640.0f, 360.0f, 0.0f, 1.0f}; // 右下
	vertexData[2].texcoord = {1.0f, 1.0f};
	// 2枚目の三角形
	vertexData[3].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 左上
	vertexData[3].texcoord = {0.0f, 0.0f};
	vertexData[4].position = {640.0f, 0.0f, 0.0f, 1.0f}; // 右上
	vertexData[4].texcoord = {1.0f, 0.0f};
	vertexData[5].position = {640.0f, 360.0f, 0.0f, 1.0f}; // 右下
	vertexData[5].texcoord = {1.0f, 1.0f};

	for (uint32_t i = 0; i < 6; ++i) {
		vertexData[i].normal = {0.0f, 0.0f, -1.0f};
	}

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformMatrixResource_ = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// transformationMatrixData_ = nullptr;
	// 書き込むためのアドレスを取得
	transformMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 単位行列を書き込んでおく
	*transformationMatrixData_ = {MathUtility::MakeIdentity4x4(), worldMatrixSprite_};

	// Sprite用のマテリアルリソースを作る
	materialResourceSprite_ = CreateBufferResource(device.Get(), sizeof(Material));
	Material* materialDataSprite = nullptr;
	// Mapしてデータを書き込む
	materialResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	material_ = {1.0f, 1.0f, 1.0f, 1.0f}, false;
	*materialDataSprite = material_;

	// SpriteはLightingしないのでfalseを設定する
	materialDataSprite->enableLighting = false;

	transform_ = {
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
	transformMatrixResource_->Unmap(0, nullptr);
	materialResourceSprite_->Unmap(0, nullptr);
}

void Sprite::Update() {
	// スプライト
	worldMatrixSprite_ = MathUtility::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrixSprite = MathUtility::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MathUtility::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::kClientWidth), static_cast<float>(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = MathUtility::Multiply(worldMatrixSprite_, MathUtility::Multiply(viewMatrixSprite, projectionMatrixSprite));
	*transformationMatrixData_ = {worldViewProjectionMatrixSprite, worldMatrixSprite_};
}

void Sprite::Draw(ComPtr<ID3D12GraphicsCommandList> commandList) {
	// Spriteの描画。
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	// TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformMatrixResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, srvHandle_);
	// 描画! (DrawCall/ドローコール)
	commandList->DrawInstanced(6, 1, 0, 0);
}

void Sprite::SetSrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { srvHandle_ = srvHandle; }

ComPtr<ID3D12Resource> Sprite::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeBytes) { 
	if (!device)
		return nullptr;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う
	// 頂点にリソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Alignment = 0;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(resource.GetAddressOf()));
	assert(SUCCEEDED(hr));

	return resource;
}
