#include "Utils.h"

namespace utils
{
	namespace file
	{
		bool IsExistsFile(const std::string path)
		{
			return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
		}
		bool IsExistsDirectory(const std::string path)
		{
			return std::filesystem::exists(path) && std::filesystem::is_directory(path);
		}
		bool DoesFileExistInDirectory(const std::string path, const std::string file)
		{
			for (const auto& file : std::filesystem::directory_iterator(path)) {
				if (file.is_regular_file()) {
					if (!file.path().compare(file)) {
						return true;
					}
				}
			}

			return false;
		}
		void SelectFilePath(std::string& vOut)
		{
			OPENFILENAME ofn;
			char szFile[260]{};

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.TXT\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) == TRUE) {
				vOut = ofn.lpstrFile;
			}
		}
		void SelectDirectoryPath(const char* str, std::string& vOut)
		{
			// Path
			BROWSEINFO bf{};
			char szFolderPath[MAX_PATH]{};

			bf.lpszTitle = str;
			bf.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

			// ダイアログを表示
			LPITEMIDLIST pidl = SHBrowseForFolder(&bf);
			if (pidl != NULL) {
				// 選択されたフォルダーのパスを取得
				if (SHGetPathFromIDList(pidl, szFolderPath)) {
					vOut = szFolderPath;
				}
				CoTaskMemFree(pidl); // メモリを解放
			}
		}
		// 参照：https://learn.microsoft.com/ja-jp/windows/win32/shell/knownfolderid
		std::string GetAppDataPath(const GUID id)
		{
			PWSTR path = nullptr;
			std::string result;

			if (SUCCEEDED(SHGetKnownFolderPath(id, 0, NULL, &path)))
				result = ConvertWideToMultiByte(path);
			else
				return std::string();

			CoTaskMemFree(path);

			return result;
		}
		std::vector<std::string> GetFileList(const std::string path, const std::string extension)
		{
			std::vector<std::string> filelist;

			for (const auto& file : std::filesystem::directory_iterator(path))
			{
				if (file.is_regular_file())
				{
					if (!extension.compare(file.path().extension().string()))
						filelist.push_back(file.path().filename().string());
				}
			}

			return filelist;
		}
		std::optional<std::string> FindDirectory(const std::string& startDir, const std::string& targetName)
		{
			WIN32_FIND_DATA findFileData;
			HANDLE hFind = FindFirstFile((startDir + "\\*").c_str(), &findFileData);

			if (hFind == INVALID_HANDLE_VALUE)
				return std::nullopt;

			do {
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					std::string dirName = findFileData.cFileName;
					if (dirName != "." && dirName != "..")
					{
						if (dirName == targetName)
						{
							FindClose(hFind);
							return startDir + "\\" + dirName;
						}
						else
						{
							auto result = FindDirectory(startDir + "\\" + dirName, targetName);
							if (result) {
								FindClose(hFind);
								return result;
							}
						}
					}
				}
			} while (FindNextFile(hFind, &findFileData) != 0);

			FindClose(hFind);
			return std::nullopt;
		}
	}
	namespace process
	{
		DWORD GetProcessIDByName(const std::string processName)
		{
			DWORD PID = 0;
			PROCESSENTRY32 entry{};
			entry.dwSize = sizeof(PROCESSENTRY32);
			const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

			while (Process32Next(snapShot, &entry)) {
				if (!processName.compare(entry.szExeFile))
					PID = entry.th32ProcessID;
			}

			CloseHandle(snapShot);

			return PID;
		}
		bool IsProcessRunning(const std::string processName)
		{
			return GetProcessIDByName(processName) != 0;
		}
		void StartProcess(const std::string target)
		{
			system(target.c_str());
		}
		void StopProcess(const std::string processName)
		{
			std::string cmd = "taskkill /IM " + processName + " /F";
			system(cmd.c_str());
		}
	}

	std::vector<std::string> GetPhysicalDriveList()
	{
		std::vector<std::string> driveList;
		DWORD driveMask = GetLogicalDrives();

		if (driveMask == 0)
			return driveList;

		for (char drive = 'A'; drive <= 'Z'; ++drive)
		{
			if (driveMask & (1 << (drive - 'A')))
			{
				std::string drivePath = std::string(1, drive) + ":\\";
				UINT driveType = GetDriveType(drivePath.c_str());
				if (driveType == DRIVE_FIXED)
					driveList.push_back(drivePath);
			}
		}
		return driveList;
	}
	std::string ConvertWideToMultiByte(const std::wstring& target_str)
	{
		if (target_str.empty())
			return std::string();

		const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, target_str.c_str(), -1, nullptr, 0, nullptr, nullptr);

		if (bufferSize == 0)
			return std::string();

		std::string vOut(bufferSize - 1, '\0');
		WideCharToMultiByte(CP_UTF8, 0, target_str.c_str(), -1, &vOut[0], bufferSize, nullptr, nullptr);

		return vOut;
	}
	
}