// SetAppIcon.cpp

#include <windows.h>

void SetAppIcon(UINT nIconID, HWND hWnd)
{
    HICON hAppIcon = ::LoadIcon(::GetModuleHandle(NULL), (LPCTSTR)MAKEINTRESOURCE(nIconID));

    ::SendMessage(hWnd, WM_SETICON, ICON_BIG,   (LPARAM)hAppIcon);
    ::SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hAppIcon);
}

// end of SetAppIcon.cpp
