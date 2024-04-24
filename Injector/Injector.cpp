#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {

    const WCHAR* process_name = L"Nerwia.exe";

    std::string dll_path_release = *argv;

    dll_path_release.erase(dll_path_release.find_last_of("\\") + 1).append("HyperBot.dll");

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (wcscmp(entry.szExeFile, process_name) == 0)
            {
                DWORD process_id = entry.th32ProcessID;

                HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
                if (process_handle == NULL) 
                {
                    cout << "Cant open process handle" << endl;
                    return 1;
                }

                LPVOID remote_memory = VirtualAllocEx(process_handle, NULL, strlen(dll_path_release.c_str()) + 1, MEM_COMMIT, PAGE_READWRITE);
                if (remote_memory == NULL) 
                {
                    cout << "Cant allocate memory" << endl;
                    CloseHandle(process_handle);
                    return 1;
                }

                if (WriteProcessMemory(process_handle, remote_memory, dll_path_release.c_str(), dll_path_release.size() + 1, NULL) == 0) 
                {
                    cout << "Cant write memory" << endl;
                    CloseHandle(process_handle);
                    return 1;
                }

                HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, remote_memory, 0, NULL);
                if (thread_handle == NULL) 
                {
                    cout << "Cant create remote thread" << endl;
                    CloseHandle(process_handle);
                    return 1;
                }

                WaitForSingleObject(thread_handle, INFINITE);

                CloseHandle(thread_handle);
                VirtualFreeEx(process_handle, remote_memory, 0/*dll_path_release.size() + 1*/, MEM_RELEASE);
                CloseHandle(process_handle);

                cout << "Injected into process " << process_name << endl;
            }
        }
    }

    return 1;
}