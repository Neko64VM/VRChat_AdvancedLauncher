#pragma once
#include <string>
#include <vector>
#include <regex>
#include <shlobj.h>
#include <iostream>
#include <optional>
#include <Windows.h>
#include <filesystem>
#include <TlHelp32.h>

#ifndef UTILS_H
#define UTILS_H

namespace utils
{
	namespace file
	{
		bool IsExistsFile(const std::string path);
		bool IsExistsDirectory(const std::string path);
		void SelectFilePath(std::string& vOut);
		void SelectDirectoryPath(const char* str, std::string& vOut);
		bool DoesFileExistInDirectory(const std::string path, const std::string file);
		std::string GetAppDataPath(const GUID id);
		std::vector<std::string> GetFileList(const std::string path, const std::string extension);
		std::optional<std::string> FindDirectory(const std::string& startDir, const std::string& targetName);
	}
	namespace process
	{
		DWORD GetProcessIDByName(const std::string processName);
		bool IsProcessRunning(const std::string processName);
		void StartProcess(const std::string path);
		void StopProcess(const std::string processName);
	}

	std::vector<std::string> GetPhysicalDriveList();
	std::string ConvertWideToMultiByte(const std::wstring& target_str);
}

#endif