#include <d3d11_hook.h>
#include <intrin.h>
#include <ue4.h>
#include <defines.h>
#include <imgui/imgui.h>
#include <Windows.h>
#include <gui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <lazy_importer.hpp>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void*** d3d11_hook::get_swapchain()
{
	unsigned long long engine = engine::GetEngine();
	if (!engine)
	{
		LOG("Engine not found");
		return nullptr;
	}

	unsigned long long game_viewport = *reinterpret_cast<unsigned long long*>(engine + offsets::GameViewport);
	if (!game_viewport)
	{
		LOG("GameViewport not found");
		return nullptr;
	}

	LOG("GameViewport: 0x{:X}", game_viewport);

	unsigned long long viewport = *reinterpret_cast<unsigned long long*>(game_viewport + offsets::Viewport);
	if (!viewport)
	{
		LOG("Viewport not found");
		return nullptr;
	}

	LOG("Viewport: 0x{:X}", viewport);

	unsigned long long viewport_rhi = *reinterpret_cast<unsigned long long*>(viewport + offsets::ViewportRHI);
	if (!viewport_rhi)
	{
		LOG("ViewportRHI not found");
		return nullptr;
	}

	LOG("ViewportRHI: 0x{:X}", viewport_rhi);

	void*** swapchain = *reinterpret_cast<void****>(viewport_rhi + offsets::Swapchain);
	LOG("Swapchain: 0x{:X}", (unsigned long long)swapchain);

	return swapchain;
}

LRESULT __stdcall d3d11_hook::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (!gui::is_detected) {
		if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
		{
			gui::show_menu = !gui::show_menu;
			ImGui::GetIO().MouseDrawCursor = gui::show_menu;
		}
	}

	if (gui::show_menu || gui::is_detected)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(oWndProc, (HWND)hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall d3d11_hook::Present(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags)
{
	static bool init = false;

	if (!init) {
		gui::init(pSwapChain);
		init = true;
	}
	else {
		ID3D11Texture2D* renderTargetTexture = nullptr;
		if (!d3d11_hook::mainRenderTargetView)
		{
			if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&renderTargetTexture))))
			{
				d3d11_hook::pDevice->CreateRenderTargetView(renderTargetTexture, nullptr, &d3d11_hook::mainRenderTargetView);
				renderTargetTexture->Release();
			}
		}
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (gui::show_menu || gui::is_detected)
	{
	//	ImGui::ShowStyleEditor();
		gui::render();
	}

	ImGui::Render();
	d3d11_hook::pContext->OMSetRenderTargets(1, &d3d11_hook::mainRenderTargetView, NULL);

	if (d3d11_hook::mainRenderTargetView)
	{
		d3d11_hook::mainRenderTargetView->Release();
		d3d11_hook::mainRenderTargetView = nullptr;
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return reinterpret_cast<HRESULT(__stdcall*)(void*, UINT, UINT)>(PresentOriginal)(pSwapChain, SyncInterval, Flags);

}

bool vmt_hook::hook(void** vtable, int index, void* fun, void** original)
{
	DWORD old_protect;
	if (LI_FN(VirtualProtect)(&vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protect))
	{
		*original = vtable[index];
		vtable[index] = fun;
		LI_FN(VirtualProtect)(&vtable[index], sizeof(void*), old_protect, &old_protect);

		return true;
	}

	return false;
}