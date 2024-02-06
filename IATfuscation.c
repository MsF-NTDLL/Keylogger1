#pragma once
#include <Windows.h>


// |--------------------------------------------------------|
// |########################################################|
// |################### IATfuscation #######################|
// |########################################################|
// |--------------------------------------------------------|
// |   Obfuscate the used APIs via API hashing              |
// |   Mask the IAT by importing random WinAPI functions    |											  
// |--------------------------------------------------------|


// ---------------------------------------------------------
// Acquire DLL base address/function address by comparing hash values, instead of strings.

// Function to create the FNV hash of a C-style string. Non-cryptographic algorithm, but it doesn't matter for this purpose
UINT32_T FNV1a(const char* text) 
{ 
	assert(text);   
	while (*text) 
	{
		hash = FNV1a((unsigned char)*text++, hash);
	}

	return hash; 
}

// Get a handle on the DLL with the specified hash (in uppercase, i.e. NTDLL.DLL)
HMODULE GetDllHandle(UINT32_T DllNameHash) 
{

#ifdef _WIN64
	PPEB PEB = (PEB*)(__readgsqword(0x60));
#elif _WIN32
	PPEB PEB = (PEB*)(__readfsdword(0x30));
#endif

	// Contains info about the loaded modules
	PPEB_LDR_DATA LDRData = (PPEB_LDR_DATA)(pPeb->Ldr);
	PLDR_DATA_TABLE_ENTR DTE = (PLDR_DATA_TABLE_ENTRY)(LDRData->InMemoryOrderModuleList.Flink);

	while (DTE) 
	{

		if (DTE->FullDllName.Length != NULL && DTE->FullDllName.Length < MAX_PATH) 
		{	

			CHAR DllName[MAX_PATH];

			for (DWORD i = 0; pDte->FullDllName.Buffer[i]; i++)
			{
				DllName[i] = (CHAR)toupper(pDte->FullDllName.Buffer[i]);
			}
			DllName[i] = '\0';

			if (FNV1a(DllName) == DllNameHash)
			{
				return pDte->Reserved2[0];
			}
		}
		else
		{
			return;
		}

		DTE = *(PLDR_DATA_TABLE_ENTRY*)(DTE);
	}
}

// Retrieve the address of an exported function from the the DLL module
FARPROC GetFunctionAddress(HMODULE ModuleHandle, UINT32_T FunctionNameHash) 
{
	// Start parsing through the PE until we find the export directory table
	PBYTE BaseAddress = (PBYTE)ModuleHandle;

	PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)BaseAddress;
	PIMAGE_NT_HEADERS NTHeaders = (PIMAGE_NT_HEADERS)(BaseAddress + DOSHeader->e_lfanew);
	if (DOSHeader->e_magic != IMAGE_DOS_SIGNATURE || NTHeaders->Signature != IMAGE_NT_SIGNATURE) 
	{
		return;
	}

	IMAGE_OPTIONAL_HEADER OptionalHeader = NTHeaders->OptionalHeader;

	// We found it, now we can get 
	PIMAGE_EXPORT_DIRECTORY ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(BaseAddress + OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	PDWORD FunctionNameArray = (PDWORD)(BaseAddress + ExportDirectory->AddressOfNames);
	PDWORD FunctionAddressArray = (PDWORD)(BaseAddress + ExportDirectory->AddressOfFunctions);
	PWORD FunctionOrdinalArray = (PWORD)(BaseAddress + ExportDirectory->AddressOfNameOrdinals);

	// Iterate through the exported functions 
	for (DWORD i = 0; i < ExportDirectory->NumberOfFunctions; i++)
	{
		CHAR* FunctionName = (CHAR*)(BaseAddress + FunctionNameArray[i]);
		PVOID FunctionAddress = (PVOID)(BaseAddress + FunctionAddressArray[FunctionOrdinalArray[i]]);

		// If the hashes are equal, we found the desired function
		if (FunctionNameHash == FNV1a(FunctionName))
		{
			return FunctionAddress;
		}
	}

	return;
}


// ---------------------------------------------------------
// Masking the IAT by importing random WinAPI functions

// Import benign WinAPI functions. Commented out cuz its not needed anymore!
//void ObfuscateIAT() 
//{	
	// The volatile keyword will make the compiler not optimize this code away
	//volatile size_t x = 0;

	// Random bullshit, GO!
	//x = GetLastError();
	//x = SetCriticalSectionSpinCount(NULL, NULL);
	//x = GetWindowContextHelpId(NULL);
	//x = GetWindowLongPtrW(NULL, NULL);
	//x = RegisterClassW(NULL);
	//x = IsWindowVisible(NULL);
	//x = ConvertDefaultLocale(NULL);
	//x = MultiByteToWideChar(NULL, NULL, NULL, NULL, NULL, NULL);
	//x = IsDialogMessageW(NULL, NULL);
//}
