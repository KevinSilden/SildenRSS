#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <vector>
#include <string>

extern std::string nrkWebsite;
extern std::string bbcWebsite;
extern std::string selectedWebsite;

extern std::string fetchRSS(const std::string& url);

class Window {
public:
    static std::vector<std::string> rssHeadlinesDescriptions;
    static std::vector<std::string> rssHeadlines;
    static std::string mainRssTitle;
    static int scrollOffset;

    static void FetchAndDisplayRSS(HWND hwnd, const std::string& website);
};

// window function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int RunApplication(HINSTANCE hInstance, int nCmdShow);
void ShowAboutDialog(HWND hwnd);

#endif
