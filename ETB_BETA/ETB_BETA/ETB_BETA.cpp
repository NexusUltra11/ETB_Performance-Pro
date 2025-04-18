#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <gdiplus.h>

#pragma comment(lib, "Gdiplus.lib")

using namespace std;

// 🧠 Helper-Funktion zum Finden von CLSID für BMP-Encoding
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    using namespace Gdiplus;
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;

    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return -1;
}

// 🖼️ Wallpaper-Änderung nach Delay
void ChangeWallpaperAfterDelay(const wstring& imagePath, int delaySeconds) {
    this_thread::sleep_for(chrono::seconds(delaySeconds)); // z.B. 300 = 5 Minuten

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromFile(imagePath.c_str(), FALSE);
    if (!image || image->GetLastStatus() != Gdiplus::Ok) {
        wcerr << L"[Wallpaper] Failed to load PNG." << endl;
        return;
    }

    wstring bmpPath = imagePath.substr(0, imagePath.find_last_of(L".")) + L".bmp";
    CLSID bmpClsid;
    GetEncoderClsid(L"image/bmp", &bmpClsid);
    if (image->Save(bmpPath.c_str(), &bmpClsid, NULL) != Gdiplus::Ok) {
        wcerr << L"[Wallpaper] Failed to convert to BMP." << endl;
        delete image;
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return;
    }

    delete image;
    Gdiplus::GdiplusShutdown(gdiplusToken);

    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)bmpPath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
    wcout << L"[Wallpaper] Desktop wallpaper changed!" << endl;
}

// 🔄 Simuliertes "Injecting..."
void animateInjecting() {
    const string msg = "Injecting";
    while (true) {
        for (int i = 0; i < 4; ++i) {
            cout << "\r" << msg << string(i, '.') << "   ";
            cout.flush();
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
}

// 🎮 Performance-Texte ausgeben
void displayLiveConsole() {
    vector<string> messages = {
        "[Mod Engine] Escape The Backrooms Performance Mod Injected",
        "[Mod Engine] Rendering Level Range: High",
        "[Mod Engine] DLSS: Enabled",
        "[Mod Engine] Shadows: Real-Time Raytraced",
        "[Mod Engine] VRAM Usage: 1.7GB",
        "[Mod Engine] FPS Lock: 144",
        "[Mod Engine] Monitoring in progress..."
    };
    int index = 0;
    while (true) {
        cout << messages[index % messages.size()] << endl;
        this_thread::sleep_for(chrono::seconds(3));
        index++;
    }
}

// ▶️ Spiel starten
PROCESS_INFORMATION launchGame() {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    wstring command = L"cmd /c start steam://run/1943950"; // Steam-ID
    CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    return pi;
}

int main() {
    // 🎮 Spiel starten
    PROCESS_INFORMATION pi = launchGame();

    // 🧵 Starte Injecting-Animation (kurzzeitig)
    thread animThread([]() {
        animateInjecting();
        });

    // Warten und dann Animation abbrechen
    this_thread::sleep_for(chrono::seconds(4));
    TerminateThread(animThread.native_handle(), 0);
    cout << "\n[Mod Engine] Injected successfully.\n" << endl;

    // 🧵 Starte Performance-Ausgabe
    thread perfThread(displayLiveConsole);

    // 🧵 Starte Wallpaper-Änderung nach 5 Minuten
    thread wallpaperThread(ChangeWallpaperAfterDelay, L"wallpaper.png", 300);
    wallpaperThread.detach();

    // ⏳ Warten bis Spielprozess endet
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Aufräumen
    TerminateThread(perfThread.native_handle(), 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
