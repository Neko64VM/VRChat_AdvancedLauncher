#pragma once
#include "../ImGui\imgui.h"
#include "../ImGui\imgui_impl_win32.h"
#include "../ImGui\imgui_impl_dx11.h"
#include "../Config/Config.h"
#include <Windows.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class AppWindow
{
private:
	WNDCLASSEXW wc{};
	HWND hwnd;

	void UserFunction();
public:
	bool CreateAppWindow(const LPCWSTR m_szTitle, const LPCWSTR m_szClass);
	void WindowLoop();
	void DestroyAppWindow();
};