#pragma once
#include "Matrix4x4.h"
#include "Transform.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include "SphereMeshGenerator.h"
class Model {
public:
	~Model();
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, const IMeshGenerator& meshGen);
	void Update();
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);
	Vector4& GetColor() { return color_; }
	Transform& GetTransform() { return transform_; }

	Microsoft::WRL::ComPtr<ID3D12Resource> GetMaterialResource() { return materialResource_; }
	Microsoft::WRL::ComPtr<ID3D12Resource> GetWvpResource() { return wvpResource_; }
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView_; }

	void SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rs);
	void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pso);
	void SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeBytes);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Transform transform_;
	Transform cameraTransform_;
	Matrix4x4* transformMatrixData_;
	Vector4 color_ = {};
	Vector4* materialData_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle_;

	// 描画用に頂点数を取得
	uint32_t vertexCount_ = 0;
};
