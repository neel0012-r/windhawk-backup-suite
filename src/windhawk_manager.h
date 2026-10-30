#pragma once

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include "json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

struct ModInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    bool disabled = false;
    bool hasSource = false;
};

inline std::string WStringToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
}

inline std::wstring UTF8ToWString(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstr[0], size);
    return wstr;
}

class WindhawkManager {
public:
    static bool IsInstalled() {
        // Check Registry or Program Files or ProgramData
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Windhawk", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
        if (fs::exists("C:\\Program Files\\Windhawk\\windhawk.exe") ||
            fs::exists("C:\\ProgramData\\Windhawk")) {
            return true;
        }
        return false;
    }

    static std::string GetInstallPath() {
        if (fs::exists("C:\\Program Files\\Windhawk\\windhawk.exe")) {
            return "C:\\Program Files\\Windhawk";
        }
        return "";
    }

    static std::string GetAppDataPath() {
        if (fs::exists("C:\\ProgramData\\Windhawk")) {
            return "C:\\ProgramData\\Windhawk";
        }
        return "";
    }

    static void OpenDownloadUrl() {
        ShellExecuteW(NULL, L"open", L"https://ramensoftware.com/downloads/windhawk_setup.exe", NULL, NULL, SW_SHOWNORMAL);
    }

    static void OpenFolder(const std::wstring& path) {
        if (fs::is_regular_file(path)) {
            std::wstring param = L"/select,\"" + path + L"\"";
            ShellExecuteW(NULL, L"open", L"explorer.exe", param.c_str(), NULL, SW_SHOWNORMAL);
        } else {
            ShellExecuteW(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
    }

    static void ParseModSource(const std::wstring& filePath, ModInfo& mod) {
        std::ifstream file(filePath.c_str());
        if (!file.is_open()) return;
        mod.hasSource = true;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("==/WindhawkMod==") != std::string::npos) break;
            if (line.find("@name") != std::string::npos) {
                size_t idx = line.find("@name");
                std::string val = line.substr(idx + 5);
                while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(val.begin());
                if (!val.empty()) mod.name = val;
            } else if (line.find("@version") != std::string::npos) {
                size_t idx = line.find("@version");
                std::string val = line.substr(idx + 8);
                while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(val.begin());
                if (!val.empty()) mod.version = val;
            } else if (line.find("@author") != std::string::npos) {
                size_t idx = line.find("@author");
                std::string val = line.substr(idx + 7);
                while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(val.begin());
                if (!val.empty()) mod.author = val;
            } else if (line.find("@description") != std::string::npos) {
                size_t idx = line.find("@description");
                std::string val = line.substr(idx + 12);
                while (!val.empty() && (val.front() == ' ' || val.front() == '\t')) val.erase(val.begin());
                if (!val.empty()) mod.description = val;
            }
        }
    }

    static std::vector<ModInfo> ScanMods() {
        std::vector<ModInfo> mods;
        std::map<std::string, ModInfo> modMap;

        // 1. Scan Registry HKLM\SOFTWARE\Windhawk\Engine\Mods
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Windhawk\\Engine\\Mods", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t subKeyName[256];
            DWORD index = 0;
            DWORD nameLen = 256;

            while (RegEnumKeyExW(hKey, index++, subKeyName, &nameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                ModInfo mod;
                mod.id = WStringToUTF8(subKeyName);
                mod.name = mod.id;
                mod.disabled = false;
                mod.hasSource = false;

                HKEY hModKey;
                if (RegOpenKeyExW(hKey, subKeyName, 0, KEY_READ, &hModKey) == ERROR_SUCCESS) {
                    DWORD disabled = 0;
                    DWORD type = REG_DWORD;
                    DWORD size = sizeof(DWORD);
                    if (RegQueryValueExW(hModKey, L"Disabled", NULL, &type, (LPBYTE)&disabled, &size) == ERROR_SUCCESS) {
                        mod.disabled = (disabled != 0);
                    }
                    RegCloseKey(hModKey);
                }

                modMap[mod.id] = mod;
                nameLen = 256;
            }
            RegCloseKey(hKey);
        }

        // 2. Scan ProgramData\Windhawk\ModsSource
        std::wstring modsSourceDir = L"C:\\ProgramData\\Windhawk\\ModsSource";
        if (fs::exists(modsSourceDir)) {
            for (const auto& entry : fs::directory_iterator(modsSourceDir)) {
                if (entry.is_regular_file() && entry.path().extension() == L".cpp") {
                    std::wstring filename = entry.path().filename().wstring();
                    // filename usually is <mod_id>.wh.cpp
                    size_t pos = filename.find(L".wh.cpp");
                    if (pos != std::wstring::npos) {
                        std::wstring modIdW = filename.substr(0, pos);
                        std::string modId = WStringToUTF8(modIdW);
                        
                        ModInfo& mod = modMap[modId];
                        mod.id = modId;
                        ParseModSource(entry.path().wstring(), mod);
                    }
                }
            }
        }

        for (auto& pair : modMap) {
            // Clean up name if empty
            if (pair.second.name.empty()) {
                pair.second.name = pair.second.id;
            }
            mods.push_back(pair.second);
        }

        return mods;
    }

