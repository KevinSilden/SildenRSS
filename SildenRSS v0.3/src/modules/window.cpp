#include "../modules/headers/window.h"
#include "../modules/headers/resources.h"
#include "../modules/headers/file_dialog.h"
#include "headers/menu.h"
#include <dwmapi.h>  
#include <shlwapi.h>
#include "../resources/libraries/pugixml.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <wininet.h>
#include <shellapi.h>
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "Comctl32.lib")

const char CLASS_NAME[] = "SimpleResizableWindow";
std::vector<std::string> Window::rssHeadlines;
std::vector<std::string> Window::rssHeadlinesDescriptions;
std::string Window::mainRssTitle = "";
std::vector<RECT> titleRects;  
std::vector<std::string> articleLinks;

int Window::scrollOffset = 0;

std::string nrkWebsite = "https://www.nrk.no/toppsaker.rss";
std::string nyTimesWebsite = "https://rss.nytimes.com/services/xml/rss/nyt/World.xml";
std::string selectedWebsite = nrkWebsite; // Default selection

// "Custom font" for title
std::wstring utf8_to_wstring(const std::string& utf8_str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    if (len == 0) return L"";
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], len);
    return wstr;
}

std::string fetchRSS(const std::string& url) {
    HINTERNET hInternet = InternetOpen("RSS Reader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        MessageBox(NULL, "No internet!", "Error", MB_OK | MB_ICONERROR);
        return "";
    }

    HINTERNET hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        MessageBox(NULL, "Failed to open RSS feed!", "Error", MB_OK | MB_ICONERROR);
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[4096];
    DWORD bytesRead;
    std::string response;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return response;
}
           

