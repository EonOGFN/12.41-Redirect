#define  _CRT_SECURE_NO_WARNINGS

#include <gui.h>
#include <d3d11_hook.h>

#include <d3d11.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include <settings.h>
#include <lazy_importer.hpp>

#include <font.h>
#include <image.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <defines.h>

void gui::init(IDXGISwapChain* pSwapChain)
{
	if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&d3d11_hook::pDevice)))
	{
		d3d11_hook::pDevice->GetImmediateContext(&d3d11_hook::pContext);
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		d3d11_hook::window = sd.OutputWindow;
		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		d3d11_hook::pDevice->CreateRenderTargetView(pBackBuffer, NULL, &d3d11_hook::mainRenderTargetView);
		pBackBuffer->Release();
		d3d11_hook::oWndProc = (WNDPROC)SetWindowLongPtr(d3d11_hook::window, GWLP_WNDPROC, (LONG_PTR)d3d11_hook::WndProc);
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
		ImGui_ImplWin32_Init(d3d11_hook::window);
		ImGui_ImplDX11_Init(d3d11_hook::pDevice, d3d11_hook::pContext);

		ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)SF_Pro_Display_Black, sizeof(SF_Pro_Display_Black), 14.0f);
		gui::init_style();

		gui::pDetectedTexture = nullptr;
	}
}

void gui::render()
{
	if (!show_menu && !is_detected)
		return;  

	RECT rect;
	LI_FN(GetWindowRect)(d3d11_hook::window, &rect);

	float width = (float)(rect.right - rect.left);
	float height = (float)(rect.bottom - rect.top);

	if (is_detected) {
		
		ImGui::SetNextWindowPos({ width / 2, height / 2 }, ImGuiCond_Always, { 0.5f, 0.5f });
		ImGui::SetNextWindowSize({ gui::pImageWidth + 10.f,  gui::pImageHeight + 10.f }, ImGuiCond_Always);
		ImGui::GetBackgroundDrawList()->AddRectFilled({ 0, 0 }, { width, height }, IM_COL32(0x33, 0x33, 0x33, 255));
		
		ImGui::Begin(xorstr_("##imageviewer"), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);

		if (ID3D11ShaderResourceView* image = gui::get_image())
		{
			ImGui::Image((ImTextureID)(intptr_t)image, { (float)gui::pImageWidth, (float)gui::pImageHeight });
		}
		else {
			ImGui::Text(xorstr_("monkey prime frere"));
		}


		return;
	}



	ImGui::GetBackgroundDrawList()->AddRectFilled({ 0, 0 }, { width, height }, IM_COL32(0x33, 0x33, 0x33, 0x85));

	ImGui::SetNextWindowPos({ width / 2, height / 2 }, ImGuiCond_Always, { 0.5f, 0.5f });
	ImGui::SetNextWindowSize({ 400.f, 300.f }, ImGuiCond_Appearing);
	ImGui::Begin(xorstr_("EON MENU (Insert to Close)"), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Checkbox(xorstr_("Disable Pre-edit"), &settings::b_use_disable_preedit);
	ImGui::Checkbox(xorstr_("Reset on Release"), &settings::b_use_reset_on_release);
	ImGui::Checkbox(xorstr_("Use Custom FOV"), &settings::b_use_custom_fov);
	if (settings::b_use_custom_fov){
		ImGui::Text(xorstr_("Custom Field of View (FOV)"));
		ImGui::SliderFloat(xorstr_("##customfov"), &settings::fov, 60.0f, 120.0f);
	}

	ImGui::End();

}

bool gui::load_image()
{
	int image_width = 0;
	int image_height = 0;


	unsigned char* image_data = stbi_load_from_memory((const unsigned char*)_detected_image, (int)sizeof(_detected_image), &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
	{
		LOG("Failed to load image data");
		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, (sizeof(desc)));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	d3d11_hook::pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	d3d11_hook::pDevice->CreateShaderResourceView(pTexture, &srvDesc, (ID3D11ShaderResourceView**)&pDetectedTexture);
	pTexture->Release();

	pImageWidth = image_width;
	pImageHeight = image_height;

	LOG("Image Width : %d\n", pImageWidth);
	LOG("Image Height : %d\n", pImageHeight);

	stbi_image_free(image_data);

	return true;
}

ID3D11ShaderResourceView* gui::get_image(void)
{
	static bool tried_to_load_image = false;

	if (!gui::pDetectedTexture && !tried_to_load_image) {
		tried_to_load_image = true;
		gui::load_image();
	}

	return (ID3D11ShaderResourceView*)gui::pDetectedTexture;
}

void gui::init_style()
{
	//cba to make a custom style because its broken
}