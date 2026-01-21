#pragma once

#include <Windows.h>
#include <thread>
#include <d3d11.h>

//this is required because we are using a vmt hook.
namespace offsets {
	constexpr unsigned long long GameViewport = 0x07C0; //in sdk
	constexpr unsigned long long Viewport = 0xb8;
	constexpr unsigned long long ViewportRHI = 0xb0;
	constexpr unsigned long long Swapchain = 0x68;
}

namespace d3d11_hook {
	void*** get_swapchain();

	HRESULT __stdcall Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	LRESULT  __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	inline void* PresentOriginal;
	inline std::once_flag init_flag;
	inline HWND window = NULL;
	inline WNDPROC oWndProc;
	inline ID3D11Device* pDevice = NULL;
	inline ID3D11DeviceContext* pContext = NULL;
	inline ID3D11RenderTargetView* mainRenderTargetView;
}

namespace vmt_hook {
	bool hook(void** vtable, int index, void* fun, void** original);
}