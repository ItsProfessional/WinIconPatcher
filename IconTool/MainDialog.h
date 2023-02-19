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

#pragma once
#include "dlg.h"
#include "utl.h"

enum class PatcherState : size_t
{
	No,
	Yes,
	Probably,
	Outdated
};

class MainDialog
{
	HWND _hwnd;

	MAKE_IDC_MEMBER(_hwnd, BUTTON_CLEAN_PATCH);
	MAKE_IDC_MEMBER(_hwnd, BUTTON_PATCH);
	MAKE_IDC_MEMBER(_hwnd, BUTTON_HELP);
	MAKE_IDC_MEMBER(_hwnd, BUTTON_RESTORE_ALL);
	MAKE_IDC_MEMBER(_hwnd, BUTTON_RESTORE_FIRST);
	MAKE_IDC_MEMBER(_hwnd, BUTTON_RESTORE_BACKUP);
	MAKE_IDC_MEMBER(_hwnd, CHECKBOX_REBUILD_CACHE);
	MAKE_IDC_MEMBER(_hwnd, NOT_IMPLEMENTED_4);
	MAKE_IDC_MEMBER(_hwnd, NOT_IMPLEMENTED_2);
	MAKE_IDC_MEMBER(_hwnd, NOT_IMPLEMENTED_3);
	MAKE_IDC_MEMBER(_hwnd, NOT_IMPLEMENTED_5);
	MAKE_IDC_MEMBER(_hwnd, NOT_IMPLEMENTED_6);
	MAKE_IDC_MEMBER(_hwnd, LIST);
	MAKE_IDC_MEMBER(_hwnd, LOG);
	MAKE_IDC_MEMBER(_hwnd, STATIC_BACKUPS);
	MAKE_IDC_MEMBER(_hwnd, STATIC_VERSION);
	MAKE_IDC_MEMBER(_hwnd, STATIC_SYSTEM_PATCHED);
	MAKE_IDC_MEMBER(_hwnd, STATIC_LOGONUI);
	MAKE_IDC_MEMBER(_hwnd, STATIC_NEEDS_PATCHING);
	MAKE_IDC_MEMBER(_hwnd, STATIC_PATH);
	MAKE_IDC_MEMBER(_hwnd, STATIC_UPDATE_AVAILABLE);
	MAKE_IDC_MEMBER(_hwnd, BUTTON_ADDPACK);
	MAKE_IDC_MEMBER(_hwnd, BUTTON_REMOVEPACK);

	PatcherState _is_installed;
	PatcherState _is_loaded;
	PatcherState _is_explorer;
	PatcherState _is_systemsettings;
	PatcherState _is_logonui;

	const std::pair<std::wstring, std::wstring> _process_user = utl::process_user();
	const std::pair<std::wstring, std::wstring> _session_user = utl::session_user();

	std::list<std::wstring> _names;

	void Log(const wchar_t* fmt, ...);

	void Uninstall();
	void Install();
	void UpdatePatcherState();
	void UpdatePatcherStateDisplay();

	void UpdateListItems();

	HRESULT PatchThemeInternal(int id);
	void SelectItem(int id);
	void PatchTheme(int id);
	void RebuildCache();
	int CurrentSelection();

public:
	MainDialog(HWND hDlg, void*);

	INT_PTR DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};