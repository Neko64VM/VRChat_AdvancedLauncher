#pragma once
#include "../Framework/AppWindow/AppWindow.h"
#include "../Framework/Config/Config.h"
#include <thread>

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
	std::string FindVRChatInstallationPath();
public:
	bool m_bConfigMode{ false };
	bool Init();
	void LauncherMenu();
	void ConfigMenu();
};