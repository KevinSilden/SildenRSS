#ifndef MENU_H
#define MENU_H

#include "Windows.h"
#include "resources.h"
#include <vector>
#include <string>

void CreateAppMenu(HWND hwnd);
std::string fetchRSS();
void FetchAndDisplayRSS(HWND hwnd);
LRESULT HandleMenuCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);

#endif