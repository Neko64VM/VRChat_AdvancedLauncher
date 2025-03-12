#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct Config
{
    // System
    bool m_ApplicationActive = true;

    // Launcher
    bool g_DesktopMode = false;
    bool g_FullScreen  = false;
    bool g_MaxFPSEnable = true;
    int  g_WindowSize = 0;
    int  g_MaxFPS = 144;
    int  g_Monitor = 0;
    bool g_WorldTest = false;
    bool g_AvatarTest = false;
    int  g_ProfileID = 0;
    bool g_CCX_Enable = false;
    int  g_CCX_Option = 0;
};

// https://github.com/nlohmann/json
class ConfigManager
{
public:
    void LoadSetting(const std::string path, const std::string filename);
    void SaveSetting(const std::string path, const std::string filename);
    std::string ReadInstallPath(const std::string& appdata_local, const std::string& config_name);
    void WriteInstallPath(const std::string& appdata_local, const std::string& config_name, const std::string& vrc_path);
};

extern Config g;
extern ConfigManager cfg;