#pragma once


#ifndef IATFUSCATE_H
#define IATFUSCATE_H

#include <Windows.h>

typedef unsigned int UINT32_T;
HMODULE GetDllHandle(UINT32_T DllNameHash);
FARPROC GetFunctionAddress(HMODULE ModuleHandle, UINT32_T FunctionNameHash);

#endif