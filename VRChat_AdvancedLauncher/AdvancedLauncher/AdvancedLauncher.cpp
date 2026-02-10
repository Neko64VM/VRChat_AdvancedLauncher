#include "AdvancedLauncher.h"
#include "../Utils/Utils.h"
#include "../Framework/ImGui/Font/NotoSansMed.h"

// モニター数を取得
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	int* count = reinterpret_cast<int*>(dwData);
	(*count)++;
	return TRUE;
}

bool AdvancedLauncher::Init()
{
	// 各種Pathを取得 (AppData)
	m_szConfigPath = utils::file::GetAppDataPath(FOLDERID_LocalAppData) + "\\VRChatAdvancedLauncher\\";
	m_szVRChatConfigPath = utils::file::GetAppDataPath(FOLDERID_LocalAppDataLow) + "\\VRChat\\VRChat\\";

	// ドライブのルートの文字列が含まれていなかったら / Config用のディレクトリが存在しなかったら
	if (m_szConfigPath.find(":\\") == std::string::npos || m_szVRChatConfigPath.find(":\\") == std::string::npos)
		return false;
	else if (!utils::file::IsExistsDirectory(m_szConfigPath))
		std::filesystem::create_directory(m_szConfigPath);

	std::string configFilePath = m_szConfigPath + m_szConfigFileName;

	// jsonがない場合はファイルを作成
	if (!utils::file::IsExistsFile(configFilePath)) {
		std::ofstream fFile(configFilePath);
		fFile.close();

		// jsonに書き込む
		std::ifstream in(configFilePath);
		if (!in || in.peek() == std::ifstream::traits_type::eof())
		{
			std::cout << "[ LOG ] create and write json file." << std::endl;
			std::ofstream out(configFilePath);
			out << config.GetLauncherDefaultConfig().dump(4);
			out.close();
		}
	}

	// jsonからVRChatのインストール先を読み取る
	m_szVRChatInstallationPath = config.ReadInstallPath(m_szConfigPath, m_szConfigFileName);

	// 初回かインストール先が変更された場合
	if (m_szVRChatInstallationPath.size() == 0 || !utils::file::IsExistsDirectory(m_szVRChatInstallationPath) || !utils::file::DoesFileExistInDirectory(m_szVRChatInstallationPath, "VRChat.exe"))
	{
		// VRChat自体のインストール先を取得
		m_szVRChatInstallationPath = FindVRChatPath();

		if (m_szVRChatInstallationPath.size() == 0) {
			MessageBox(nullptr, "VRChatのインストール先が見つかりませんでした。", "ERROR", MB_TOPMOST | MB_OK | MB_ICONERROR);
		}	
		else {
			config.WriteInstallPath(m_szConfigPath, m_szConfigFileName, m_szVRChatInstallationPath); // jsonに保存
		}
	}

	m_szVRChatFullPath = m_szVRChatInstallationPath + "\\launch.exe";

	// モニターの数を取得
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&m_iMonitorCount));

	// config.jsonから設定をロード
	config.LoadLauncherSetting(m_szConfigPath, m_szConfigFileName);

	// ToDo.
	if (bIsSteamVRRunning)
	{
		g.m_bDesktopMode = false;
	}

	// jsonがない場合はファイルを作成 #2
	std::string config_path = m_szVRChatConfigPath + "config.json";

	if (!utils::file::IsExistsFile(config_path)) {
		std::ofstream fFile(config_path);
		fFile.close();

		// jsonに書き込む
		std::ifstream in(config_path);
		if (!in || in.peek() == std::ifstream::traits_type::eof())
		{
			std::cout << "[ LOG ] create and write json file." << std::endl;
			std::ofstream out(config_path);
			out << config.GetVRChatDefaultConfig().dump(4);
		}
	}

	config.LoadVRChatSetting(config_path);

	return true;
}

void AdvancedLauncher::ProcessThread()
{
	bool result = utils::process::StartProcess(m_szVRChatFullPath, BuildCommand());

	if (result)
		config.SaveLauncherSetting(m_szConfigPath, m_szConfigFileName);
	else 
		MessageBox(nullptr, "VRChatの起動に失敗しました…。", "ERROR", MB_OK | MB_TOPMOST | MB_ICONERROR);
}

std::string AdvancedLauncher::FindVRChatPath()
{
	// レジストリを読みSteamのインストール先を見つける
	// HKEY_LOCAL_MACHINE\SOFTWARE\Valve\Steam (32-bit)
	// HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Valve\Steam (64-bit)
	HKEY hKey{};
	std::string subKey{ "SOFTWARE\\WOW6432Node\\Valve\\Steam" };
	std::string value{ "InstallPath" };
	std::wstring reg_value = ReadRegistryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", L"InstallPath");
	std::string steam_path = utils::ConvertWideToMultiByte(reg_value);

	if (steam_path.size() == 0)
		return std::string();

	steam_path += "\\steamapps\\libraryfolders.vdf"; // Steamのlibraryfolders.vdfを読み、VRChatがインストールされている可能性のあるパスを取得する

	std::vector<std::string> library_paths = FindSteamPath(steam_path);

	for (const std::string& path : library_paths) {
		std::string potential_path = path + "//steamapps//common//VRChat";
		if (utils::file::IsExistsDirectory(potential_path) && utils::file::DoesFileExistInDirectory(potential_path, "VRChat.exe"))
			return potential_path;
	}

	return std::string();
}

std::string AdvancedLauncher::BuildCommand()
{
	std::ostringstream vOut;

	vOut << " \"vrchat://launch/";

	if (g.m_bDesktopMode) vOut << " --no-vr";

	vOut << " -screen-fullscreen " + std::to_string((int)g.m_bFullScreen);

	std::string width  = " -screen-width ";
	std::string height = " -screen-height ";

	switch (g.m_iWindowSize)
	{
	case 0:
		width  += std::to_string((int)GetSystemMetrics(SM_CXSCREEN));
		height += std::to_string((int)GetSystemMetrics(SM_CYSCREEN));
		break;
	case 1:
		width += std::to_string((int)480);
		height += std::to_string((int)270);
		break;
	case 2:
		width += std::to_string((int)1280);
		height += std::to_string((int)720);
		break;
	case 3:
		width += std::to_string((int)1920);
		height += std::to_string((int)1080);
		break;
	default:
		break;
	}
	vOut << width;
	vOut << height;

	std::string monitor_str = " -monitor " + std::to_string((int)g.m_iMonitor + 1);
	vOut << monitor_str;

	if (g.m_bMaxFPSEnable) {
		std::string framerate_str = " --fps=" + std::to_string((int)g.m_iMaxFPS);
		vOut << framerate_str;
	}

	if (g.m_bAvatarTest) vOut << " --watch-avatars";
	if (g.m_bWorldTest) vOut << " --watch-world";

	std::string profile_str = " --profile=" + std::to_string((int)g.m_iProfileID);
	vOut << profile_str;

	if (g.m_bCCX_Enable) {
		switch (g.m_iCCX_Option)
		{
		case 0:
			vOut << " --affinity=3F";
			break;
		case 1:
			vOut << " --affinity=FF";
			break;
		case 2:
			vOut << " --affinity=FFF";
			break;
		case 3:
			vOut << " --affinity=FFFF";
			break;
		}
	}

	vOut << "\"";

	return vOut.str();
}