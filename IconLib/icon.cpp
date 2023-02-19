//  WinIconPatcher - An icon patcher for Windows
// Copyright (C) 2022  namazso <admin@namazso.eu>
// Copyright (C) 2023  ItsProfessional <itsprofessional1234@gmail.com>
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "../public/icontool.h"
#include <atlcomcli.h>
#include <string>
#include <winternl.h>
#include <vector>
#include <stdexcept>
#include <comdef.h>

typedef VOID(NTAPI LDR_ENUM_CALLBACK)(_In_ PLDR_DATA_TABLE_ENTRY ModuleInformation, _In_ PVOID Parameter, _Out_ BOOLEAN* Stop);
typedef LDR_ENUM_CALLBACK* PLDR_ENUM_CALLBACK;

NTSTATUS NTAPI LdrEnumerateLoadedModules(_In_ BOOLEAN ReservedFlag, _In_ PLDR_ENUM_CALLBACK EnumProc, _In_opt_ PVOID Context);


enum THEME_MANAGER_INITIALIZATION_FLAGS : unsigned
{
	ThemeInitNoFlags = 0,
	ThemeInitCurrentThemeOnly = 1 << 0,
	ThemeInitFlagUnk1 = 1 << 1,
	ThemeInitFlagUnk2 = 1 << 2,
};

static constexpr GUID CLSID_ThemeManager2 = { 0x9324da94, 0x50ec, 0x4a14, { 0xa7, 0x70, 0xe9, 0x0c, 0xa0, 0x3e, 0x7c, 0x8f } };



















void icontool_get_pack_count(PULONG count)
{
	std::string directory = "C:\\ProgramData\\WinIconPatcher\\";

	WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    std::vector<std::string> dir_list;

    hFind = FindFirstFileA(full_path.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Invalid handle value! Please check your path...");

    while (FindNextFileA(hFind, &findData) != 0)
    {
        dir_list.push_back(std::string(findData.cFileName));
    }

    FindClose(hFind);

	*count = 0;
	int icount = dir_list.size() - 1; // you have to negate 1 because otherwise it'll show 1 extra for the count, e.g. if you have 2 files in the directory, it'll say 3 files are present in there if you do not negate minus like this
	*count = icount;
}









void icontool_get_pack(ULONG idx, std::string& pack) // using ampersand sign on pack parameter when calling the function causes a "cannot convert from std::string to <expanded from of std::string macro>" or whatever
{
	std::string directory = "C:\\ProgramData\\WinIconPatcher\\";

	WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    std::vector<std::string> dir_list;

    hFind = FindFirstFileA(full_path.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Invalid handle value! Please check your path...");

    while (FindNextFileA(hFind, &findData) != 0)
    {
        dir_list.push_back(std::string(findData.cFileName));
    }

    FindClose(hFind);

	dir_list.erase(dir_list.begin()); // remove first value, as that is ".." (the "go up" folder thingy). we do not need that, so remove it so we can use index 0 to get the first value

	for (auto& element : dir_list) {
		element = directory + element; // make it into a path

		std::string suffix = ".7z";
		bool endsWith7z = element.size() >= suffix.size() && 0 == element.compare(element.size()-suffix.size(), suffix.size(), suffix);

		if (!endsWith7z)
		{
			// if does not end with ".7z", remove the element. The below line erases by values, instead of index which is why I'm using this here
			dir_list.erase(std::remove(dir_list.begin(), dir_list.end(), element), dir_list.end()); // if this makes no sense, see: https://stackoverflow.com/a/3385251 (comment on https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position)
		}
	}


	pack = ""; // we don't need to use ampersand dereference operator for strings, which is why that is not here.
	std::string ipack = dir_list[idx]; // you have to negate 1 because otherwise it'll show 1 extra for the count, e.g. if you have 2 files in the directory, it'll say 3 files are present in there if you do not negate minus like this

	pack = ipack; // note: not to be confused to windows IPack icon themes, this is just a naming convention I think, idk.
}


void icontool_pack_get_display_name(std::string& pack, LPWSTR out, SIZE_T cch)
{
	memset(out, 0, cch * sizeof(WCHAR));
	std::wstring str;

	//const auto hr = ...
	//if (FAILED(hr))
	//	return hr;




	std::size_t filePos = pack.find_last_of("\\");

	
	//std::string dir = pack.substr(0, botDirPos); // get directory
	std::string file = pack.substr(filePos + 1, pack.length()); // get file (name + extension)


	//std::wstring stemp = std::wstring(pack.begin(), pack.end());
	//LPCWSTR pack2 = stemp.c_str();
	//str = pack2;

	// remove extension
	size_t extensionPos = file.find_last_of("."); 
	std::string name = file.substr(0, extensionPos); 

	// set display name to filename
	std::wstring stemp = std::wstring(name.begin(), name.end());
	LPCWSTR name2 = stemp.c_str();





	str = name2;



	





	if (str.size() >= cch)
	{ // I should be making these methods return HRESULT and actually do it but im too lazy lol, if you wanna do it you can make a pr
		constexpr HRESULT hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		std::wstring wString;
		wString.append(&errMsg[0]);
		std::string convertedString(wString.begin(), wString.end());

		throw std::runtime_error(convertedString);
	}

	std::copy_n(str.begin(), str.size(), out);
}




void icontool_pack_get_vs_path(std::string& pack, LPWSTR out, SIZE_T cch)
{
	memset(out, 0, cch * sizeof(WCHAR));
	std::wstring str;

	//const auto hr = theme->GetVisualStyle(str);
	//if (FAILED(hr))
	//	return hr;
	
	std::wstring stemp = std::wstring(pack.begin(), pack.end());
	LPCWSTR packStr = stemp.c_str();

	str = packStr;

	if (str.size() >= cch)
	{ // I should be making these methods return HRESULT and actually do it but im too lazy lol, if you wanna do it you can make a pr
		constexpr HRESULT hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		std::wstring wString;
		wString.append(&errMsg[0]);
		std::string convertedString(wString.begin(), wString.end());

		throw std::runtime_error(convertedString);
	}

	std::copy_n(str.begin(), str.size(), out);
}
