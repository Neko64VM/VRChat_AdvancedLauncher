#include "AdvancedLauncher.h"
#include "../Utils/Utils.h"
#include <thread>

ImColor TitleTextCol = ImColor(0.17f, 0.67f, 0.76f, 0.65f);
const char* coreList[] = { "3 [ Ryzen5  (3Core * 2CCX)]", "4 [ Ryzen7  (4Core * 2CCX)]", "6 [ Ryzen9  (6Core * 2CCX)]", "8 [ Ryzen9+ (8Core * 2CCX)]" };
const char* resolutionList[] = { "Display size", "480 * 270", "1280 * 720", "1920 * 1080" };

void AdvancedLauncher::LauncherMenu()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.SeparatorTextBorderSize = 1.f;

    // Profile selector
    const char** profileList = new const char* [g.g_profileList.size()];

    for (size_t i = 0; i < g.g_profileList.size(); ++i) {
        profileList[i] = g.g_profileList[i].c_str();
    }

    // モニター数をメニューで処理できるようにする
    const char** monitorList = new const char* [m_iMonitorCount];

    for (int j = 0; j < m_iMonitorCount; j++) {
        std::string itemStr = "Monitor " + std::to_string(j + 1);
        char* itemCopy = new char[itemStr.size() + 1];
        strcpy_s(itemCopy, itemStr.size() + 1, itemStr.c_str());
        monitorList[j] = itemCopy;
    }

    // Create ImGui window
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(450.f - (style.WindowPadding.x * 2), 500.f - (style.WindowPadding.y * 2)));
    ImGui::Begin("VRChat - Advanced Launcher", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

    // MenuBar. 設定ファイル関連.
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Config"))
        {
            if (ImGui::MenuItem("Open config folder")) {
                ShellExecute(NULL, "open", "explorer", m_szConfigPath.c_str(), NULL, SW_SHOW);
            }
            if (ImGui::MenuItem("Edit config.json")) {
                std::string path = m_szConfigPath + m_szConfigFileName;
                ShellExecute(NULL, "open", "notepad.exe", path.c_str(), NULL, SW_SHOW);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::TextColored(TitleTextCol, "Display");
    ImGui::Separator();
    ImGui::Checkbox("DesktopMode", &g.g_DesktopMode); // --no-vr
    ImGui::Checkbox("FullScreen", &g.g_FullScreen); // -screen-fullscreen 0/1
    ImGui::Checkbox("FPS Limit", &g.g_MaxFPSEnable);
    ImGui::SliderInt("Max FPS", &g.g_MaxFPS, 10, 240); // --fps=**
    ImGui::Combo("Monitor", &g.g_Monitor, monitorList, m_iMonitorCount); // -monitor
    ImGui::Combo("WindowSize", &g.g_WindowSize, resolutionList, IM_ARRAYSIZE(resolutionList));

    ImGui::NewLine();

    ImGui::TextColored(TitleTextCol, "Test");
    ImGui::Separator();
    ImGui::Checkbox("Avater Test", &g.g_AvatarTest); // --watch-avatars
    ImGui::Checkbox("World Test", &g.g_WorldTest); // --watch-world

    ImGui::NewLine();

    ImGui::TextColored(TitleTextCol, "Misc");
    ImGui::Separator();
    ImGui::Combo("Profile", &g.g_ProfileID, profileList, g.g_profileList.size()); // --profile=?

    ImGui::Checkbox("CCX Option -", &g.g_CCX_Enable); ImGui::SameLine();
    ImGui::TextColored(ImColor(1.f, 0.f, 0.f, 1.f), "Some Ryzen 1/2/3/5/7000 user only");

    if (g.g_CCX_Enable)
        ImGui::Combo("Core per CCX", &g.g_CCX_Option, coreList, IM_ARRAYSIZE(coreList));

    // Launch
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.f - (style.WindowPadding.y * 4));
    if (ImGui::Button("Launch", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {

        if (utils::process::IsProcessRunning("VRChat.exe"))
        {
            int result = MessageBox(NULL, "既に他のVRChatインスタンスが存在します。\n本当に実行しますか？", "確認", MB_YESNO | MB_TOPMOST | MB_ICONWARNING);

            if (result == IDYES)
                std::thread([&]() { ProcessThread(); }).detach();
        }
        else {
            std::thread([&]() { ProcessThread(); }).detach();
        }
    }

    ImGui::End();
    
    // CleanUp
    for (int k = 0; k < m_iMonitorCount; k++)
        delete[] monitorList[k];

    delete[] monitorList;
}