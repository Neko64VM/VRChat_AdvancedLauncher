#pragma once
#include "../ImGui\imgui.h"
#include "../ImGui\imgui_impl_win32.h"
#include "../ImGui\imgui_impl_dx11.h"
#include "../Config/Config.h"
#include <Windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern bool                     g_SwapChainOccluded;
extern UINT                     g_ResizeWidth, g_ResizeHeight;
extern ID3D11RenderTargetView* g_mainRenderTargetView;

class AppWindow
{
private:
	WNDCLASSEXW wc{};
	HWND hwnd;
public:
	bool InitWindow();
	void WindowLoop();
	void DestroyAppWindow();
};