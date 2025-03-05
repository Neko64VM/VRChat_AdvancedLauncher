#pragma once
#include "../Framework/AppWindow/AppWindow.h"
#include "../Framework/Config/Config.h"
#include <thread>

class AdvancedLauncher
{
private:
	int m_iMonitorCount;
	
	std::string m_szConfigPath;
	std::string m_szVRChatConfigPath;
	std::string m_szVRChatIInstallationPath;
	std::string m_szConfigFileName = "config.json";

	void ProcessThread();
	std::string BuildCommand();
	std::string FindVRChatInstallationPath();
public:
	bool Init();
	void LauncherMenu();
};