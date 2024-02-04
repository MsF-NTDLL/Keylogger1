#pragma once

typedef HWND(WINAPI* fnGetForegroundWindow)(void);

typedef LRESULT(WINAPI* fnCallNextHookEx)(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam);

// Will populate more when the client is actually finished