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

#include "MainDialog.h"

#include <chrono>

#include "utl.h"

#pragma comment(lib, "Shell32.lib")

// note: using undocumented stuff is bad
extern "C" NTSYSAPI VOID NTAPI RtlGetNtVersionNumbers(_Out_opt_ PULONG NtMajorVersion, _Out_opt_ PULONG NtMinorVersion, _Out_opt_ PULONG NtBuildNumber);
extern "C" NTSYSAPI NTSTATUS NTAPI RtlAdjustPrivilege(_In_ ULONG Privilege, _In_ BOOLEAN Enable, _In_ BOOLEAN Client, _Out_ PBOOLEAN WasEnabled);


static constexpr wchar_t kHelpText[] =
LR"(- For any custom themes to work SecureUxTheme or another patcher must be installed
- Styles need to be signed, the signature just doesn't need to be valid
  - To add an invalid signature to a style select a theme using it and click Patch
- After install and reboot, there are multiple ways to set themes:
  - If "Hook explorer" is enabled you can use "Personalization" to set a patched theme
  - If "Hook SystemSettings" is enabled you can use "Themes" to set a patched theme
)";


static constexpr wchar_t kHelpText2[] =
LR"(  - You can simply use IconTool to patch and apply themes (recommended)
- To prevent LogonUI from resetting colors either
  - DefaultColors must be renamed / deleted
  - or LogonUI must be hooked
)";





static std::wstring GetWindowTextStr(HWND hwnd) // get text of control
{
	SetLastError(0);
	const auto len = GetWindowTextLengthW(hwnd);
	const auto error = GetLastError();
	if (len == 0 && error != 0)
		return {};
	std::wstring str;
	str.resize(len + 1);
	str.resize(GetWindowTextW(hwnd, str.data(), (int)str.size()));
	return str;
}




void MainDialog::Log(const wchar_t* fmt, ...)
{
	std::wstring str;
	va_list args;
	va_start(args, fmt);
	utl::vfmt(str, fmt, args);
	va_end(args);

	auto log = GetWindowTextStr(_hwnd_LOG);
	if (!log.empty())
		log.append(ESTRt(L"\r\n"));
	//LARGE_INTEGER li{};
	//QueryPerformanceCounter(&li);

	const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	log.append(std::to_wstring(ms));
	log.append(ESTRt(L" > "));
	log.append(str);
	SetWindowTextW(_hwnd_LOG, log.c_str());
}









//void MainDialog::Uninstall()
//{
//	Log(ESTRt(L"Uninstall started..."));
//
//	const auto hr = secureuxtheme_uninstall();
//
//	Log(ESTRt(L"secureuxtheme_uninstall() returned %08X"), hr);
//
//	if (FAILED(hr))
//		utl::FormattedMessageBox(_hwnd, ESTRt(L"Error"), MB_OK | MB_ICONERROR, ESTRt(L"Uninstall failed. Error: %s"), utl::ErrorToString(hr).c_str());
//
//	UpdatePatcherState();
//}










