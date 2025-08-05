#include "Config.h"
#include "../Utils/Utils.h"

Config g;
VRChatConfig vg;
ConfigManager config;

// デフォルトの設定
json ConfigManager::GetLauncherDefaultConfig()
{
    return json{
            { "AvatarTest", false },
            { "CCXEnable", false },
            { "CCXOption", 2 },
            { "DesktopMode", false },
            { "FullScreen", false },
            { "MaxFPS", 144 },
            { "MaxFPSEnable", true },
            { "Monitor", 0 },
            { "Profile", std::vector<std::string>{ "Main", "Sub", "None" }},
            { "ProfileID", 0 },
            { "VRChatPath", "" },
            { "WorldTest", false },
            { "WindowSize", 0 }
    };
}

json ConfigManager::GetVRChatDefaultConfig()
{
    return json{
          { "cache_size", 8 },
          { "cache_expiry_delay", 3 },
          { "cache_directory", "" },
          { "picture_output_folder", "" },
    };
}

void ConfigManager::LoadVRChatSetting(const std::string path)
{
    std::ifstream file(path);
    if (file.good()) {
        json JSON;
        file >> JSON;

        vg.cache_size = JSON["cache_size"];
        vg.cache_expiry_delay = JSON["cache_expiry_delay"];

        std::string cache_path = JSON["cache_directory"];
        std::string picture_path = JSON["picture_output_folder"];

        strcpy_s(vg.cache_directory, 128, cache_path.c_str());
        strcpy_s(vg.picture_output_folder, 128, picture_path.c_str());
    }

    std::cout << "[ LOG ] Load json - ConfigManager::LoadVRChatSetting();" << std::endl;

    file.close();
}

void ConfigManager::WriteVRChatSetting(const std::string path)
{
    std::ifstream file(path);

    if (file.good()) {
        json JSON;
        file >> JSON;

        JSON["cache_size"] = vg.cache_size;
        JSON["cache_expiry_delay"] = vg.cache_expiry_delay;
        JSON["cache_directory"] = vg.cache_directory;
        JSON["picture_output_folder"] = vg.picture_output_folder;

        std::ofstream outputFile(path, std::ios::trunc);

        if (outputFile.good()) {
            outputFile << JSON.dump(4);
            std::cout << "[ LOG ] json updated - ConfigManager::SaveVRChatSetting()" << std::endl;
        }
        else {
            std::cout << "[ LOG ] failed to update json - ConfigManager::SaveVRChatSetting()" << std::endl;
        }
    }

    file.close();
}

void ConfigManager::LoadLauncherSetting(const std::string path, const std::string filename)
{
    std::string configPath = path + filename;
    std::ifstream file(configPath);
    if (file.good()) {
        json JSON;
        file >> JSON;

        g.m_bAvatarTest = JSON["AvatarTest"];
        g.m_bCCX_Enable = JSON["CCXEnable"];
        g.m_iCCX_Option = JSON["CCXOption"];
        g.m_bDesktopMode = JSON["DesktopMode"];
        g.m_bFullScreen = JSON["FullScreen"];
        g.m_iMaxFPS = JSON["MaxFPS"];
        g.m_bMaxFPSEnable = JSON["MaxFPSEnable"];
        g.m_iMonitor = JSON["Monitor"];
        g.m_vecProfileList = JSON["Profile"].get<std::vector<std::string>>(); // std::vector<std::string>>として読み取る
        g.m_iProfileID = JSON["ProfileID"];
        g.m_bWorldTest = JSON["WorldTest"];
        g.m_iWindowSize = JSON["WindowSize"];
    }

    std::cout << "[ LOG ] Load json - ConfigManager::LoadSetting()" << std::endl;

    file.close();
}

void ConfigManager::SaveLauncherSetting(const std::string path, const std::string filename)
{
    std::string configPath = path + filename;
    std::ifstream file(configPath);

    if (file.good()) {
        json JSON;
        file >> JSON;

        JSON["AvatarTest"] = g.m_bAvatarTest;
        JSON["CCXEnable"] = g.m_bCCX_Enable;
        JSON["CCXOption"] = g.m_iCCX_Option;
        JSON["DesktopMode"] = g.m_bDesktopMode;
        JSON["FullScreen"] = g.m_bFullScreen;
        JSON["MaxFPS"] = g.m_iMaxFPS;
        JSON["MaxFPSEnable"] = g.m_bMaxFPSEnable;
        JSON["Monitor"] = g.m_iMonitor;
        JSON["Profile"] = g.m_vecProfileList;
        JSON["ProfileID"] = g.m_iProfileID;
        JSON["WorldTest"] = g.m_bWorldTest;
        JSON["WindowSize"] = g.m_iWindowSize;

        std::ofstream outputFile(configPath, std::ios::trunc);

        if (outputFile.good()) {
            outputFile << JSON.dump(4);
            std::cout << "[ LOG ] json updated - ConfigManager::SaveSetting()" << std::endl;
        }
        else {
            std::cout << "[ LOG ] failed to update json - ConfigManager::SaveSetting()" << std::endl;
        }
    }

    file.close();
}

std::string ConfigManager::ReadInstallPath(const std::string& appdata_local, const std::string& config_name)
{
    std::string configFile = appdata_local + config_name;

    std::ifstream f(configFile);

    if (f.good()) {
        json j;
        f >> j;

        std::string p = j["VRChatPath"];
        f.close();
        
        return p;
    }
    else {
        return std::string();
    }
}

void ConfigManager::WriteInstallPath(const std::string& appdata_local, const std::string& config_name, const std::string& vrc_path)
{
    std::string path = appdata_local + config_name;

    std::ifstream f(path);

    if (f.good()) {
        json j;
        f >> j;

        j["VRChatPath"] = vrc_path;

        std::ofstream outputFile(path, std::ios::trunc);
        if (outputFile.good()) {
            outputFile << j.dump(4);
            std::cout << "[ LOG ] json updated - ConfigManager::WriteInstallPath()" << std::endl;
        }
        else {
            std::cout << "[ LOG ] failed to update json - ConfigManager::WriteInstallPath()" << std::endl;
        }
    }

    f.close();
}