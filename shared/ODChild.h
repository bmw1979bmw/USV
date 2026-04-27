// OwnDrawChld.h: interface for the COwnDrawChld class.
//
#pragma once

void RegisterODChild(COLORREF clBk, LPCTSTR szClassName = _T("ODChild"));
LRESULT CALLBACK ODWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


