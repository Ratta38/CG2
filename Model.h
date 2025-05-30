#pragma once
#include "Material.h"
#include "Matrix4x4.h"
#include "SphereMeshGenerator.h"
#include "Transform.h"
#include "Vector4.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "MeshData.h"
#include "ModelData.h"
#include "MaterialData.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include <string>
class Model {
public:
	~Model();
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, const IMeshGenerator& meshGen);
	void Update();
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);
	Material& GetColor() { return material_; }
	Transform& GetTransform() { return transform_; }

	Microsoft::WRL::ComPtr<ID3D12Resource> GetMaterialResource() { return materialResource_; }
	Microsoft::WRL::ComPtr<ID3D12Resource> GetWvpResource() { return wvpResource_; }
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView_; }
	bool& GetUseMonsterBallRef() { return useMonsterBall_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle() { return srvHandle_; }
	ModelData& GetModelData() { return modelData_; }
	Matrix4x4 GetCameraMatrix() { return cameraMatrix_; }

	void SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rs);
	void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pso);
	void SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle);
	void SetTextureHandle2(D3D12_GPU_DESCRIPTOR_HANDLE handle);
	void SetTextureHandle3(D3D12_GPU_DESCRIPTOR_HANDLE handle);
	void SetViewMatrix(Matrix4x4 viewMatrix) { viewMatrix_ = viewMatrix; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeBytes);
	void UpdateColor(Material& material);
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	Transform transform_;
	Transform cameraTransform_;
	Matrix4x4 cameraMatrix_;
	Matrix4x4 viewMatrix_;
	Transform uvTransform_;
	Matrix4x4 worldMatrix_;
	TransformationMatrix* transformMatrixData_;
	Material material_ = {};
	Material* materialData_;

	DirectionalLight* directionalLightData_;
	DirectionalLight directionalLight_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle2_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle3_;

	// 画像切り替え用の変数
	bool useMonsterBall_;

	// 描画用に頂点数を取得
	uint32_t vertexCount_;
	uint32_t indexCount_;

	// 色の増減量
	Vector4 stepColor_;

	ModelData modelData_;
};
