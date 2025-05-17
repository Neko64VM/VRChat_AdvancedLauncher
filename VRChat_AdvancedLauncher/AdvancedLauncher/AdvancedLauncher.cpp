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
			out << config.GetDefaultConfig().dump(4);
		}
	}

	// jsonからVRChatのインストール先を読み取る
	m_szVRChatInstallationPath = config.ReadInstallPath(m_szConfigPath, m_szConfigFileName);

	// 初回かインストール先が変更された場合
	if (m_szVRChatInstallationPath.size() == 0 || !utils::file::IsExistsDirectory(m_szVRChatInstallationPath) || !utils::file::DoesFileExistInDirectory(m_szVRChatInstallationPath, "VRChat.exe"))
	{
		// MsgBox
		MessageBox(nullptr, "VRChatのインストール先を検索します。\nこれにはしばらく時間がかかります。続けるにはOKを押してください。", "情報", MB_TOPMOST | MB_OK | MB_ICONINFORMATION);

		// VRChat自体のインストール先を取得
		m_szVRChatInstallationPath = FindVRChatInstallationPath();

		if (m_szVRChatInstallationPath.size() == 0) {
			MessageBox(nullptr, "VRChatのインストール先が見つかりませんでした。", "ERROR", MB_TOPMOST | MB_OK | MB_ICONERROR);
		}	
		else {
			// jsonに保存
			config.WriteInstallPath(m_szConfigPath, m_szConfigFileName, m_szVRChatInstallationPath);
		}
	}

	m_szVRChatFullPath = m_szVRChatInstallationPath + "\\launch.exe";

	// モニターの数を取得
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&m_iMonitorCount));

	// config.jsonから設定をロード
	config.LoadLauncherSetting(m_szConfigPath, m_szConfigFileName);

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
	if (utils::process::StartProcess(m_szVRChatFullPath, BuildCommand()))
		config.SaveLauncherSetting(m_szConfigPath, m_szConfigFileName);
	else
		MessageBox(nullptr, "VRChatの起動に失敗しました…。", "ERROR", MB_OK | MB_TOPMOST | MB_ICONERROR);
}

std::string AdvancedLauncher::FindVRChatInstallationPath()
{
	static std::string targetDir = "SteamLibrary";
	std::vector<std::string> steam_dir_list;

	// PC内のディレクトリ名をスキャンしてSteamライブラリを探す
	for (const auto& drive_root : utils::GetPhysicalDriveList()) 
	{
		auto result = utils::file::FindDirectory(drive_root, targetDir);

		if (result)
			steam_dir_list.push_back(*result);
	}

	// SteamLibをベースにVRChatのインストール先を探す
	for (const auto& dir : steam_dir_list)
	{
		std::string common_path = dir + "\\steamapps\\common";

		if (std::filesystem::is_directory(common_path)) {
			auto vrc_dir = std::filesystem::directory_iterator(common_path);

			for (const auto& file : vrc_dir) {
				if (file.path().string().find("VRChat") != std::string::npos) {
					steam_dir_list.clear();
					return file.path().string();
				}
			}
		}
	}

	steam_dir_list.clear();
	
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