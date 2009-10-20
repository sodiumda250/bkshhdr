// ウィンドウの作成(ダミーのウィンドウ)
HWND hHiddenParentWnd = CreateWindowEx(WS_EX_TOOLWINDOW,
            lpszMyClassName, NULL, 0, CW_USEDEFAULT, 0,
            CW_USEDEFAULT, 0, NULL, NULL, hMyInstance, NULL);
