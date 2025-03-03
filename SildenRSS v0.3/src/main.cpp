#include "modules/headers/window.h"
#include "modules/headers/menu.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::string testRSS = fetchRSS(selectedWebsite);
    return RunApplication(hInstance, nCmdShow);
}
