#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>

#include "WebView2.h"
#include "embedded_loader.h"
#include "windhawk_manager.h"
#include "ui_html.h"
#include "json.hpp"

using json = nlohmann::json;

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

// Function pointer for dynamic loader
typedef HRESULT (STDAPICALLTYPE *CreateCoreWebView2EnvironmentWithOptions_fn)(
    PCWSTR browserExecutableFolder,
    PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions* environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler
);

// Global WebView2 pointers
static ICoreWebView2Controller* g_controller = nullptr;
static ICoreWebView2* g_webview = nullptr;
static HWND g_hwnd = nullptr;

#define WM_APP_ASYNC_JSON (WM_APP + 10)

// Helper to send JSON to WebView
void SendJsonToWebview(const json& j) {
    if (!g_webview) return;
    std::string dump = j.dump();
    std::wstring script = L"if (window.onVaultMessage) window.onVaultMessage(" + UTF8ToWString(dump) + L");";
    g_webview->ExecuteScript(script.c_str(), nullptr);
}

// Thread-safe post to UI thread
void PostJsonToUI(const json& j) {
    json* pCopy = new json(j);
    PostMessageW(g_hwnd, WM_APP_ASYNC_JSON, 0, (LPARAM)pCopy);
}

// Forward declaration of message processor
void HandleWebMessage(const std::string& msgJsonStr);

// COM Handler: Environment Completed
class EnvironmentCompletedHandler : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler {
    std::atomic<ULONG> ref_{1};
    HWND hwnd_;
public:
    EnvironmentCompletedHandler(HWND hwnd) : hwnd_(hwnd) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler) {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++ref_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG c = --ref_;
        if (c == 0) delete this;
        return c;
    }
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT hr, ICoreWebView2Environment* env) override;
};

// COM Handler: WebMessage Received
class WebMessageReceivedHandler : public ICoreWebView2WebMessageReceivedEventHandler {
    std::atomic<ULONG> ref_{1};
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_ICoreWebView2WebMessageReceivedEventHandler) {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++ref_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG c = --ref_;
        if (c == 0) delete this;
        return c;
    }
    HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) override {
        LPWSTR msgRaw = nullptr;
        if (SUCCEEDED(args->get_WebMessageAsJson(&msgRaw)) && msgRaw) {
            std::string msgStr = WStringToUTF8(msgRaw);
            CoTaskMemFree(msgRaw);
            HandleWebMessage(msgStr);
        }
        return S_OK;
    }
};

// COM Handler: Controller Completed
class ControllerCompletedHandler : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
    std::atomic<ULONG> ref_{1};
    HWND hwnd_;
public:
    ControllerCompletedHandler(HWND hwnd) : hwnd_(hwnd) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler) {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++ref_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG c = --ref_;
        if (c == 0) delete this;
        return c;
    }
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT hr, ICoreWebView2Controller* controller) override {
        if (FAILED(hr) || !controller) return hr;
        g_controller = controller;
        g_controller->AddRef();

        g_controller->get_CoreWebView2(&g_webview);

        RECT bounds;
        GetClientRect(hwnd_, &bounds);
        g_controller->put_Bounds(bounds);

        // Add WebMessage event handler
        EventRegistrationToken token;
        auto msgHandler = new WebMessageReceivedHandler();
        g_webview->add_WebMessageReceived(msgHandler, &token);
        msgHandler->Release();

        // Navigate to embedded UI
        g_webview->NavigateToString(UTF8ToWString(UI_HTML).c_str());

        return S_OK;
    }
};

HRESULT STDMETHODCALLTYPE EnvironmentCompletedHandler::Invoke(HRESULT hr, ICoreWebView2Environment* env) {
    if (FAILED(hr) || !env) return hr;
    auto handler = new ControllerCompletedHandler(hwnd_);
    env->CreateCoreWebView2Controller(hwnd_, handler);
    handler->Release();
    return S_OK;
}

// Get Desktop Folder
std::wstring GetDesktopPath() {
    wchar_t path[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, path) == S_OK) {
        return path;
    }
    return L"C:\\";
}

// Open Native Windows Folder Picker
std::wstring PickFolder(HWND parent) {
    BROWSEINFOW bi = { 0 };
    bi.hwndOwner = parent;
    bi.lpszTitle = L"Select Destination Folder for Windhawk Backup";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl != 0) {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, path)) {
            CoTaskMemFree(pidl);
            return path;
        }
        CoTaskMemFree(pidl);
    }
    return L"";
}

