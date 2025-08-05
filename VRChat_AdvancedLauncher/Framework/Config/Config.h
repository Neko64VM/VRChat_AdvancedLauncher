#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// JSON Lib : https://github.com/nlohmann/json

// System/Global
static std::atomic<bool> bApplicationActive(true);
static bool bIsSteamVRRunning{ false };

struct Config
{
    bool m_bDesktopMode{ false };
    bool m_bFullScreen{ false };
    bool m_bMaxFPSEnable{ true };
    int  m_iWindowSize{ 0 };
    int  m_iMaxFPS{ 144 };
    int  m_iMonitor{ 0 };
    bool m_bAvatarTest{ false };
    bool m_bWorldTest{ false };
    int  m_iProfileID{ 0 };
    bool m_bCCX_Enable{ false };
    int  m_iCCX_Option{ 0 };
    std::vector<std::string> m_vecProfileList{};
};

struct VRChatConfig
{
    int cache_size{ 8 };
    int cache_expiry_delay{ 3 };
    char cache_directory[128]{};
    char picture_output_folder[128]{};
};

class ConfigManager
{
public:
    json GetLauncherDefaultConfig();
    json GetVRChatDefaultConfig();
    void LoadVRChatSetting(const std::string path);
    void WriteVRChatSetting(const std::string path);
    void LoadLauncherSetting(const std::string path, const std::string filename);
    void SaveLauncherSetting(const std::string path, const std::string filename);
    std::string ReadInstallPath(const std::string& appdata_local, const std::string& config_name);
    void WriteInstallPath(const std::string& appdata_local, const std::string& config_name, const std::string& vrc_path);
};

extern Config g;
extern VRChatConfig vg;
extern ConfigManager config;