//void MainDialog::Install()
//{
//	Log(ESTRt(L"Install started..."));
//
//	ULONG install_flags{};
//	if (BST_CHECKED == Button_GetCheck(_hwnd_CHECK_EXPLORER))
//		install_flags |= SECUREUXTHEME_INSTALL_HOOK_EXPLORER;
//	if (BST_CHECKED == Button_GetCheck(_hwnd_CHECK_SYSTEMSETTINGS))
//		install_flags |= SECUREUXTHEME_INSTALL_HOOK_SETTINGS;
//	if (BST_CHECKED == Button_GetCheck(_hwnd_CHECK_LOGONUI))
//		install_flags |= SECUREUXTHEME_INSTALL_HOOK_LOGONUI;
//	if (BST_CHECKED == Button_GetCheck(_hwnd_CHECK_COLORS))
//		install_flags |= SECUREUXTHEME_INSTALL_RENAME_DEFAULTCOLORS;
//
//	const auto hr = secureuxtheme_install(install_flags);
//
//	Log(ESTRt(L"secureuxtheme_uninstall(%08X) returned %08X"), install_flags, hr);
//
//
//
//
//	if (FAILED(hr))
//	{
//		utl::FormattedMessageBox(_hwnd, ESTRt(L"Error"), MB_OK | MB_ICONERROR, ESTRt(L"Install failed. Error: %s"), utl::ErrorToString(hr).c_str());
//	}
//	else
//	{
//		const auto reboot = IDYES == utl::FormattedMessageBox(_hwnd, ESTRt(L"Success"), MB_YESNO, ESTRt(L"Installing succeeded, patcher will be loaded next boot. Do you want to reboot now?"));
//
//
//
//
//		if (reboot)
//		{
//			BOOLEAN old = FALSE;
//
//			const auto status = RtlAdjustPrivilege(19, TRUE, FALSE, &old);
//			Log(ESTRt(L"RtlAdjustPrivilege returned %08X"), status);
//			if (!NT_SUCCESS(status))
//			{
//
//				utl::FormattedMessageBox(_hwnd, ESTRt(L"Error"), MB_OK | MB_ICONERROR, ESTRt(L"Adjusting shutdown privilege failed. Error: %s"), utl::ErrorToString(HRESULT_FROM_WIN32(RtlNtStatusToDosError(status))).c_str());
//
//
//				return;
//			}
//
//
//			const auto succeeded = ExitWindowsEx(EWX_REBOOT, 0);
//			if (!succeeded)
//			{
//				const auto ret = GetLastError();
//				Log(ESTRt(L"ExitWindowsEx failed with GetLastError() = %08X"), ret);
//
//				utl::FormattedMessageBox(_hwnd, ESTRt(L"Error"), MB_OK | MB_ICONERROR, ESTRt(L"Rebooting failed. Error: %s"), utl::ErrorToString(HRESULT_FROM_WIN32(ret)).c_str());
//			}
//		}
//
//
//	}
//}





















static const wchar_t* PatcherStateText(PatcherState state)
{
	static const wchar_t* const text[] = { L"No", L"Yes", L"Probably", L"Outdated" };
	return text[(size_t)state];
}



void MainDialog::UpdatePatcherState()
{
	//const auto state = secureuxtheme_get_state_flags();
	//_is_installed = state & SECUREUXTHEME_STATE_INSTALLED ? (state & SECUREUXTHEME_STATE_CURRENT ? PatcherState::Yes : PatcherState::Outdated) : PatcherState::No; // If installed and current version, "Yes". If installed but outdated, "Outdated". If not installed, "No".

	//_is_loaded = state & SECUREUXTHEME_STATE_LOADED ? PatcherState::Yes : PatcherState::No;
	//_is_logonui = state & SECUREUXTHEME_STATE_LOGONUI_HOOKED ? PatcherState::Yes : PatcherState::No;
	//_is_explorer = state & SECUREUXTHEME_STATE_EXPLORER_HOOKED ? PatcherState::Yes : PatcherState::No;
	//_is_systemsettings = state & SECUREUXTHEME_STATE_SETTINGS_HOOKED ? PatcherState::Yes : PatcherState::No;

	//UpdatePatcherStateDisplay();
}





void MainDialog::UpdatePatcherStateDisplay()
{
	static constexpr std::pair<PatcherState MainDialog::*, HWND MainDialog::*> statics[]
	{
	  { &MainDialog::_is_installed,       &MainDialog::_hwnd_STATIC_VERSION    },
	  { &MainDialog::_is_loaded,          &MainDialog::_hwnd_STATIC_SYSTEM_PATCHED },
	  { &MainDialog::_is_logonui,         &MainDialog::_hwnd_STATIC_LOGONUI           },
	  { &MainDialog::_is_explorer,        &MainDialog::_hwnd_STATIC_BACKUPS           },
	  { &MainDialog::_is_systemsettings,  &MainDialog::_hwnd_STATIC_UPDATE_AVAILABLE  },
	};

	for (const auto& x : statics)
		SetWindowTextW(this->*x.second, PatcherStateText(this->*x.first));
}