// Open Native Windows Open File Dialog for .zip
std::wstring PickZipFile(HWND parent) {
    wchar_t filename[MAX_PATH] = { 0 };
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = parent;
    ofn.lpstrFilter = L"Windhawk Backup (*.zip)\0*.zip\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = L"Select Windhawk Backup Archive";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        return filename;
    }
    return L"";
}

// Handle all messages from web UI
void HandleWebMessage(const std::string& msgJsonStr) {
    try {
        json msg = json::parse(msgJsonStr);
        std::string action = msg.value("action", "");

        if (action == "app_ready" || action == "refresh_mods") {
            bool installed = WindhawkManager::IsInstalled();
            auto mods = WindhawkManager::ScanMods();

            json resp = json::object();
            resp["type"] = "init";
            resp["isInstalled"] = installed;
            resp["defaultDesktop"] = WStringToUTF8(GetDesktopPath());
            resp["mods"] = json::array();

            for (const auto& m : mods) {
                json item = json::object();
                item["id"] = m.id;
                item["name"] = m.name;
                item["version"] = m.version;
                item["author"] = m.author;
                item["description"] = m.description;
                item["disabled"] = m.disabled;
                resp["mods"].push_back(item);
            }

            SendJsonToWebview(resp);
        }
        else if (action == "install_windhawk") {
            WindhawkManager::OpenDownloadUrl();
        }
        else if (action == "browse_destination_folder") {
            std::wstring folder = PickFolder(g_hwnd);
            if (!folder.empty()) {
                json resp = json::object();
                resp["type"] = "destination_folder_selected";
                resp["folder"] = WStringToUTF8(folder);
                SendJsonToWebview(resp);
            }
        }
        else if (action == "create_backup") {
            std::vector<std::string> selectedMods;
            if (msg.contains("selectedModIds") && msg["selectedModIds"].is_array()) {
                for (const auto& id : msg["selectedModIds"]) {
                    selectedMods.push_back(id.get<std::string>());
                }
            }

            std::wstring targetZipPath = UTF8ToWString(msg.value("targetZipPath", ""));

            std::thread([selectedMods, targetZipPath]() {
                std::string error;
                bool ok = WindhawkManager::CreateBackup(selectedMods, targetZipPath, error);
                if (ok) {
                    json resp = json::object();
                    resp["type"] = "backup_complete";
                    resp["path"] = WStringToUTF8(targetZipPath);
                    PostJsonToUI(resp);
                } else {
                    json resp = json::object();
                    resp["type"] = "error";
                    resp["message"] = error.empty() ? "Failed to create backup archive." : error;
                    PostJsonToUI(resp);
                }
            }).detach();
        }
        else if (action == "browse_zip_file") {
            std::wstring zipPath = PickZipFile(g_hwnd);
            if (!zipPath.empty()) {
                // Immediately notify webview that inspection started so moving progress bar shows
                json startMsg = json::object();
                startMsg["type"] = "inspect_started";
                startMsg["path"] = WStringToUTF8(zipPath);
                SendJsonToWebview(startMsg);

                std::thread([zipPath]() {
                    json manifest;
                    std::string error;
                    if (WindhawkManager::InspectBackup(zipPath, manifest, error)) {
                        json resp = json::object();
                        resp["type"] = "zip_selected";
                        resp["path"] = WStringToUTF8(zipPath);
                        resp["manifest"] = manifest;
                        PostJsonToUI(resp);
                    } else {
                        json resp = json::object();
                        resp["type"] = "error";
                        resp["message"] = error.empty() ? "Could not inspect the selected zip archive." : error;
                        PostJsonToUI(resp);
                    }
                }).detach();
            }
        }
        else if (action == "restore_backup") {
            std::wstring zipPath = UTF8ToWString(msg.value("zipPath", ""));
            std::vector<std::string> selectedMods;
            if (msg.contains("selectedModIds") && msg["selectedModIds"].is_array()) {
                for (const auto& id : msg["selectedModIds"]) {
                    selectedMods.push_back(id.get<std::string>());
                }
            }
            bool restart = msg.value("restartWindhawk", true);

            std::thread([zipPath, selectedMods, restart]() {
                std::string error;
                bool ok = WindhawkManager::RestoreBackup(zipPath, selectedMods, restart, error);
                if (ok) {
                    json resp = json::object();
                    resp["type"] = "restore_complete";
                    PostJsonToUI(resp);
                } else {
                    json resp = json::object();
                    resp["type"] = "error";
                    resp["message"] = error.empty() ? "Failed to restore mods from backup." : error;
                    PostJsonToUI(resp);
                }
            }).detach();
        }
        else if (action == "open_folder") {
            std::wstring path = UTF8ToWString(msg.value("path", ""));
            if (!path.empty()) {
                WindhawkManager::OpenFolder(path);
            }
        }
        else if (action == "minimize") {
            ShowWindow(g_hwnd, SW_MINIMIZE);
        }
        else if (action == "close") {
            PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
        }
    } catch (const std::exception& ex) {
        json resp = json::object();
        resp["type"] = "error";
        resp["message"] = ex.what();
        SendJsonToWebview(resp);
    }
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_APP_ASYNC_JSON: {
        json* pJson = (json*)lParam;
        if (pJson) {
            SendJsonToWebview(*pJson);
            delete pJson;
        }
        return 0;
    }
    case WM_SIZE:
        if (g_controller) {
            RECT bounds;
            GetClientRect(hwnd, &bounds);
            g_controller->put_Bounds(bounds);
        }
        return 0;
    case WM_DESTROY:
        if (g_controller) {
            g_controller->Close();
            g_controller->Release();
            g_controller = nullptr;
        }
        if (g_webview) {
            g_webview->Release();
            g_webview = nullptr;
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// Check if running as administrator
BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return fRet;
}

// WinMain entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)pCmdLine;

    // Self-elevation check
    if (!IsElevated()) {
        wchar_t szPath[MAX_PATH];
        if (GetModuleFileNameW(NULL, szPath, ARRAYSIZE(szPath))) {
            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.lpVerb = L"runas";
            sei.lpFile = szPath;
            sei.nShow = SW_NORMAL;
            if (ShellExecuteExW(&sei)) {
                return 0;
            }
        }
    }

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // Extract embedded WebView2Loader.dll
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring loaderDir = std::wstring(tempPath) + L"WindhawkVault";
    CreateDirectoryW(loaderDir.c_str(), NULL);
    std::wstring loaderDll = loaderDir + L"\\WebView2Loader.dll";

    if (!fs::exists(loaderDll)) {
        FILE* fp = _wfopen(loaderDll.c_str(), L"wb");
        if (fp) {
            fwrite(embedded_webview2_loader, 1, embedded_webview2_loader_len, fp);
            fclose(fp);
        }
    }

    HMODULE hLoader = LoadLibraryW(loaderDll.c_str());
    if (!hLoader) {
        MessageBoxW(NULL, L"Could not load WebView2Loader component.", L"Windhawk Vault Error", MB_ICONERROR);
        return 1;
    }

    auto pfnCreateCoreWebView2EnvironmentWithOptions = 
        (CreateCoreWebView2EnvironmentWithOptions_fn)GetProcAddress(hLoader, "CreateCoreWebView2EnvironmentWithOptions");

    if (!pfnCreateCoreWebView2EnvironmentWithOptions) {
        MessageBoxW(NULL, L"Could not resolve WebView2 initialization.", L"Windhawk Vault Error", MB_ICONERROR);
        return 1;
    }

    // Load Icon
    HICON hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    HICON hIconSm = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    if (!hIcon) hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(1));
    if (!hIconSm) hIconSm = hIcon;

    // Register Window Class
    const wchar_t CLASS_NAME[] = L"WindhawkBackupSuite_App";
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = hIcon;
    wc.hIconSm = hIconSm;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassExW(&wc);

    // Calculate centered position
    int width = 980;
    int height = 700;
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenW - width) / 2;
    int y = (screenH - height) / 2;

    HWND hwnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        CLASS_NAME,
        L"Windhawk Backup Suite",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        x, y, width, height,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 1;
    g_hwnd = hwnd;

    if (hIcon) SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    if (hIconSm) SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);

    // Enable Windows 11 Dark Mode & Native Rounded Corners
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

    // Initialize WebView2
    std::wstring userDataFolder = loaderDir + L"\\wv2_profile";
    auto envHandler = new EnvironmentCompletedHandler(hwnd);
    HRESULT hr = pfnCreateCoreWebView2EnvironmentWithOptions(nullptr, userDataFolder.c_str(), nullptr, envHandler);
    envHandler->Release();

    if (FAILED(hr)) {
        MessageBoxW(hwnd, L"Microsoft Edge WebView2 Runtime is required.\nPlease install WebView2 to use Windhawk Vault.", L"Windhawk Vault", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message Loop
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    FreeLibrary(hLoader);
    CoUninitialize();
    return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}

