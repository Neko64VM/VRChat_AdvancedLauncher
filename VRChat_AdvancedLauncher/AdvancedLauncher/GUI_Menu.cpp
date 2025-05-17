#include "AdvancedLauncher.h"
#include "../Utils/Utils.h"
#include <thread>

ImColor TitleTextCol{ ImColor(0.17f, 0.67f, 0.76f, 0.65f) };
const char* coreList[]{ "3 [ Ryzen5  (3Core * 2CCX)]", "4 [ Ryzen7  (4Core * 2CCX)]", "6 [ Ryzen9  (6Core * 2CCX)]", "8 [ Ryzen9+ (8Core * 2CCX)]" };
const char* resolutionList[]{ "Display size", "480 * 270", "1280 * 720", "1920 * 1080" };

void AdvancedLauncher::ConfigMenu()
{
    ImGuiStyle& style = ImGui::GetStyle();
    // Create ImGui window
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(450.f - (style.WindowPadding.x * 2), 500.f - (style.WindowPadding.y * 2)));
    ImGui::Begin("ConfigMode - config.json", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

    // Picture
    ImGui::TextColored(TitleTextCol, "Picture Path");
    ImGui::Separator();
    ImGui::InputText("Path##Picture", vg.picture_output_folder, IM_ARRAYSIZE(vg.picture_output_folder)); // picture_output_folder

    if (ImGui::Button("Select Folder##Picture")) {
        utils::file::SelectDirectoryPath("VRChat Picture Path", vg.picture_output_folder, 128);
    }

    ImGui::NewLine();

    // Cache
    ImGui::TextColored(TitleTextCol, "Cache");
    ImGui::Separator();
    ImGui::SliderInt("MaxSize (GB)", &vg.cache_size, 1, 256); // cache_size
    ImGui::SliderInt("ExpiryDelay (Day)", &vg.cache_expiry_delay, 1, 365); // cache_expiry_delay
    ImGui::InputText("Path##Directory", vg.cache_directory, IM_ARRAYSIZE(vg.cache_directory)); // cache_directory
    if (ImGui::Button("Select Folder##CacheDir")) {
        utils::file::SelectDirectoryPath("VRChat Cache Path", vg.cache_directory, 128);
    }

    // Launch
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.f - (style.WindowPadding.y * 4));

    if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - style.WindowPadding.x, 30.f))) {
        //JSON  Write
        std::string path = m_szVRChatConfigPath + "config.json";
        config.WriteVRChatSetting(path);

        m_bConfigMode = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 30.f))) {
        m_bConfigMode = false;
    }

    ImGui::End();
}

void AdvancedLauncher::LauncherMenu()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.SeparatorTextBorderSize = 1.f;

    // Profile selector
    const char** profileList = new const char* [g.m_vecProfileList.size()];

    for (size_t i = 0; i < g.m_vecProfileList.size(); ++i) {
        profileList[i] = g.m_vecProfileList[i].c_str();
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
        if (ImGui::BeginMenu("VRChat"))
        {
            if (ImGui::MenuItem("Open log file path")) {
                ShellExecute(NULL, "open", "explorer", m_szVRChatConfigPath.c_str(), NULL, SW_SHOW);
            }
            if (ImGui::MenuItem("Configure config.json")) {
                m_bConfigMode = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("LauncherConfig"))
        {
            if (ImGui::MenuItem("Open config path")) {
                ShellExecute(NULL, "open", "explorer", m_szConfigPath.c_str(), NULL, SW_SHOW);
            }
            if (ImGui::MenuItem("Edit config")) {
                std::string path = m_szConfigPath + m_szConfigFileName;
                ShellExecute(NULL, "open", "notepad", path.c_str(), NULL, SW_SHOW);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::TextColored(TitleTextCol, "Display");
    ImGui::Separator();

    // --no-vr
    if (ImGui::RadioButton("VR Mode", g.m_bDesktopMode == false))
        g.m_bDesktopMode = false;

    ImGui::SameLine();

    if (ImGui::RadioButton("Desktop Mode", g.m_bDesktopMode == true))
        g.m_bDesktopMode = true;

    //ImGui::Checkbox("DesktopMode", &g.g_DesktopMode); // --no-vr
    ImGui::Checkbox("FullScreen", &g.m_bFullScreen); // -screen-fullscreen 0/1
    ImGui::Checkbox("FPS Limit", &g.m_bMaxFPSEnable);
    ImGui::SliderInt("Max FPS", &g.m_iMaxFPS, 10, 240); // --fps=**
    ImGui::Combo("Monitor", &g.m_iMonitor, monitorList, m_iMonitorCount); // -monitor
    ImGui::Combo("WindowSize", &g.m_iWindowSize, resolutionList, IM_ARRAYSIZE(resolutionList));

    ImGui::NewLine();

    ImGui::TextColored(TitleTextCol, "Test");
    ImGui::Separator();
    ImGui::Checkbox("Avater Test", &g.m_bAvatarTest); // --watch-avatars
    ImGui::Checkbox("World Test", &g.m_bWorldTest); // --watch-world

    ImGui::NewLine();

    ImGui::TextColored(TitleTextCol, "Misc");
    ImGui::Separator();
    ImGui::Combo("Profile", &g.m_iProfileID, profileList, g.m_vecProfileList.size()); // --profile=?

    ImGui::Checkbox("CCX Option -", &g.m_bCCX_Enable); ImGui::SameLine();
    ImGui::TextColored(ImColor(1.f, 0.f, 0.f, 1.f), "Some Ryzen 1/2/3/5/7000 user only");

    if (g.m_bCCX_Enable)
        ImGui::Combo("Core per CCX", &g.m_iCCX_Option, coreList, IM_ARRAYSIZE(coreList));

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