#pragma once
#include "Matrix4x4.h"
#include "Transform.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include "Material.h"
#include "TransformationMatrix.h"
class Sprite {
public:
	~Sprite();
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);
	void Update();
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView_; }
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetTransformMatrixResource() { return transformMatrixResource_; }
	Transform& GetTransform() { return transform_; }
	Transform& GetUVTransform() { return uvTransform_; }
	void SetSrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeBytes);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformMatrixResource_;
	TransformationMatrix* transformationMatrixData_;
	Transform transform_;
	Transform uvTransform_;
	Matrix4x4 worldMatrixSprite_;
	Material* materialData_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite_;
	Material material_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
};