    // Helper to read all registry values of a key into JSON
    static json ReadRegistryKeyValues(HKEY hKey) {
        json valuesJson = json::object();
        DWORD index = 0;
        wchar_t valName[256];
        DWORD valNameLen = 256;
        DWORD valType = 0;
        BYTE data[4096];
        DWORD dataLen = sizeof(data);

        while (RegEnumValueW(hKey, index++, valName, &valNameLen, NULL, &valType, data, &dataLen) == ERROR_SUCCESS) {
            std::string name = WStringToUTF8(valName);
            json valObj = json::object();

            if (valType == REG_SZ || valType == REG_EXPAND_SZ) {
                std::wstring wstr = (wchar_t*)data;
                valObj["type"] = "sz";
                valObj["data"] = WStringToUTF8(wstr);
            } else if (valType == REG_DWORD) {
                DWORD dwordVal = *(DWORD*)data;
                valObj["type"] = "dword";
                valObj["data"] = dwordVal;
            } else if (valType == REG_BINARY) {
                // store as hex string
                std::stringstream ss;
                for (DWORD i = 0; i < dataLen; ++i) {
                    ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
                }
                valObj["type"] = "binary";
                valObj["data"] = ss.str();
            } else {
                valObj["type"] = "raw";
                valObj["data"] = "";
            }

            valuesJson[name] = valObj;

            valNameLen = 256;
            dataLen = sizeof(data);
        }
        return valuesJson;
    }

