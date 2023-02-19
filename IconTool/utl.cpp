// WinIconPatcher - An icon patcher for Windows
// Copyright (C) 2022  namazso <admin@namazso.eu>
// Copyright (C) 2023  ItsProfessional <itsprofessional1234@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"

#include "utl.h"



EXTERN_C_START

NTSYSAPI NTSTATUS NTAPI NtOpenKey(_Out_ PHANDLE KeyHandle, _In_  ACCESS_MASK DesiredAccess, _In_  POBJECT_ATTRIBUTES ObjectAttributes);
NTSYSCALLAPI NTSTATUS NTAPI NtOpenSymbolicLinkObject(_Out_ PHANDLE LinkHandle, _In_ ACCESS_MASK DesiredAccess, _In_ POBJECT_ATTRIBUTES ObjectAttributes);
NTSYSCALLAPI NTSTATUS NTAPI NtQuerySymbolicLinkObject(_In_ HANDLE LinkHandle, _Inout_ PUNICODE_STRING LinkTarget, _Out_opt_ PULONG ReturnedLength);

EXTERN_C_END




// Last time instance wasn't just your own PE header's pointer was in 16 bit days.
HINSTANCE utl::get_instance() { return (HINSTANCE)&__ImageBase; }

static OBJECT_ATTRIBUTES make_object_attributes(
	const wchar_t* ObjectName,
	ULONG Attributes = OBJ_CASE_INSENSITIVE,
	HANDLE RootDirectory = nullptr,
	PSECURITY_DESCRIPTOR SecurityDescriptor = nullptr
)
{
	OBJECT_ATTRIBUTES a;
	UNICODE_STRING ustr;
	RtlInitUnicodeString(&ustr, ObjectName);
	InitializeObjectAttributes(&a, &ustr, Attributes, RootDirectory, SecurityDescriptor
	);
	return a;
}

const std::pair<std::wstring, std::wstring> utl::session_user()
{
	LPWSTR wtsinfo_ptr = nullptr;
	DWORD bytes_returned = 0;
	const auto success = WTSQuerySessionInformationW(
		WTS_CURRENT_SERVER_HANDLE,
		WTS_CURRENT_SESSION,
		WTSSessionInfo,
		&wtsinfo_ptr,
		&bytes_returned
	);
	if (success && wtsinfo_ptr)
	{
		const auto wtsinfo = (WTSINFOW*)wtsinfo_ptr;
		std::wstring username{ wtsinfo->UserName };
		std::wstring domain{ wtsinfo->Domain };
		WTSFreeMemory(wtsinfo_ptr);
		return std::make_pair(std::move(domain), std::move(username));
	}
	return {};
}

const std::pair<std::wstring, std::wstring> utl::process_user()
{
	std::pair<std::wstring, std::wstring> pair;
	HANDLE token = nullptr;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
	{
		struct data_s
		{
			TOKEN_USER user;
			char data[0x800];
		} s;
		static_assert(offsetof(data_s, user) == 0, "this is not good");

		DWORD size = sizeof(s);
		auto success = GetTokenInformation(
			token,
			TokenUser,
			&s,
			sizeof(s),
			&size
		);
		if (success)
		{
			WCHAR username[USERNAME_LENGTH + 1]{};
			DWORD username_len = (DWORD)std::size(username);
			WCHAR domain[DOMAIN_LENGTH + 1]{};
			DWORD domain_len = (DWORD)std::size(domain);
			SID_NAME_USE name_use{};
			success = LookupAccountSidW(
				nullptr,
				s.user.User.Sid,
				username,
				&username_len,
				domain,
				&domain_len,
				&name_use
			);
			if (success)
				pair = std::pair<std::wstring, std::wstring>(domain, username);
		}
		CloseHandle(token);
	}
	return pair;
}

std::wstring utl::ErrorToString(HRESULT error)
{
	wchar_t buf[0x1000];

	FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		error,
		MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT),
		buf,
		(DWORD)std::size(buf),
		nullptr
	);
	std::wstring wstr{ buf };
	const auto pos = wstr.find_last_not_of(L"\r\n");
	if (pos != std::wstring::npos)
		wstr.resize(pos);
	return wstr;
}





//int utl::readzip(int argc, char* argv[])
//{
//    if (argc != 2)
//        return -1;
//  
//    std::ifstream file(argv[1]);
//    if (!file.is_open())
//        return -2;
//  
//    int errorp = 0;
//    zip_t* arch = zip_open(argv[1], 0, &errorp);
//    if (arch == NULL)
//        return errorp;
//  
//    zip_stat_t finfo;
//    zip_stat_init(&finfo);
//  
//    zip_file_t* fd = NULL;
//    char* txt = NULL;
//    int count = 0;
//    while (zip_stat_index(arch, count, 0, &finfo) == 0) {
//        txt = new char[finfo.size + 1];
//        fd = zip_fopen_index(arch, count, 0);
//        zip_fread(fd, txt, finfo.size);
//        std::cout << "file #" << count + 1 << ": " << finfo.name << std::endl << std::endl;
//        std::cout << txt << std::endl << std::endl;
//        delete[] txt;
//        count++;
//    }
//    return 0;
//}

void utl::GetOpenWindows(std::vector<std::wstring> &windows)
{
	HWND hwnd = GetForegroundWindow();
	while (hwnd != NULL)
	{
		TCHAR className[256];
		GetClassName(hwnd, className, sizeof(className));
		if (_tcscmp(className, L"CabinetWClass") == 0 || _tcscmp(className, L"ExploreWClass") == 0)
		{
			TCHAR windowText[256];
			GetWindowText(hwnd, windowText, sizeof(windowText));
			windows.push_back(windowText);
		}
		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
	}
}

bool utl::FindProcessId(const std::wstring &processName, DWORD &processId)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return false;

	Process32First(processesSnapshot, &processInfo);
	do
	{
		if (_tcsicmp(processName.c_str(), processInfo.szExeFile) == 0)
		{
			processId = processInfo.th32ProcessID;
			CloseHandle(processesSnapshot);
			return true;
		}
	} while (Process32Next(processesSnapshot, &processInfo));

	CloseHandle(processesSnapshot);
	return false;
}


bool utl::GetFilesInDirectory(const std::wstring &directory, const std::wstring &format, std::vector<std::wstring> &files)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile((directory + L"\\" + format).c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		files.push_back(findData.cFileName);
	} while (FindNextFile(hFind, &findData));

	FindClose(hFind);
	return true;
}
