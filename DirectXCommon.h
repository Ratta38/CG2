#pragma once
class WinApp; // 前方宣言
#include "WinApp.h"
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <string>
#include <wrl.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>

class DirectXCommon {
public:
	DirectXCommon();
	~DirectXCommon();

	void Initialize(std::shared_ptr<WinApp> winApp);
	void BeginFrame();
	void EndFrame();
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device_; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() const { return srvDescriptorHeap_; }
	std::shared_ptr<WinApp> GetWinApp() const { return winApp_; }
	UINT GetSwapChainDescBufferCount() const { return swapChainDesc_.BufferCount; }
	DXGI_FORMAT GetRtvFormat() const { return rtvDesc_.Format; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList_; }
	HANDLE GetFenceEvent() const { return fenceEvent_; }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPipelineState() { return graphicsPipelineState_; }
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return rootSignature_; }

private:
	void InitializeDXGIDevice();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateDescriptorHeaps();
	void CreateRTV();
	void InitializeShaderCompiler();
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void CreateViewport();
	void CreateScissor();
	void CreateFence();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescripterHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// 文字列を表示する関数
	/// </summary>
	/// <param name="message">文字列</param>
	void Log(const std::string& message);

	IDxcBlob* CompileShader(
	    // CompilerするShaderファイルへのパス
	    const std::wstring& filePath,
	    // Compilerに使用するProfile
	    const wchar_t* profile,
	    // 初期化で生成したものを3つ
	    IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

private:

	std::shared_ptr<WinApp> winApp_;

	// DXGI
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;

	// D3D12 Core
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	// Descriptor Heaps
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	// SwapChain Resources
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2] = {};

	// Depth Buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

	// Shader Compiler
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	// Root Signature / Pipeline
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	// Viewport / Scissor
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	// Sync
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_ = nullptr;

	UINT backBufferIndex_ = 0;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;

	// Debug
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_;

#ifdef _DEBUG
	void SetupDebugLayer();
#endif
};
