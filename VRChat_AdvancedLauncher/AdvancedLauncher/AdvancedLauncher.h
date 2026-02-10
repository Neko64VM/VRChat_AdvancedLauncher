#pragma once
#include "../Framework/AppWindow/AppWindow.h"
#include "../Framework/Config/Config.h"
#include <thread>
#include <regex>

class AdvancedLauncher
{
private:
	int m_iMonitorCount{ 0 };
	
	std::string m_szConfigPath{};
	std::string m_szVRChatConfigPath{};
	std::string m_szVRChatInstallationPath{};
	std::string m_szVRChatFullPath{};
	std::string m_szConfigFileName{ "config.json" };

	void ProcessThread();
	std::string BuildCommand();
	std::string FindVRChatPath();

	std::wstring ReadRegistryString(HKEY hKeyRoot, const wchar_t* subKey, const wchar_t* valueName)
	{
		DWORD buffer_size{ 0 };
		if (RegGetValueW(hKeyRoot, subKey, valueName, RRF_RT_REG_SZ, nullptr, nullptr, &buffer_size) != ERROR_SUCCESS)
			return std::wstring();

		std::vector<wchar_t> buffer(buffer_size / sizeof(wchar_t));
		if (RegGetValueW(hKeyRoot, subKey, valueName, RRF_RT_REG_SZ, nullptr, buffer.data(), &buffer_size) == ERROR_SUCCESS)
			return std::wstring(buffer.data());

		return std::wstring();
	}

	std::vector<std::string> FindSteamPath(const std::string& vdf_path)
	{
		std::vector<std::string> library_paths;
		std::ifstream f(vdf_path);

		if (!f.is_open())
			return library_paths;

		std::smatch match;
		std::regex path_regex(R"(\"path\"\s+\"(.+?)\")");
		

		while (f) {
			std::string line;
			std::getline(f, line);

			if (std::regex_search(line, match, path_regex)) {
				std::string path = match[1].str();
				std::replace(path.begin(), path.end(), '\\', '/');
				library_paths.push_back(path);
			}
		}

		f.close();

		return library_paths;
	}

public:
	bool m_bConfigMode{ false };
	bool Init();
	void LauncherMenu();
	void ConfigMenu();
};