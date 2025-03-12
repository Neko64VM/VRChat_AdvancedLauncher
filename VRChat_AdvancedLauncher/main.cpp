#include "AdvancedLauncher\AdvancedLauncher.h"

auto window = std::make_unique<AppWindow>();
auto launcher = std::make_unique<AdvancedLauncher>();

// DEBUG時にはコンソールウィンドウを表示する
#if _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    if (!launcher->Init())
        return 1;

	// タイトル, クラス名
    if (!window->CreateAppWindow(L"VRChat Advanced Launcher", L"Launcher"))
        return 2;

	// ImGuiのスタイルを設定VRChat Style
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 4.f;
	style.ScrollbarRounding = 12.f;
	style.GrabRounding = 4.f;
	style.WindowBorderSize = 0.f;
	style.FrameBorderSize = 1.f;

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.12f, 0.25f, 0.28f, 0.50f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.09f, 0.19f, 0.21f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.17f, 0.27f, 0.29f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.58f, 0.64f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.58f, 0.64f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.17f, 0.67f, 0.76f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.17f, 0.67f, 0.76f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.17f, 0.67f, 0.76f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.09f, 0.19f, 0.21f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.09f, 0.58f, 0.64f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.09f, 0.19f, 0.21f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.13f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.58f, 0.64f, 1.00f);

    window->WindowLoop();
    window->DestroyAppWindow();

    return 0;
}

void AppWindow::UserFunction()
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(450.f - (style.WindowPadding.x * 2), 500.f - (style.WindowPadding.y * 2)));
    ImGui::Begin("VRChat - Advanced Launcher", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    launcher->LauncherMenu();

    ImGui::End();
}