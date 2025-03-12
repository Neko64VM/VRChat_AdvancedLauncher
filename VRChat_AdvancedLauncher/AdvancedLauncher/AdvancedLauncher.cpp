#include "AdvancedLauncher.h"
#include "../Utils/Utils.h"
#include "../Framework/ImGui/Font/NotoSansMed.h"

// モニター数を取得
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	int* count = reinterpret_cast<int*>(dwData);
	(*count)++;
	return TRUE;
}

// デフォルトの設定ファイル
json get_default_config() {
	return json{
		{ "AvatarTest", false },
		{ "CCXEnable", false },
		{ "CCXOption", 2 },
		{ "DesktopMode", false },
		{ "FullScreen", false },
		{ "MaxFPS", 144 },
		{ "MaxFPSEnable", true },
		{ "Monitor", 0 },
		{ "ProfileID", 0 },
		{ "VRChatPath", "" },
		{ "WorldTest", false },
		{ "WindowSize", 0 }
	};
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
			json default_config = get_default_config();
			out << default_config.dump(4);
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
	config.LoadSetting(m_szConfigPath, m_szConfigFileName);

	return true;
}

void AdvancedLauncher::ProcessThread()
{
	std::string command = m_szVRChatFullPath + BuildCommand();
	std::cout << command << std::endl;

	utils::process::StartProcess(command.c_str());
	config.SaveSetting(m_szConfigPath, m_szConfigFileName);
}

std::string AdvancedLauncher::FindVRChatInstallationPath()
{
	static std::string targetDir = "SteamLibrary";
	std::vector<std::string> steam_dir_list;

	// Steamライブラリを探す
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

	if (g.g_DesktopMode) vOut << " --no-vr";

	vOut << " -screen-fullscreen " + std::to_string((int)g.g_FullScreen);

	std::string width  = " -screen-width ";
	std::string height = " -screen-height ";

	switch (g.g_WindowSize)
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

	std::string monitor_str = " -monitor " + std::to_string((int)g.g_Monitor + 1);
	vOut << monitor_str;

	if (g.g_MaxFPSEnable) {
		std::string framerate_str = " --fps=" + std::to_string((int)g.g_MaxFPS);
		vOut << framerate_str;
	}

	if (g.g_AvatarTest) vOut << " --watch-avatars";
	if (g.g_WorldTest) vOut << " --watch-world";

	std::string profile_str = " --profile=" + std::to_string((int)g.g_ProfileID);
	vOut << profile_str;

	if (g.g_CCX_Enable) {
		switch (g.g_CCX_Option)
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

	//std::cout << "Builded Command : " << vOut.str() << std::endl;

	return vOut.str();
}