void Window::FetchAndDisplayRSS(HWND hwnd, const std::string& website) {
    std::string xml = fetchRSS(website);  // ✅ Dynamically fetch the selected RSS feed
        if (xml.empty()) {
        MessageBox(hwnd, "Fetched RSS is empty!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    rssHeadlines.clear();  // ✅ Clear old headlines
    articleLinks.clear();
    mainRssTitle = "";     // ✅ Reset main title

    pugi::xml_document doc;
    if (!doc.load_string(xml.c_str())) {
        MessageBox(hwnd, "Failed to parse RSS XML!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    rssHeadlines.clear();  // Clear old headlines
    mainRssTitle = "";  // Reset the main title

    pugi::xml_node channel = doc.child("rss").child("channel");
    if (!channel) {
        MessageBox(hwnd, "No <channel> found!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Extract the main RSS feed title from <channel>
    pugi::xml_node channelTitleNode = channel.child("title");
    if (channelTitleNode) {
        mainRssTitle = channelTitleNode.child_value();
    }

    // Extract and store headline titles, descriptions and links

    for (pugi::xml_node item = channel.child("item"); item; item = item.next_sibling("item")) {
        std::string title = item.child("title").child_value();
        std::string description = item.child("description").child_value();
        std::string link = item.child("link").child_value();

    if (!title.empty()) {
        Window::rssHeadlines.push_back(title);
        titleRects.push_back({0, 0, 0, 0});
    }

    if (!description.empty()) {
        Window::rssHeadlinesDescriptions.push_back(description);
    }

    if (!link.empty()) {
        articleLinks.push_back(link);
    } else {
        articleLinks.push_back("https://www.nrk.no/");  // Default link if missing
    }
}
    InvalidateRect(hwnd, NULL, TRUE);  // Force repaint
}

// Detect if dark mode is enabled in regedit (0 = Dark Mode, 1 = Light Mode). Light mode is horrible atm ...... 
bool IsDarkModeEnabled() {
    DWORD value = 1;
    HKEY hKey;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, 
                     "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 
                     0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(value);
        RegQueryValueEx(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    return value == 0;
}

// Centering application on screen when launched
void CenterWindow(HWND hwnd, int width, int height) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - width) / 2;
    int posY = (screenHeight - height) / 2;
    SetWindowPos(hwnd, NULL, posX, posY, width, height, SWP_NOZORDER | SWP_NOSIZE);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hBrush = NULL;
    switch (uMsg) {

        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            Window::scrollOffset -= delta / 2;
            if (Window::scrollOffset < 0) Window::scrollOffset = 0;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_UP:
                    Window::scrollOffset -= 20;
                    if (Window::scrollOffset < 0) Window::scrollOffset = 0;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case VK_DOWN:
                    Window::scrollOffset += 20;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
            }

        case WM_CREATE:
            hBrush = IsDarkModeEnabled() ? 
                (HBRUSH)GetStockObject(BLACK_BRUSH) : (HBRUSH)GetStockObject(WHITE_BRUSH);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {

                case NYTIMES_BUTTON:
                selectedWebsite = nyTimesWebsite;
                Window::FetchAndDisplayRSS(hwnd, selectedWebsite);
                break;
                
                case NRK_BUTTON:
                selectedWebsite = nrkWebsite;
                Window::FetchAndDisplayRSS(hwnd, selectedWebsite);
                break;

                case ID_FILE_EXIT:
                    PostQuitMessage(0);
                    break;

                case ID_HELP_ABOUT:
                    ShowAboutDialog(hwnd);
                    break;
            }
            break;

            case WM_LBUTTONDOWN: {
                int mouseX = LOWORD(lParam);
                int mouseY = HIWORD(lParam);
            
                for (size_t i = 0; i < titleRects.size(); i++) {
                    if (mouseX >= titleRects[i].left && mouseX <= titleRects[i].right &&
                        mouseY >= titleRects[i].top && mouseY <= titleRects[i].bottom) {
            
                        // Open links in browser(blue text)
                        ShellExecuteA(NULL, "open", articleLinks[i].c_str(), NULL, NULL, SW_SHOWNORMAL);
                        break;
                    }
                }
            }
            break;
            
        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETCLIENTAREAANIMATION || wParam == 0) {
                HBRUSH newBrush = IsDarkModeEnabled() ? 
                    (HBRUSH)GetStockObject(BLACK_BRUSH) : (HBRUSH)GetStockObject(WHITE_BRUSH);

                if (hBrush) DeleteObject(hBrush);
                hBrush = newBrush;

                SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
                InvalidateRect(hwnd, NULL, TRUE);

                DWORD darkMode = IsDarkModeEnabled() ? 1 : 0;
                DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
            }
            break;

        case WM_ERASEBKGND: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect((HDC)wParam, &rect, hBrush);
            return 1;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
        
            // Setting a dark BG
            SetBkMode(hdc, TRANSPARENT);
            RECT rect;
            GetClientRect(hwnd, &rect);
        
            int y = 20 - Window::scrollOffset;
        
        // Setting the font for the title of the website
        HFONT hTitleFont = CreateFontW(
        28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, L"Arial"
        );

        HFONT hOldFont = (HFONT)SelectObject(hdc, hTitleFont);

        if (!Window::mainRssTitle.empty()) {
            std::wstring wMainTitle = utf8_to_wstring(Window::mainRssTitle);  // ✅ Convert to UTF-16 properly

            SetTextColor(hdc, RGB(0, 255, 0));  // ✅ Green for the main title
            TextOutW(hdc, 10, y, wMainTitle.c_str(), wcslen(wMainTitle.c_str()));  // ✅ Proper string length
            y += 40;  // Move text lower for spacing
    }

    SelectObject(hdc, hOldFont);
    DeleteObject(hTitleFont);
    SetTextColor(hdc, RGB(255, 255, 255));

    titleRects.clear();

    for (size_t i = 0; i < Window::rssHeadlines.size(); i++) {
        std::wstring wTitle = utf8_to_wstring(Window::rssHeadlines[i]);

        RECT titleRect = {10, y, 600, y + 20};
        titleRects.push_back(titleRect);

    SetTextColor(hdc, RGB(0, 0, 255));  //blue text, "hyperlinks"
    DrawTextW(hdc, wTitle.c_str(), -1, &titleRect, DT_LEFT);
    y += 25;

    if (i < Window::rssHeadlinesDescriptions.size()) {
        std::wstring wDescription = utf8_to_wstring(Window::rssHeadlinesDescriptions[i]);

        RECT descRect = {10, y, 600, y + 100};  
        SetTextColor(hdc, RGB(180, 180, 180));  

        DrawTextW(hdc, wDescription.c_str(), -1, &descRect, DT_WORDBREAK | DT_LEFT);
        y = descRect.bottom + 10;  
    }
}


//Display news headlines & descriptions, convertion for NOR letters
SetTextColor(hdc, RGB(255, 255, 255));

for (size_t i = 0; i < Window::rssHeadlines.size(); i++) {
    std::wstring wTitle = utf8_to_wstring(Window::rssHeadlines[i]);  
    TextOutW(hdc, 10, y, wTitle.c_str(), wcslen(wTitle.c_str()));  
    y += 20;

    // Descriptions, text wrapping
    if (i < Window::rssHeadlinesDescriptions.size()) {
        std::wstring wDescription = utf8_to_wstring(Window::rssHeadlinesDescriptions[i]);

        RECT descRect = {10, y, 300, y + 100};
        SetTextColor(hdc, RGB(180, 180, 180));

        DrawTextW(hdc, wDescription.c_str(), -1, &descRect, DT_WORDBREAK | DT_LEFT);
        y = descRect.bottom + 10;
    }
    SetTextColor(hdc, RGB(255, 255, 255));  // Reset color for next headline
}

EndPaint(hwnd, &ps);
break;
        }
        case WM_DESTROY:
            if (hBrush) DeleteObject(hBrush);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int RunApplication(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSEX wc = {};  
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = IsDarkModeEnabled() ? 
        (HBRUSH)GetStockObject(BLACK_BRUSH) : (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Load application icons
    wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(APP_ICON), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(APP_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window registration failed!", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "SildenRSS - v.0.3", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    // Apply dark mode settings if enabled
    if (IsDarkModeEnabled()) {
        DWORD darkMode = 1;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
    }

    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)wc.hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)wc.hIconSm);

    CenterWindow(hwnd, 800, 600);

    // Drop down menus
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hWebsitesMenu = CreatePopupMenu();
    HMENU hHelpMenu = CreatePopupMenu();

    AppendMenu(hWebsitesMenu, MF_STRING, NRK_BUTTON, "NRK");
    AppendMenu(hWebsitesMenu, MF_STRING, NYTIMES_BUTTON, "New York Times");
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, "Exit");
    AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, "About");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hWebsitesMenu, "Websites");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, "Help");

    SetMenu(hwnd, hMenu);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void ShowAboutDialog(HWND hwnd) {
    MessageBox(hwnd, 
        "SildenRSS v0.3\nA RSS Feed\nBy silden.it\n", 
        "About", 
        MB_OK);
}