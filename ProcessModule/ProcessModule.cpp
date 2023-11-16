#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>


class ProcessModule
{
public:
    DWORD GetProcessIdByName(const wchar_t* pName)
    {
        DWORD pId = 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pId);

        if (hSnap != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(pe32);

            if (Process32First(hSnap, &pe32))
            {
                do
                {
                    if (!lstrcmp(pe32.szExeFile, pName))
                    {
                        pId = pe32.th32ProcessID;
                        break;
                    }
                } while (Process32Next(hSnap, &pe32));
            }
        }

        return pId;
    }

    uintptr_t GetProcessModule(DWORD pId, const wchar_t* mName)
    {
        uintptr_t moduleBaseAddress = 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pId);
        
        if (hSnap != INVALID_HANDLE_VALUE) 
        {
            MODULEENTRY32 pe32;
            pe32.dwSize = sizeof(pe32);

            if (Module32First(hSnap, &pe32))
            {
                do
                {
                    if (!lstrcmp(pe32.szModule, mName))
                    {
                        moduleBaseAddress = (uintptr_t)pe32.modBaseAddr;
                        break;
                    }
                } while (Module32Next(hSnap, &pe32));
            }
        }

        return moduleBaseAddress;
    }

    HANDLE AttachProcess(DWORD pId)
    {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pId);

        if (hProcess != INVALID_HANDLE_VALUE)
        {
            return hProcess;
        }
        else
        {
            return 0;
        }
    }

    BOOL wpm(HANDLE hProcess, int address, int data)
    {
        return WriteProcessMemory(hProcess, (void*)address, &data, sizeof(int), NULL);
    }

    BOOL rpm(HANDLE hProcess, int address, int data)
    {
        return ReadProcessMemory(hProcess, (void*)address, &data, sizeof(int), NULL);
    }
};


int main()
{
    ProcessModule pm;
    DWORD pId = pm.GetProcessIdByName(L"steam.exe");
    printf("Process pId: %d\n", pId);

    HANDLE hProcss = pm.AttachProcess(pId);
    printf("Attahed process: %d\n", (int)hProcss);

    int data = 0;
    pm.rpm(hProcss, 0x90675, data);
    printf("Data: %d", data);
}

