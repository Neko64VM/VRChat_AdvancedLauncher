#include "AdvancedLauncher\AdvancedLauncher.h"

AdvancedLauncher* launcher = new AdvancedLauncher();

// DEBUG時にはコンソールウィンドウを表示する
#if _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    AppWindow* wnd = new AppWindow();

    if (!launcher->Init())
        return 1;

    if (!wnd->InitWindow())
        return 2;

    wnd->WindowLoop();
    wnd->DestroyAppWindow();
    delete wnd, launcher;

    return 0;
}

void AppWindow::WindowLoop()
{
    // VRChat Style
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

    while (g.m_ApplicationActive)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
            Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
        ImGui::SetNextWindowSize(ImVec2(450.f - (style.WindowPadding.x * 2), 500.f - (style.WindowPadding.y * 2)));
        ImGui::Begin("VRChat - Advanced Launcher", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

        launcher->LauncherMenu();

        ImGui::End();

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 1.f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }
}