void MainDialog::UpdateListItems()
{
	// lambda function for adding items (styles) to the 0th index (a.k.a beginning)
	const auto add_item = [this](LPCTSTR name, LPARAM lparam)
	{
		LVITEM lvitem
		{
		  LVIF_PARAM, INT_MAX, 0, 0, 0, (LPTSTR)_T("") // insert to a new row then the last row, in the first column
		};



		lvitem.lParam = lparam;
		const auto item = ListView_InsertItem(_hwnd_LIST, &lvitem);


		ListView_SetItemText(_hwnd_LIST, item, 0, (LPTSTR)name);
	};


	ListView_DeleteAllItems(_hwnd_LIST);

	ULONG pack_count{};
	icontool_get_pack_count(&pack_count);

	for (auto i = 0u; i < pack_count; ++i)
	{
		std::string pack{};
		icontool_get_pack(i, pack);

		wchar_t name[256]{};
		icontool_pack_get_display_name(pack, name, std::size(name));
		add_item(name, i);
	}



	// LVS_EX_AUTOSIZECOLUMNS just doesn't fucking work no matter where I put it
	ListView_SetColumnWidth(_hwnd_LIST, 0, LVSCW_AUTOSIZE); // autosize the list
}




MainDialog::MainDialog(HWND hDlg, void*) : _hwnd(hDlg)
{
	Log(ESTRt(L"Version " CI_VERSION));

	ULONG major = 0, minor = 0, build = 0;
	RtlGetNtVersionNumbers(&major, &minor, &build);
	Log(ESTRt(L"Running on %d.%d.%d flavor %01X"), major, minor, build & 0xFFFF, build >> 28);

	Log(ESTRt(L"Session user: %s Process user: %s"), _session_user.second.c_str(), _process_user.second.c_str());



	auto hicon = (HICON)LoadImageW(utl::get_instance(), MAKEINTRESOURCEW(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
	SendMessageW(_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
	SendMessageW(_hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon);

	Button_Enable(_hwnd_BUTTON_REMOVEPACK, FALSE);

	Button_Enable(_hwnd_BUTTON_PATCH, FALSE);
	Button_Enable(_hwnd_BUTTON_CLEAN_PATCH, FALSE);






	// The below line sets extended styles for the style list. extended styles are basically advanced settings for controls
	// LVS_EX_AUTOSIZECOLUMNS: Columns of the ListView should automatically size to fit the contents
	// LVS_EX_FULLROWSELECT: Entire row should be selected when an item is selected, rather than just the first column

	ListView_SetExtendedListViewStyle(_hwnd_LIST, LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT); // First flag specifies to automatically size colums to fit the contents; Second flag specifies to select all the columns in the row when an item is selected, not just the item that was selected.



	// This inserts a column so that items can be inserted to it, otherwise items won't be inserted. Note: This is not adding the column to a specific row, it is setting it to all rows. Note: The rows exist even if there are no items in them (I think). So, this line adds the column to all rows so that items can be added to them
	LVCOLUMN col{};
	ListView_InsertColumn(_hwnd_LIST, 0, &col);






	SendMessage(_hwnd_LIST, LVM_SETTEXTBKCOLOR, 0, (LPARAM)CLR_NONE); // Sets the background color for the text in the items of the ListView control to transparent, which allows the background color of the control itself to show through. I think this has no effect if you have the default aero.msstyles applied, but with custom styles applied, icontool style list will appear broken without this I'm pretty sure.



	//auto style = GetWindowStyle(_hwnd_LIST);
	//style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER;
	//SetWindowLongW(_hwnd_LIST, GWL_STYLE, style);







	UpdateListItems();

	UpdatePatcherState();
}






















void MainDialog::SelectItem(int id)
{
	if (id == -1) // invalid selection
	{
		Static_SetText(_hwnd_STATIC_PATH, ESTRt(L"Error: Invalid selection"));
		return;
	}


	std::string pack{};
	icontool_get_pack(id, pack);
	//if (FAILED(hr)) // getting theme failed
	//{
	//	Static_SetText(_hwnd_STATIC_PATH, _T(""));
	//	Log(ESTRt(L"SelectItem: icontool_get_theme(%d) failed with %08X"), id, hr);
	//	return;
	//}




	wchar_t path[MAX_PATH];
	icontool_pack_get_vs_path(pack, path, std::size(path)); // put style path into path variable



	//if (FAILED(hr)) // get stylepath failed
	//{
	//	Static_SetText(_hwnd_STATIC_PATH, _T("")); // empty

	//	Log(ESTRt(L"SelectItem: icontool_theme_get_vs_path failed with %08X"), hr);
	//	if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	//	{
	//		utl::FormattedMessageBox(_hwnd, ESTRt(L"Warning"), MB_OK | MB_ICONWARNING, ESTRt(L"Getting visual style path failed!\n\nThis is often caused by incorrectly installed themes. Please make sure you copied all files and folders from the theme distribution before opening an issue."));
	//	}

	//	return;
	//}







	Static_SetText(_hwnd_STATIC_PATH, path); // set "stylepath"

	Button_Enable(_hwnd_BUTTON_PATCH, TRUE); // enable the apply button, since an item has been selected
	Button_Enable(_hwnd_BUTTON_CLEAN_PATCH, TRUE); // enable the apply button, since an item has been selected
	Button_Enable(_hwnd_BUTTON_REMOVEPACK, TRUE); // // enable the remove pack button, since an item has been selected
}


















HRESULT MainDialog::PatchThemeInternal(int id)
{
//	Log(ESTRt(L"PatchThemeInternal(%d)"), id);
//
//	wchar_t path[MAX_PATH]{};
//	bool patched = true;
//
//
//
//	{
//		ITheme* theme = nullptr;
//		auto result = icontool_get_theme(id, &theme);
//		if (SUCCEEDED(result))
//		{
//			result = icontool_theme_get_vs_path(theme, path, std::size(path));
//			if (SUCCEEDED(result))
//			{
//				if (path[0] && icontool_signature_check(path) == E_FAIL)
//					patched = false;
//			}
//			else
//			{
//				Log(ESTRt(L"icontool_theme_get_vs_path failed with %08X"), result);
//			}
//		}
//		else
//		{
//			Log(ESTRt(L"icontool_get_theme(%d) failed with %08X"), id, result);
//			return result;
//		}
//	}
//
//
//
//
//	HRESULT fix_result = S_OK;
//	if (!patched && path[0])
//		fix_result = icontool_signature_fix(path);
//
//	return fix_result;
	return 0;
}











































void MainDialog::PatchTheme(int id) // this is basically a wrapper-like function that does some extra stuff, while the actual patching code is in the PatchThemeInternal function
{
	Log(ESTRt(L"PatchTheme(%d)"), id);

	if (id == -1)
		return; // invalid selection... whatever..

	const auto result = PatchThemeInternal(id);

	if (FAILED(result))
	{
		utl::FormattedMessageBox(_hwnd, ESTRt(L"Error"), MB_OK | MB_ICONERROR, ESTRt(L"Patching theme failed. The following error was encountered:\r\n%s\r\n%s"), utl::ErrorToString(result).c_str(), ESTRt(L"Consider sending a bug report"));
	}

	// reload theme details (patch status)
	SelectItem(id);
}







void MainDialog::RebuildCache() // this is basically a wrapper-like function that does some extra stuff, while the actual patching code is in the PatchThemeInternal function
{
	Log(ESTRt(L"RebuildCache"));

	//const auto result = PatchThemeInternal(id);

	//if (FAILED(result))
	//	utl::FormattedMessageBox(_hwnd, ESTRt(L"Error"), MB_OK | MB_ICONERROR, ESTRt(L"Patching theme failed. The following error was encountered:\r\n%s\r\n%s"), utl::ErrorToString(result).c_str(), ESTRt(L"Consider sending a bug report"));

	
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

	// Save the current file explorer windows
	std::vector<std::wstring> openWindows;
	utl::GetOpenWindows(openWindows);

	// Restart explorer (killing restarts it, because someone at microsoft thought that making the shell auto-reopen when you kill it was a good idea. See: https://stackoverflow.com/a/8617330 (comment on https://stackoverflow.com/questions/8610318/stop-explorer-process-completely-using-powershell) [It is about powershell, but it still applies here]
	DWORD explorerPID = 0;
	if (utl::FindProcessId(L"explorer.exe", explorerPID))
	{
		HANDLE explorerHandle = OpenProcess(PROCESS_TERMINATE, FALSE, explorerPID);
		if (explorerHandle != NULL)
		{
			TerminateProcess(explorerHandle, 2);
			CloseHandle(explorerHandle);
			WaitForSingleObject(explorerHandle, INFINITE);
		}

		// Reopen the saved file explorer windows
		for (const auto& window : openWindows)
			ShellExecute(NULL, L"open", window.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}








int MainDialog::CurrentSelection() // get selected item
{
	// if somehow multiple items selected, return
	const auto count = ListView_GetSelectedCount(_hwnd_LIST);
	if (count != 1)
	{
		Log(ESTRt(L"CurrentSelection: count is %d, expected 1"), count);
		return -1;
	}

	LVITEM item{};
	item.iItem = ListView_GetSelectionMark(_hwnd_LIST);
	item.mask = LVIF_PARAM; // attributes that we want to get/set, in this case it is LVIF_PARAM (a.k.a lParam)

	ListView_GetItem(_hwnd_LIST, &item); // set "item" to the selected item

	return (int)item.lParam; // return the index
}









INT_PTR MainDialog::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return FALSE; // When the dialog is initialized, return false, in order to not select the default control


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{


		case IDOK:
		case IDCLOSE:
		case IDCANCEL:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				DestroyWindow(_hwnd);
			}
			return TRUE;





		case IDC_BUTTON_HELP:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				MessageBoxW(_hwnd,(std::wstring{ ESTRt(kHelpText) } + ESTRt(kHelpText2)).c_str(), ESTRt(L"Help"), MB_OK); // Ignore the "expression must have a constant value" error, it is a code analysis bug or something.
			}
			return TRUE;




		case IDC_BUTTON_ADDPACK:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				OPENFILENAME ofn;       // common dialog box structure
				TCHAR szFile[260];       // buffer for file name
				HWND hwnd = GetActiveWindow();  // owner window
				HANDLE hf;              // file handle




				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hwnd;

				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0'; // set lpstrFile[0] to '\0' so that GetOpenFileName does not use the contents of szFile to initialize itself.
				
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = L"All Files (*.*)\0*.*\0Icon Pack (*.zip)\0*.zip\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;




				// Display the Open dialog box. 

				if (GetOpenFileName(&ofn) == TRUE)
				{
					hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

					//MessageBox(NULL, ofn.lpstrFile, L"File Information", MB_OK);

					auto filePath = ofn.lpstrFile;

					CloseHandle(hf); // you must close the handle otherwise you'll get a ERROR_SHARING_VIOLATION when using CopyFileW. Look here for more info: https://stackoverflow.com/questions/71615227/copyfileex-fails-with-error-sharing-violation-for-file-created-by-createfile


					TCHAR dest_path[MAX_PATH];
					
					DWORD path_len = GetEnvironmentVariable(L"PROGRAMDATA", dest_path, MAX_PATH);

					if (path_len == 0) {
						MessageBox(NULL, L"Error retrieving PROGRAMDATA environment variable", L"Error", MB_OK);
						return TRUE; // I probably should be returning false here, but doing so closes the fucking main dialog!!!
					}

					wcscat_s(dest_path, L"\\WinIconPatcher\\");

					

					// make backup of dest_path as data_folder, it is currently %programdata/WinIconPatcher. later we'll set it to the actual filepath where it should be copied as e.g. %programdata%/WinIconPatcher/my 7tsp pack.7z
					TCHAR data_folder[MAX_PATH];
					wcscpy_s(data_folder, dest_path);
					data_folder[MAX_PATH - 1] = L'\0';



					wcscat_s(dest_path, PathFindFileName(filePath));


					if (CreateDirectory(data_folder, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
					{
						// Creating folders successful


						if (!CopyFileW(filePath, dest_path, false)) // Copying files failed
						{
							MessageBoxW(NULL, L"Failed to copy files", L"Info", MB_OK);
						}
					}
					else
					{
						// Creating folders failed
						MessageBoxW(NULL, L"Failed to create data folder", L"Info", MB_OK);
					}




					// Ignore the code below, I kept it here because I might need it again

					// remove directory and all sub directories and files
					//filesystem::remove_all("folder");

				}


				UpdateListItems(); // Update the list, so that the newly added item is shown on the list
			}

			return TRUE;







		case IDC_BUTTON_REMOVEPACK:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				int id = CurrentSelection();

				if (id == -1) // invalid selection
				{
					Static_SetText(_hwnd_STATIC_PATH, ESTRt(L"Error: Invalid selection"));
					return TRUE; // I probably should be returning false here, but doing so closes the fucking main dialog!!!
				}


				std::string pack{};
				icontool_get_pack(id, pack);
				//if (FAILED(hr)) // getting theme failed
				//{
				//	Static_SetText(_hwnd_STATIC_PATH, _T(""));
				//	Log(ESTRt(L"SelectItem: icontool_get_theme(%d) failed with %08X"), id, hr);
				//	return;
				//}




				wchar_t path[MAX_PATH];
				icontool_pack_get_vs_path(pack, path, std::size(path)); // put style path into path variable

				DeleteFileW(path);

				UpdateListItems();
			}

			return TRUE;







		case IDC_BUTTON_RESTORE_ALL:
			if (HIWORD(wParam) == BN_CLICKED)
				//Install();
				MessageBoxW(NULL, ESTRt(L"Not implemented"), ESTRt(L"Info"), MB_OK);

			return TRUE;



		case IDC_BUTTON_RESTORE_FIRST:
			if (HIWORD(wParam) == BN_CLICKED)
				//Uninstall();
				MessageBoxW(NULL, ESTRt(L"Not implemented"), ESTRt(L"Info"), MB_OK);

			return TRUE;



		case IDC_BUTTON_PATCH:
			if (HIWORD(wParam) == BN_CLICKED)
				//PatchTheme(CurrentSelection());
				MessageBoxW(NULL, ESTRt(L"Not implemented"), ESTRt(L"Info"), MB_OK);

				return TRUE;


		case IDC_BUTTON_CLEAN_PATCH:
			if (HIWORD(wParam) == BN_CLICKED)
				//RestoreAllFiles();
				//PatchTheme(CurrentSelection());
				MessageBoxW(NULL, ESTRt(L"Not implemented"), ESTRt(L"Info"), MB_OK);

			return TRUE;



		case IDC_BUTTON_REBUILD_CACHE:
			if (HIWORD(wParam) == BN_CLICKED)
				RebuildCache();

			return TRUE;
		}
		break; // End of WM_COMMAND






	case WM_NOTIFY:
	{
		const auto nmhdr = (LPNMHDR)lParam;
		if (nmhdr->idFrom == IDC_LIST && nmhdr->code == LVN_ITEMCHANGED) // Selection changed in the IDC_LIST control
		{
			const auto pnmv = (LPNMLISTVIEW)lParam;
			if (pnmv->uNewState & LVIS_SELECTED)
			{
				SelectItem(pnmv->iItem); // Call SelectTheme
				return TRUE;
			}
		}
	}
	return FALSE;





	case WM_CLOSE:
		DestroyWindow(_hwnd);
		return TRUE;


	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;


	}


	return (INT_PTR) FALSE;
}
