#pragma once

struct IDXGISwapChain;
struct ID3D11ShaderResourceView;

namespace gui {
	void init(IDXGISwapChain* pSwapchain);

	void init_style();

	void render();

	bool load_image();

	ID3D11ShaderResourceView* get_image(void);
	
	inline bool show_menu = false;
	inline int pImageWidth = 0;
	inline int pImageHeight = 0;
	inline void* pDetectedTexture = nullptr;

	inline bool is_detected = false;
}