    // Export registry tree for a mod into JSON
    static json ExportModRegistry(const std::string& modId) {
        json modReg = json::object();
        std::wstring subKeyPath = L"SOFTWARE\\Windhawk\\Engine\\Mods\\" + UTF8ToWString(modId);

        HKEY hModKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_READ, &hModKey) == ERROR_SUCCESS) {
            modReg["values"] = ReadRegistryKeyValues(hModKey);

            // Check Settings subkey
            HKEY hSettingsKey;
            if (RegOpenKeyExW(hModKey, L"Settings", 0, KEY_READ, &hSettingsKey) == ERROR_SUCCESS) {
                modReg["settings"] = ReadRegistryKeyValues(hSettingsKey);
                RegCloseKey(hSettingsKey);
            }

            RegCloseKey(hModKey);
        }
        return modReg;
    }

    // Restore registry tree for a mod from JSON
    static bool RestoreModRegistry(const std::string& modId, const json& modReg) {
        std::wstring subKeyPath = L"SOFTWARE\\Windhawk\\Engine\\Mods\\" + UTF8ToWString(modId);
        HKEY hModKey;
        if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hModKey, NULL) != ERROR_SUCCESS) {
            return false;
        }

        // Write mod root values
        if (modReg.contains("values") && modReg["values"].is_object()) {
            for (auto& item : modReg["values"].items()) {
                std::wstring nameW = UTF8ToWString(item.key());
                const auto& valObj = item.value();
                std::string type = valObj.value("type", "");

                if (type == "sz") {
                    std::wstring dataW = UTF8ToWString(valObj.value("data", ""));
                    RegSetValueExW(hModKey, nameW.c_str(), 0, REG_SZ, (const BYTE*)dataW.c_str(), (DWORD)((dataW.size() + 1) * sizeof(wchar_t)));
                } else if (type == "dword") {
                    DWORD dw = valObj.value("data", 0);
                    RegSetValueExW(hModKey, nameW.c_str(), 0, REG_DWORD, (const BYTE*)&dw, sizeof(DWORD));
                }
            }
        }

        // Write Settings subkey values
        if (modReg.contains("settings") && modReg["settings"].is_object()) {
            HKEY hSettingsKey;
            if (RegCreateKeyExW(hModKey, L"Settings", 0, NULL, 0, KEY_WRITE, NULL, &hSettingsKey, NULL) == ERROR_SUCCESS) {
                for (auto& item : modReg["settings"].items()) {
                    std::wstring nameW = UTF8ToWString(item.key());
                    const auto& valObj = item.value();
                    std::string type = valObj.value("type", "");

                    if (type == "sz") {
                        std::wstring dataW = UTF8ToWString(valObj.value("data", ""));
                        RegSetValueExW(hSettingsKey, nameW.c_str(), 0, REG_SZ, (const BYTE*)dataW.c_str(), (DWORD)((dataW.size() + 1) * sizeof(wchar_t)));
                    } else if (type == "dword") {
                        DWORD dw = valObj.value("data", 0);
                        RegSetValueExW(hSettingsKey, nameW.c_str(), 0, REG_DWORD, (const BYTE*)&dw, sizeof(DWORD));
                    }
                }
                RegCloseKey(hSettingsKey);
            }
        }

        RegCloseKey(hModKey);
        return true;
    }

    // Execute PowerShell command silently
    static bool RunPowerShellSilent(const std::wstring& cmd) {
        std::wstring fullCmd = L"powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -Command \"" + cmd + L"\"";
        
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        std::vector<wchar_t> cmdBuf(fullCmd.begin(), fullCmd.end());
        cmdBuf.push_back(0);

        if (CreateProcessW(NULL, cmdBuf.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, 30000); // 30s timeout
            DWORD exitCode = 1;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return (exitCode == 0);
        }
        return false;
    }

    // Restart Windhawk
    static void RestartWindhawk() {
        std::wstring exePath = L"C:\\Program Files\\Windhawk\\windhawk.exe";
        if (fs::exists(exePath)) {
            // First send exit or restart
            ShellExecuteW(NULL, L"open", exePath.c_str(), L"-restart -tray-only", NULL, SW_HIDE);
        }
    }

    // Create Backup
    static bool CreateBackup(const std::vector<std::string>& selectedModIds,
                             const std::wstring& targetZipPath,
                             std::string& outError) {
        try {
            auto now = std::chrono::system_clock::now();
            auto timeT = std::chrono::system_clock::to_time_t(now);
            std::stringstream ssTime;
            ssTime << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");

            wchar_t tempPath[MAX_PATH];
            GetTempPathW(MAX_PATH, tempPath);
            std::wstring stagingDir = std::wstring(tempPath) + L"WindhawkVault_Staging_" + std::to_wstring(timeT);
            std::wstring stagingModsDir = stagingDir + L"\\ModsSource";

            fs::create_directories(stagingModsDir);

            json manifest = json::object();
            manifest["vault_app"] = "Windhawk Backup Suite";
            manifest["version"] = "1.0.0";
            manifest["created_at"] = ssTime.str();
            manifest["total_mods"] = selectedModIds.size();
            manifest["mods"] = json::array();

            json regBackup = json::object();

            auto allMods = ScanMods();
            std::map<std::string, ModInfo> modMap;
            for (const auto& m : allMods) modMap[m.id] = m;

            for (const auto& id : selectedModIds) {
                ModInfo mod = modMap[id];
                json modItem = json::object();
                modItem["id"] = mod.id;
                modItem["name"] = mod.name;
                modItem["version"] = mod.version;
                modItem["author"] = mod.author;
                modItem["description"] = mod.description;
                modItem["disabled"] = mod.disabled;
                manifest["mods"].push_back(modItem);

                // Copy .wh.cpp file
                std::wstring srcPath = L"C:\\ProgramData\\Windhawk\\ModsSource\\" + UTF8ToWString(id) + L".wh.cpp";
                if (fs::exists(srcPath)) {
                    std::wstring destPath = stagingModsDir + L"\\" + UTF8ToWString(id) + L".wh.cpp";
                    fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);
                }

                // Export registry
                regBackup[id] = ExportModRegistry(id);
            }

            // Write manifest.json
            {
                std::ofstream mFile((stagingDir + L"\\manifest.json").c_str());
                mFile << manifest.dump(4);
            }

            // Write registry.json
            {
                std::ofstream rFile((stagingDir + L"\\registry.json").c_str());
                rFile << regBackup.dump(4);
            }

            // If target zip already exists, remove it
            if (fs::exists(targetZipPath)) {
                fs::remove(targetZipPath);
            }

            // Compress staging directory into target zip
            std::wstring psCmd = L"Compress-Archive -Path '" + stagingDir + L"\\*' -DestinationPath '" + targetZipPath + L"' -Force";
            bool success = RunPowerShellSilent(psCmd);

            // Clean staging
            std::error_code ec;
            fs::remove_all(stagingDir, ec);

            if (!success || !fs::exists(targetZipPath)) {
                outError = "Failed to create ZIP archive using Compress-Archive.";
                return false;
            }

            return true;
        } catch (const std::exception& ex) {
            outError = ex.what();
            return false;
        }
    }

    // Inspect Backup ZIP
    static bool InspectBackup(const std::wstring& zipPath, json& outManifest, std::string& outError) {
        try {
            if (!fs::exists(zipPath)) {
                outError = "Backup file not found!";
                return false;
            }

            wchar_t tempPath[MAX_PATH];
            GetTempPathW(MAX_PATH, tempPath);
            auto timeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::wstring extractDir = std::wstring(tempPath) + L"WindhawkVault_Inspect_" + std::to_wstring(timeT);
            fs::create_directories(extractDir);

            // Fast extract ONLY manifest.json instead of whole archive
            std::wstring manifestPath = extractDir + L"\\manifest.json";
            std::wstring psCmd = L"Add-Type -A System.IO.Compression.FileSystem; "
                                 L"$z=[System.IO.Compression.ZipFile]::OpenRead('" + zipPath + L"'); "
                                 L"$e=$z.GetEntry('manifest.json'); "
                                 L"if($e){ [System.IO.Compression.ZipFileExtensions]::ExtractToFile($e, '" + manifestPath + L"', $true); } "
                                 L"$z.Dispose()";
            
            RunPowerShellSilent(psCmd);

            // Fallback to full extraction if fast extract didn't produce manifest
            if (!fs::exists(manifestPath)) {
                std::wstring fallbackCmd = L"Expand-Archive -Path '" + zipPath + L"' -DestinationPath '" + extractDir + L"' -Force";
                RunPowerShellSilent(fallbackCmd);
            }

            if (!fs::exists(manifestPath)) {
                outError = "Invalid backup archive: manifest.json is missing!";
                std::error_code ec;
                fs::remove_all(extractDir, ec);
                return false;
            }

            std::ifstream mFile(manifestPath.c_str());
            outManifest = json::parse(mFile);

            std::error_code ec;
            fs::remove_all(extractDir, ec);
            return true;
        } catch (const std::exception& ex) {
            outError = ex.what();
            return false;
        }
    }

    // Restore Backup
    static bool RestoreBackup(const std::wstring& zipPath,
                             const std::vector<std::string>& selectedModIds,
                             bool restartWindhawk,
                             std::string& outError) {
        try {
            if (!fs::exists(zipPath)) {
                outError = "Backup file not found!";
                return false;
            }

            wchar_t tempPath[MAX_PATH];
            GetTempPathW(MAX_PATH, tempPath);
            auto timeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::wstring extractDir = std::wstring(tempPath) + L"WindhawkVault_Restore_" + std::to_wstring(timeT);
            fs::create_directories(extractDir);

            // Expand archive
            std::wstring psCmd = L"Expand-Archive -Path '" + zipPath + L"' -DestinationPath '" + extractDir + L"' -Force";
            if (!RunPowerShellSilent(psCmd)) {
                outError = "Failed to extract backup archive.";
                std::error_code ec;
                fs::remove_all(extractDir, ec);
                return false;
            }

            std::wstring manifestPath = extractDir + L"\\manifest.json";
            std::wstring regPath = extractDir + L"\\registry.json";

            json regBackup = json::object();
            if (fs::exists(regPath)) {
                std::ifstream rFile(regPath.c_str());
                regBackup = json::parse(rFile);
            }

            std::wstring targetModsSource = L"C:\\ProgramData\\Windhawk\\ModsSource";
            fs::create_directories(targetModsSource);

            for (const auto& id : selectedModIds) {
                // Restore .wh.cpp file
                std::wstring srcModFile = extractDir + L"\\ModsSource\\" + UTF8ToWString(id) + L".wh.cpp";
                if (fs::exists(srcModFile)) {
                    std::wstring destModFile = targetModsSource + L"\\" + UTF8ToWString(id) + L".wh.cpp";
                    fs::copy_file(srcModFile, destModFile, fs::copy_options::overwrite_existing);
                }

                // Restore registry
                if (regBackup.contains(id)) {
                    RestoreModRegistry(id, regBackup[id]);
                }
            }

            std::error_code ec;
            fs::remove_all(extractDir, ec);

            if (restartWindhawk) {
                RestartWindhawk();
            }

            return true;
        } catch (const std::exception& ex) {
            outError = ex.what();
            return false;
        }
    }
};
