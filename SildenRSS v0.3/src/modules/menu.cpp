LRESULT HandleMenuCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case NRK_BUTTON:
                    selectedWebsite = nrkWebsite;  // ✅ Update selected website
                    Window::FetchAndDisplayRSS(hwnd, selectedWebsite);
                    break;

                case BBC_BUTTON:
                    selectedWebsite = bbcWebsite;  // ✅ Update selected website
                    Window::FetchAndDisplayRSS(hwnd, selectedWebsite);
                    break;

                case ID_FILE_EXIT:
                    PostQuitMessage(0);
                    break;

                case ID_HELP_ABOUT:
                    MessageBox(hwnd, "SildenRSS v0.3\nA RSS Feed\nBy silden.it", "About", MB_OK | MB_ICONINFORMATION);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
